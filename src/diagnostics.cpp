#include "diagnostics.h"
#include "config.h"
#include <WiFi.h>
#include <LittleFS.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_system.h>
#include <math.h>

Diagnostics& Diagnostics::instance() {
    static Diagnostics inst;
    return inst;
}

void Diagnostics::addResult(const char* name, const char* cat, bool pass,
                            float val, const char* unit, const char* detail) {
    if (_resultCount >= MAX_RESULTS) return;
    _results[_resultCount++] = {name, cat, pass, val, unit, detail};
}

void Diagnostics::start() {
    _status = DiagStatus::RUNNING;
    _progress = 0;
    _step = 0;
    _resultCount = 0;
    _currentTest = "Initializing";
}

void Diagnostics::tick() {
    if (_status != DiagStatus::RUNNING) return;

    switch (_step) {
        case 0:  _currentTest = "Chip Info";      testChipInfo();      _progress = 8;  break;
        case 1:  _currentTest = "Flash";          testFlash();         _progress = 16; break;
        case 2:  _currentTest = "PSRAM";          testPsram();         _progress = 24; break;
        case 3:  _currentTest = "Heap";           testHeap();          _progress = 32; break;
        case 4:  _currentTest = "Temperature";    testTemperature();   _progress = 40; break;
        case 5:  _currentTest = "ADC";            testAdc();           _progress = 48; break;
        case 6:  _currentTest = "GPIO";           testGpio();          _progress = 56; break;
        case 7:  _currentTest = "USB";            testUsb();           _progress = 64; break;
        case 8:  _currentTest = "WiFi";           testWifi();          _progress = 72; break;
        case 9:  _currentTest = "CPU Benchmark";  benchCpu();          _progress = 80; break;
        case 10: _currentTest = "Float Benchmark";benchFloat();        _progress = 86; break;
        case 11: _currentTest = "Memcpy Benchmark";benchMemcpy();       _progress = 92; break;
        case 12: _currentTest = "PSRAM Benchmark";benchPsram();         _progress = 96; break;
        case 13: _currentTest = "LittleFS";       benchLittleFS();     _progress = 100; break;
        default:
            _status = DiagStatus::DONE;
            _currentTest = "Complete";
            return;
    }
    _step++;
}

// ── Individual tests ───────────────────────────────────────────────

void Diagnostics::testChipInfo() {
    esp_chip_info_t ci;
    esp_chip_info(&ci);

    addResult("CPU Cores",     "CPU", true,  ci.cores,              "cores", nullptr);
    addResult("CPU Frequency", "CPU", true,  getCpuFrequencyMhz(),  "MHz",   nullptr);
    addResult("Chip Revision", "CPU", true,  ci.revision,           "",      nullptr);
    addResult("WiFi",          "CPU", (ci.features & CHIP_FEATURE_WIFI_BGN) != 0, 1, "", nullptr);
    addResult("BLE",           "CPU", (ci.features & CHIP_FEATURE_BLE) != 0, 1, "", nullptr);
}

void Diagnostics::testFlash() {
    uint32_t size = 0;
    esp_flash_get_size(NULL, &size);
    float mb = size / 1024.0f / 1024.0f;
    bool ok = size > 0;
    addResult("Flash Size", "Flash", ok, mb, "MB", nullptr);

    // Simple read test at a safe offset
    uint8_t buf[4] = {0};
    esp_err_t err = esp_flash_read(NULL, buf, DIAG_FLASH_TEST_ADDR, 4);
    addResult("Flash Read", "Flash", err == ESP_OK, 1, "", err == ESP_OK ? "OK" : "FAIL");
}

void Diagnostics::testPsram() {
    bool found = psramFound();
    if (found) {
        float mb = ESP.getPsramSize() / 1024.0f / 1024.0f;
        addResult("PSRAM Present", "PSRAM", true, 1, "", "YES");
        addResult("PSRAM Size",    "PSRAM", true, mb, "MB", nullptr);

        // Allocate and write test
        uint8_t* buf = (uint8_t*)ps_malloc(1024);
        bool allocOk = buf != nullptr;
        if (allocOk) {
            memset(buf, 0xAA, 1024);
            bool match = buf[512] == 0xAA;
            free(buf);
            addResult("PSRAM R/W", "PSRAM", match, 1, "", match ? "PASS" : "FAIL");
        } else {
            addResult("PSRAM R/W", "PSRAM", false, 0, "", "ALLOC FAIL");
        }
    } else {
        addResult("PSRAM Present", "PSRAM", false, 0, "", "NO");
    }
}

void Diagnostics::testHeap() {
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t heapSize = ESP.getHeapSize();
    bool ok = freeHeap > 50000;
    addResult("Free Heap",  "Memory", ok, freeHeap,  "bytes", nullptr);
    addResult("Heap Size",  "Memory", true, heapSize, "bytes", nullptr);
}

void Diagnostics::testTemperature() {
    float temp = temperatureRead();
    bool ok = !isnan(temp) && temp > 10.0f && temp < 90.0f;
    addResult("Temperature", "System", ok, temp, "°C", nullptr);
}

void Diagnostics::testAdc() {
    // ESP32-S3 ADC1 on GPIO4
    int raw = analogRead(4);
    bool ok = raw >= 0 && raw <= 4095;
    addResult("ADC GPIO4", "ADC", ok, raw, "raw", nullptr);
}

void Diagnostics::testGpio() {
    // Test a safe output pin (GPIO48 — onboard RGB on many S3 boards)
    const uint8_t testPin = 48;
    pinMode(testPin, OUTPUT);
    digitalWrite(testPin, HIGH);
    delayMicroseconds(10);
    int level = digitalRead(testPin);
    digitalWrite(testPin, LOW);
    bool ok = level == HIGH;
    addResult("GPIO Output", "GPIO", ok, testPin, "GPIO", ok ? "PASS" : "FAIL");
}

void Diagnostics::testUsb() {
#if CONFIG_TINYUSB_CDC_ENABLED || ARDUINO_USB_CDC_ON_BOOT
    addResult("USB CDC", "USB", true, 1, "", "ON BOOT");
#else
    addResult("USB CDC", "USB", false, 0, "", "DISABLED");
#endif
    addResult("USB OTG", "USB", true, 1, "", "S3 NATIVE");
}

void Diagnostics::testWifi() {
    uint64_t mac = ESP.getEfuseMac();
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             (uint8_t)(mac >> 40), (uint8_t)(mac >> 32),
             (uint8_t)(mac >> 24), (uint8_t)(mac >> 16),
             (uint8_t)(mac >> 8),  (uint8_t)(mac));

    bool validMac = !(macStr[0] == '0' && macStr[1] == '0' && macStr[3] == '0');
    addResult("MAC Address", "Network", validMac, 1, "", macStr);
    addResult("AP Active",   "Network", WiFi.softAPgetStationNum() >= 0, 1, "", AP_SSID);
}

// ── Benchmarks ───────────────────────────────────────────────────

void Diagnostics::benchCpu() {
    volatile uint32_t sum = 0;
    uint32_t start = micros();
    for (uint32_t i = 0; i < DIAG_CPU_ITERATIONS; i++) {
        sum += i;
    }
    uint32_t elapsed = micros() - start;
    float opsPerSec = (float)DIAG_CPU_ITERATIONS / elapsed * 1000000.0f;
    addResult("CPU Integer", "Benchmark", true, opsPerSec, "ops/s", nullptr);
    (void)sum;
}

void Diagnostics::benchFloat() {
    volatile float sum = 0.0f;
    uint32_t start = micros();
    for (uint32_t i = 0; i < 100000; i++) {
        sum += sinf((float)i * 0.001f);
    }
    uint32_t elapsed = micros() - start;
    float opsPerSec = 100000.0f / elapsed * 1000000.0f;
    addResult("CPU Float", "Benchmark", true, opsPerSec, "ops/s", nullptr);
    (void)sum;
}

void Diagnostics::benchMemcpy() {
    uint8_t* src = (uint8_t*)malloc(DIAG_MEMCPY_SIZE);
    uint8_t* dst = (uint8_t*)malloc(DIAG_MEMCPY_SIZE);
    if (!src || !dst) {
        addResult("Memcpy", "Benchmark", false, 0, "KB/s", "ALLOC FAIL");
        free(src); free(dst);
        return;
    }
    memset(src, 0x55, DIAG_MEMCPY_SIZE);

    uint32_t start = micros();
    for (int i = 0; i < 100; i++) {
        memcpy(dst, src, DIAG_MEMCPY_SIZE);
    }
    uint32_t elapsed = micros() - start;
    float kbps = (float)(DIAG_MEMCPY_SIZE * 100) / elapsed * 1000000.0f / 1024.0f;
    addResult("Memcpy", "Benchmark", true, kbps, "KB/s", nullptr);
    free(src); free(dst);
}

void Diagnostics::benchPsram() {
    if (!psramFound()) {
        addResult("PSRAM Memcpy", "Benchmark", false, 0, "KB/s", "NO PSRAM");
        return;
    }
    uint8_t* src = (uint8_t*)ps_malloc(DIAG_MEMCPY_SIZE);
    uint8_t* dst = (uint8_t*)ps_malloc(DIAG_MEMCPY_SIZE);
    if (!src || !dst) {
        addResult("PSRAM Memcpy", "Benchmark", false, 0, "KB/s", "ALLOC FAIL");
        free(src); free(dst);
        return;
    }
    memset(src, 0xAA, DIAG_MEMCPY_SIZE);

    uint32_t start = micros();
    for (int i = 0; i < 100; i++) {
        memcpy(dst, src, DIAG_MEMCPY_SIZE);
    }
    uint32_t elapsed = micros() - start;
    float kbps = (float)(DIAG_MEMCPY_SIZE * 100) / elapsed * 1000000.0f / 1024.0f;
    addResult("PSRAM Memcpy", "Benchmark", true, kbps, "KB/s", nullptr);
    free(src); free(dst);
}

void Diagnostics::benchLittleFS() {
    if (!LittleFS.begin(false)) {
        addResult("LittleFS", "Benchmark", false, 0, "KB/s", "NOT MOUNTED");
        return;
    }
    const char* testFile = "/.bench_tmp";
    uint8_t buf[512];
    memset(buf, 0xCC, sizeof(buf));

    File f = LittleFS.open(testFile, "w");
    if (!f) {
        addResult("LittleFS Write", "Benchmark", false, 0, "KB/s", "OPEN FAIL");
        return;
    }
    uint32_t start = micros();
    for (int i = 0; i < 200; i++) {
        f.write(buf, sizeof(buf));
    }
    f.close();
    uint32_t elapsed = micros() - start;
    float kbps = (512.0f * 200) / elapsed * 1000000.0f / 1024.0f;
    LittleFS.remove(testFile);
    addResult("LittleFS Write", "Benchmark", true, kbps, "KB/s", nullptr);
}

// ── Serialization ────────────────────────────────────────────────

void Diagnostics::serializeInfo(JsonObject obj) {
    esp_chip_info_t ci;
    esp_chip_info(&ci);

    uint32_t flashSize = 0;
    esp_flash_get_size(NULL, &flashSize);

    uint64_t mac = ESP.getEfuseMac();
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             (uint8_t)(mac >> 40), (uint8_t)(mac >> 32),
             (uint8_t)(mac >> 24), (uint8_t)(mac >> 16),
             (uint8_t)(mac >> 8),  (uint8_t)(mac));

    obj["board"]       = BOARD_NAME;
    obj["cores"]       = ci.cores;
    obj["revision"]    = ci.revision;
    obj["cpu_mhz"]     = getCpuFrequencyMhz();
    obj["flash_mb"]    = flashSize / 1024.0 / 1024.0;
    obj["psram_mb"]    = psramFound() ? ESP.getPsramSize() / 1024.0 / 1024.0 : 0;
    obj["free_heap"]   = ESP.getFreeHeap();
    obj["heap_size"]   = ESP.getHeapSize();
    obj["free_psram"]  = psramFound() ? ESP.getFreePsram() : 0;
    obj["mac"]         = macStr;
    obj["sdk"]         = ESP.getSdkVersion();
    obj["chip_id"]     = String((uint16_t)(ESP.getEfuseMac() >> 32), HEX) +
                         String((uint32_t)ESP.getEfuseMac(), HEX);
    obj["wifi"]        = (ci.features & CHIP_FEATURE_WIFI_BGN) != 0;
    obj["ble"]         = (ci.features & CHIP_FEATURE_BLE) != 0;
    obj["usb_otg"]     = true;
}

void Diagnostics::serializeResults(JsonArray arr) {
    for (uint8_t i = 0; i < _resultCount; i++) {
        JsonObject r = arr.add<JsonObject>();
        r["name"]     = _results[i].name;
        r["category"] = _results[i].category;
        r["pass"]     = _results[i].pass;
        r["value"]    = _results[i].value;
        r["unit"]     = _results[i].unit;
        if (_results[i].detail) r["detail"] = _results[i].detail;
    }
}

void Diagnostics::serializeScore(JsonObject obj) {
    uint8_t passed = 0;
    for (uint8_t i = 0; i < _resultCount; i++) {
        if (_results[i].pass) passed++;
    }
    float pct = _resultCount > 0 ? (float)passed / _resultCount * 100.0f : 0;
    obj["passed"]  = passed;
    obj["total"]   = _resultCount;
    obj["percent"] = pct;
}