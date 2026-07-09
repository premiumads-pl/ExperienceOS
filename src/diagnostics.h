#pragma once
#include <ArduinoJson.h>

enum class DiagStatus : uint8_t {
    IDLE,
    RUNNING,
    DONE,
    ERROR
};

struct DiagResult {
    const char* name;
    const char* category;
    bool        pass;
    float       value;
    const char* unit;
    const char* detail;
};

class Diagnostics {
public:
    static Diagnostics& instance();

    DiagStatus status() const { return _status; }
    uint8_t    progress() const { return _progress; }
    const char* currentTest() const { return _currentTest; }

    void start();
    void tick();  // call from loop when RUNNING

    void serializeInfo(JsonObject obj);
    void serializeResults(JsonArray arr);
    void serializeScore(JsonObject obj);

private:
    Diagnostics() = default;

    DiagStatus _status = DiagStatus::IDLE;
    uint8_t    _progress = 0;
    const char* _currentTest = "";
    uint8_t    _step = 0;

    static constexpr uint8_t MAX_RESULTS = 24;
    DiagResult _results[MAX_RESULTS];
    uint8_t    _resultCount = 0;

    void addResult(const char* name, const char* cat, bool pass,
                   float val, const char* unit, const char* detail);

    void testChipInfo();
    void testFlash();
    void testPsram();
    void testHeap();
    void testTemperature();
    void testAdc();
    void testGpio();
    void testUsb();
    void testWifi();
    void benchCpu();
    void benchFloat();
    void benchMemcpy();
    void benchPsram();
    void benchLittleFS();
};