#include "ota.h"
#include "config.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

static OtaState _state = OtaState::IDLE;
static bool _staOnline = false;
static bool _pendingCheck = false;
static bool _pendingUpdate = false;

static String _remoteVer;
static bool _updateAvail = false;
static int _httpCode = 0;
static String _msg;

static String ghBase() {
    return "https://raw.githubusercontent.com/" GH_OWNER "/" GH_REPO "/" GH_BRANCH "/";
}

void otaSetStaOnline(bool online) {
    _staOnline = online;
}

bool otaStaOnline() {
    return _staOnline;
}

bool otaStaConfigured() {
    return strlen(STA_SSID) > 0;
}

OtaState otaGetState() { return _state; }
const char* otaCurrentVer() { return FW_VERSION; }
const char* otaRemoteVer() { return _remoteVer.c_str(); }
bool otaUpdateAvail() { return _updateAvail; }
int otaHttpCode() { return _httpCode; }
const char* otaMsg() { return _msg.c_str(); }

void otaRequestCheck() {
    if (_state == OtaState::IDLE) {
        _pendingCheck = true;
    }
}

void otaRequestUpdate() {
    if (_state == OtaState::IDLE) {
        _pendingUpdate = true;
    }
}

static void doCheck() {
    _state = OtaState::CHECKING;
    _msg = "";
    _remoteVer = "";
    _updateAvail = false;
    _httpCode = 0;

    if (!_staOnline) {
        _msg = "Brak polaczenia STA";
        _state = OtaState::IDLE;
        return;
    }

    Serial.println("[GH] Sprawdzam wersje...");

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    String url = ghBase() + GH_VER_PATH + "?nc=" + String(millis());

    if (!http.begin(client, url)) {
        _msg = "HTTP begin failed";
        _state = OtaState::IDLE;
        return;
    }

    _httpCode = http.GET();

    if (_httpCode == 200) {
        _remoteVer = http.getString();
        _remoteVer.trim();
        _updateAvail = (_remoteVer.length() > 0 && _remoteVer != String(FW_VERSION));
        Serial.printf("[GH] Wersja lokalna: %s, zdalna: %s, dostepna: %s\n",
                      FW_VERSION, _remoteVer.c_str(), _updateAvail ? "TAK" : "NIE");
    } else {
        _msg = "HTTP " + String(_httpCode);
        Serial.printf("[GH] Blad sprawdzania: %s\n", _msg.c_str());
    }

    http.end();
    _state = OtaState::IDLE;
}

static void doUpdate() {
    _state = OtaState::UPDATING;
    _msg = "";

    if (!_staOnline) {
        _msg = "Brak polaczenia STA";
        _state = OtaState::IDLE;
        return;
    }

    if (!_updateAvail) {
        _msg = "Brak nowej wersji";
        _state = OtaState::IDLE;
        return;
    }

    Serial.println("[GH] Pobieram firmware...");

    WiFiClientSecure client;
    client.setInsecure();

    httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    httpUpdate.rebootOnUpdate(true);

    String url = ghBase() + GH_FW_PATH + "?nc=" + String(millis());
    t_httpUpdate_return ret = httpUpdate.update(client, url);

    if (ret != HTTP_UPDATE_OK) {
        _msg = "BLAD " + String(httpUpdate.getLastError()) + ": " +
               httpUpdate.getLastErrorString();
        Serial.printf("[GH] %s\n", _msg.c_str());
    }

    _state = OtaState::IDLE;
}

void otaLoop() {
    if (_state != OtaState::IDLE) return;

    if (_pendingCheck) {
        _pendingCheck = false;
        doCheck();
        return;
    }

    if (_pendingUpdate) {
        _pendingUpdate = false;
        doUpdate();
    }
}