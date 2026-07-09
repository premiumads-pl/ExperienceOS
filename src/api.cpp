#include "api.h"
#include "diagnostics.h"
#include "gpio_map.h"
#include <ArduinoJson.h>

static void sendJson(AsyncWebServerRequest* req, JsonDocument& doc) {
    String out;
    serializeJson(doc, out);
    req->send(200, "application/json", out);
}

void setupApiRoutes(AsyncWebServer& server) {

    // Static system info (no tests needed)
    server.on("/api/info", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonObject info = doc["info"].to<JsonObject>();
        Diagnostics::instance().serializeInfo(info);
        sendJson(req, doc);
    });

    // Start diagnostics run
    server.on("/api/diagnostics/start", HTTP_POST, [](AsyncWebServerRequest* req) {
        auto& diag = Diagnostics::instance();
        if (diag.status() == DiagStatus::RUNNING) {
            req->send(409, "application/json", "{\"error\":\"already running\"}");
            return;
        }
        diag.start();
        req->send(200, "application/json", "{\"status\":\"started\"}");
    });

    // Poll diagnostics progress + results
    server.on("/api/diagnostics/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        auto& diag = Diagnostics::instance();
        JsonDocument doc;

        const char* statusStr = "idle";
        switch (diag.status()) {
            case DiagStatus::RUNNING: statusStr = "running"; break;
            case DiagStatus::DONE:    statusStr = "done";    break;
            case DiagStatus::ERROR:   statusStr = "error";   break;
            default: break;
        }

        doc["status"]   = statusStr;
        doc["progress"] = diag.progress();
        doc["current"]  = diag.currentTest();

        JsonArray results = doc["results"].to<JsonArray>();
        diag.serializeResults(results);

        JsonObject score = doc["score"].to<JsonObject>();
        diag.serializeScore(score);

        sendJson(req, doc);
    });

    // GPIO pin map
    server.on("/api/gpio", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray pins = doc["pins"].to<JsonArray>();
        serializePinMap(pins);
        sendJson(req, doc);
    });

    // Single pin detail
    server.on("/api/pin", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!req->hasParam("gpio")) {
            req->send(400, "application/json", "{\"error\":\"missing gpio param\"}");
            return;
        }
        uint8_t gpio = req->getParam("gpio")->value().toInt();
        const PinCapability* pin = findPin(gpio);

        JsonDocument doc;
        if (!pin) {
            doc["found"] = false;
            doc["gpio"]  = gpio;
        } else {
            doc["found"]     = true;
            doc["gpio"]      = pin->gpio;
            doc["adc"]       = pin->adc;
            doc["dac"]       = pin->dac;
            doc["touch"]     = pin->touch;
            doc["pwm"]       = pin->pwm;
            doc["i2c"]       = pin->i2c;
            doc["spi"]       = pin->spi;
            doc["uart"]      = pin->uart;
            doc["strapping"] = pin->strapping;
            doc["usb"]       = pin->usb;
            doc["jtag"]      = pin->jtag;
            doc["octal"]     = pin->octal;
            doc["label"]     = pin->label;
        }
        sendJson(req, doc);
    });
}