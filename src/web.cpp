#include "web.h"
#include "api.h"
#include <LittleFS.h>

bool initLittleFS() {
    if (!LittleFS.begin(true)) {
        Serial.println("[FS] LittleFS mount failed!");
        return false;
    }
    Serial.println("[FS] LittleFS mounted");
    return true;
}

void setupWebServer(AsyncWebServer& server) {
    // Serve static files from LittleFS /data
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    // API routes
    setupApiRoutes(server);

    // 404 fallback
    server.onNotFound([](AsyncWebServerRequest* req) {
        req->send(404, "text/plain", "Not Found");
    });
}