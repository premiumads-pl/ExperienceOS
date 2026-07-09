#pragma once

#if __has_include("fw_version.h")
  #include "fw_version.h"
#else
  #define FW_VERSION "0.1-dev"
#endif

#if __has_include("secrets.h")
  #include "secrets.h"
#endif
#ifndef STA_SSID
  #define STA_SSID     ""
  #define STA_PASSWORD ""
#endif

// ── GitHub OTA ───────────────────────────────────────────────────
#define GH_OWNER    "premiumads-pl"
#define GH_REPO     "ExperienceOS"
#define GH_BRANCH   "main"
#define GH_FW_PATH  "firmware/firmware.bin"
#define GH_VER_PATH "firmware/version.txt"

// ── WiFi Access Point ──────────────────────────────────────────────
#define AP_SSID       "ExperienceOS"
#define AP_PASSWORD   ""          // open AP for easy phone access
#define AP_CHANNEL    1
#define AP_MAX_CLIENTS 4

// ── Web Server ───────────────────────────────────────────────────
#define WEB_PORT      80

// ── Diagnostics ──────────────────────────────────────────────────
#define DIAG_HEAP_ITERATIONS   10000
#define DIAG_MEMCPY_SIZE       4096
#define DIAG_CPU_ITERATIONS    500000
#define DIAG_FLASH_TEST_ADDR   0x3D0000   // safe offset in flash

// ── Board identity (auto-detected at runtime) ──────────────────────
#define BOARD_NAME    "ESP32-S3"