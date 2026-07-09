#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

struct PinCapability {
    uint8_t  gpio;
    bool     adc;
    bool     dac;
    bool     touch;
    bool     pwm;
    bool     i2c;
    bool     spi;
    bool     uart;
    bool     strapping;
    bool     usb;
    bool     jtag;
    bool     octal;
    const char* label;
};

// ESP32-S3 pin capabilities (DevKit-class layout)
static const PinCapability PIN_MAP[] = {
    { 0,  true,  false, true,  true,  false, false, true,  true,  false, true,  false, "Boot / Touch" },
    { 1,  false, false, false, true,  false, false, true,  false, false, false, false, "UART0 TX" },
    { 2,  false, false, false, true,  false, false, true,  false, false, false, false, "UART0 RX" },
    { 3,  true,  false, true,  true,  false, false, true,  false, false, false, false, "JTAG EN" },
    { 4,  true,  false, true,  true,  false, false, true,  false, false, false, false, "ADC1_CH3" },
    { 5,  true,  false, true,  true,  false, false, true,  false, false, false, false, "ADC1_CH4" },
    { 6,  true,  false, true,  true,  false, false, true,  false, false, false, false, "ADC1_CH5" },
    { 7,  true,  false, true,  true,  false, false, true,  false, false, false, false, "ADC1_CH6" },
    { 8,  false, false, false, true,  false, false, true,  false, false, false, false, "" },
    { 9,  false, false, false, true,  false, false, true,  false, false, false, false, "" },
    { 10, false, false, false, true,  false, false, true,  false, false, false, false, "" },
    { 11, false, false, false, true,  false, true,  true,  false, false, false, false, "SPI FSPI" },
    { 12, false, false, false, true,  false, true,  true,  false, false, false, false, "SPI FSPI" },
    { 13, false, false, false, true,  false, true,  true,  false, false, false, false, "SPI FSPI" },
    { 14, false, false, false, true,  false, true,  true,  false, false, false, false, "SPI FSPI" },
    { 15, false, false, false, true,  false, false, true,  false, false, false, false, "" },
    { 16, false, false, false, true,  false, false, true,  false, false, false, false, "UART1 RX" },
    { 17, false, false, false, true,  false, false, true,  false, false, false, false, "UART1 TX" },
    { 18, false, false, false, true,  false, true,  true,  false, false, false, false, "SPI FSPI" },
    { 19, false, false, false, true,  true,  true,  true,  false, true,  false, false, "USB D-" },
    { 20, false, false, false, true,  true,  true,  true,  false, true,  false, false, "USB D+" },
    { 21, false, false, false, true,  true,  false, true,  false, false, false, false, "I2C SDA" },
    { 33, false, false, false, true,  false, false, true,  false, false, false, true,  "Octal PSRAM" },
    { 34, false, false, false, true,  false, false, true,  false, false, false, true,  "Octal PSRAM" },
    { 35, false, false, false, true,  false, false, true,  false, false, false, true,  "Octal PSRAM" },
    { 36, false, false, false, true,  false, false, true,  false, false, false, true,  "Octal PSRAM" },
    { 37, false, false, false, true,  false, false, true,  false, false, false, true,  "Octal PSRAM" },
    { 38, false, false, false, true,  false, false, true,  false, false, false, true,  "Octal FSPI" },
    { 39, false, false, false, true,  false, false, true,  false, false, false, true,  "Octal FSPI" },
    { 40, false, false, false, true,  false, false, true,  false, false, false, true,  "Octal FSPI" },
    { 41, false, false, false, true,  false, false, true,  false, false, false, true,  "Octal FSPI" },
    { 42, false, false, false, true,  false, false, true,  false, false, false, false, "" },
    { 43, false, false, false, true,  false, false, true,  true,  false, false, false, "UART0 TX" },
    { 44, false, false, false, true,  false, false, true,  true,  false, false, false, "UART0 RX" },
    { 45, false, false, false, true,  false, false, true,  true,  false, false, false, "Strapping" },
    { 46, false, false, false, true,  false, false, true,  true,  false, false, false, "Strapping" },
    { 47, false, false, false, true,  false, false, true,  false, false, false, false, "" },
    { 48, false, false, false, true,  false, false, true,  false, false, false, false, "RGB LED" },
};

static const size_t PIN_MAP_SIZE = sizeof(PIN_MAP) / sizeof(PIN_MAP[0]);

inline void serializePinMap(JsonArray arr) {
    for (size_t i = 0; i < PIN_MAP_SIZE; i++) {
        const auto& p = PIN_MAP[i];
        JsonObject obj = arr.add<JsonObject>();
        obj["gpio"]      = p.gpio;
        obj["adc"]       = p.adc;
        obj["dac"]       = p.dac;
        obj["touch"]     = p.touch;
        obj["pwm"]       = p.pwm;
        obj["i2c"]       = p.i2c;
        obj["spi"]       = p.spi;
        obj["uart"]      = p.uart;
        obj["strapping"] = p.strapping;
        obj["usb"]       = p.usb;
        obj["jtag"]      = p.jtag;
        obj["octal"]     = p.octal;
        obj["label"]     = p.label;
    }
}

inline const PinCapability* findPin(uint8_t gpio) {
    for (size_t i = 0; i < PIN_MAP_SIZE; i++) {
        if (PIN_MAP[i].gpio == gpio) return &PIN_MAP[i];
    }
    return nullptr;
}