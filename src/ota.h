#pragma once
#include <Arduino.h>

enum class OtaState : uint8_t {
    IDLE,
    CHECKING,
    UPDATING
};

void otaSetStaOnline(bool online);
bool otaStaOnline();

void otaRequestCheck();
void otaRequestUpdate();
void otaLoop();

OtaState otaGetState();
const char* otaCurrentVer();
const char* otaRemoteVer();
bool otaUpdateAvail();
int otaHttpCode();
const char* otaMsg();
bool otaStaConfigured();