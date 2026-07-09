#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "web.h"
#include "diagnostics.h"
#include "ota.h"

AsyncWebServer server(WEB_PORT);

static bool staUp = false;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================================");
    Serial.println("       ExperienceOS Hardware Explorer v0.1");
    Serial.printf( "       Firmware: %s\n", FW_VERSION);
    Serial.println("========================================================");

    initLittleFS();

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, 0, AP_MAX_CLIENTS);

    IPAddress ip = WiFi.softAPIP();
    Serial.printf("[WiFi] AP started: %s\n", AP_SSID);
    Serial.printf("[WiFi] AP IP:      %s\n", ip.toString().c_str());

    if (strlen(STA_SSID) > 0) {
        Serial.printf("[WiFi] STA connecting: %s\n", STA_SSID);
        WiFi.begin(STA_SSID, STA_PASSWORD);
    } else {
        Serial.println("[WiFi] STA not configured (no secrets.h)");
    }

    uint64_t mac = ESP.getEfuseMac();
    Serial.printf("[WiFi] MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  (uint8_t)(mac >> 40), (uint8_t)(mac >> 32),
                  (uint8_t)(mac >> 24), (uint8_t)(mac >> 16),
                  (uint8_t)(mac >> 8),  (uint8_t)(mac));

    setupWebServer(server);
    server.begin();
    Serial.printf("[Web]  Server running on http://%s\n", ip.toString().c_str());
    Serial.println("========================================================");
    Serial.println("  Connect phone to WiFi: ExperienceOS");
    Serial.println("  Open browser: http://192.168.4.1");
    Serial.println("========================================================");
}

void loop() {
    if (!staUp && WiFi.status() == WL_CONNECTED) {
        staUp = true;
        otaSetStaOnline(true);
        Serial.printf("[WiFi] STA connected: %s\n", WiFi.localIP().toString().c_str());
    } else if (staUp && WiFi.status() != WL_CONNECTED) {
        staUp = false;
        otaSetStaOnline(false);
        Serial.println("[WiFi] STA disconnected");
    }

    otaLoop();

    if (Diagnostics::instance().status() == DiagStatus::RUNNING) {
        Diagnostics::instance().tick();
    }

    delay(10);
}