#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "web.h"
#include "diagnostics.h"

AsyncWebServer server(WEB_PORT);

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================================");
    Serial.println("       ExperienceOS Hardware Explorer v0.1");
    Serial.println("========================================================");

    // Mount LittleFS
    initLittleFS();

    // Start Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, 0, AP_MAX_CLIENTS);

    IPAddress ip = WiFi.softAPIP();
    Serial.printf("[WiFi] AP started: %s\n", AP_SSID);
    Serial.printf("[WiFi] IP address:  %s\n", ip.toString().c_str());

    uint64_t mac = ESP.getEfuseMac();
    Serial.printf("[WiFi] MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  (uint8_t)(mac >> 40), (uint8_t)(mac >> 32),
                  (uint8_t)(mac >> 24), (uint8_t)(mac >> 16),
                  (uint8_t)(mac >> 8),  (uint8_t)(mac));

    // Start web server
    setupWebServer(server);
    server.begin();
    Serial.printf("[Web]  Server running on http://%s\n", ip.toString().c_str());
    Serial.println("========================================================");
    Serial.println("  Connect phone to WiFi: ExperienceOS");
    Serial.println("  Open browser: http://192.168.4.1");
    Serial.println("========================================================");
}

void loop() {
    // Run diagnostics one step per loop iteration
    if (Diagnostics::instance().status() == DiagStatus::RUNNING) {
        Diagnostics::instance().tick();
    }
    delay(10);
}