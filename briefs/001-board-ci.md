# Brief 001 — Konfiguracja płytki + partycje OTA + CI (PlatformIO)

## Cel
Doprowadzić projekt do stanu, w którym: (a) kompiluje się i uruchamia na realnej płytce
**ESP32-S3-R2 (4 MB flash + 2 MB QSPI PSRAM)** bez crasha, (b) GitHub Actions buduje firmware
i publikuje `firmware/firmware.bin` + `firmware/version.txt` (fundament pod OTA z GitHub — Brief 002).

## Kontekst / ograniczenia
- Płytka: ESP32-S3-R2 — **4 MB flash, 2 MB QSPI PSRAM** (NIE 16 MB, NIE OPI).
- Obecny `platformio.ini` jest BŁĘDNY: `flash_size=16MB`, `default_16MB.csv`, `memory_type=qio_opi`.
- Repo: `premiumads-pl/ExperienceOS`, `main` = ten projekt. Stary dashboard w tagu `dashboard-v4`.
- Wersjonowanie OTA: `version.txt` = numer builda CI (jak w dashboard-v4).

## Zakres (co zrobić)
1. `platformio.ini`:
   - `board_upload.flash_size = 4MB`
   - `board_build.arduino.memory_type = qio_qspi` (QSPI, NIE opi)
   - `board_build.partitions = partitions/otaapp_4mb.csv`
   - zostaw: `framework=arduino`, LittleFS, `-DBOARD_HAS_PSRAM`, `-DARDUINO_USB_CDC_ON_BOOT=1`,
     `lib_deps` (ESPAsyncWebServer, AsyncTCP, ArduinoJson).
2. Nowy plik `partitions/otaapp_4mb.csv` dla 4 MB flash z OTA:
   - `nvs` (20K), `otadata` (8K), `app0` (~1.7M), `app1` (~1.7M), `littlefs` (reszta, ~0.4–0.5M).
   - Suma ≤ 4 MB. app0/app1 muszą pomieścić firmware (~1.3–1.7 MB).
3. `.github/workflows/build-firmware.yml` (CI PlatformIO):
   - trigger: `push` na `main` (paths: `src/**`, `data/**`, `platformio.ini`, `partitions/**`, sam workflow)
     + `workflow_dispatch`; `permissions: contents: write`.
   - kroki: checkout → setup Python 3 → `pip install platformio` →
     wersja `VERSION="${{ github.run_number }}"`, zapis `src/fw_version.h` = `#define FW_VERSION "<VERSION>"`
     oraz `firmware/version.txt` = `<VERSION>` → `pio run -e esp32s3` →
     `cp .pio/build/esp32s3/firmware.bin firmware/firmware.bin` →
     commit `firmware/firmware.bin` + `firmware/version.txt` z `[skip ci]` i `git push`.
4. `src/config.h`: dodaj na górze fallback wersji:
   ```c
   #if __has_include("fw_version.h")
     #include "fw_version.h"
   #else
     #define FW_VERSION "0.1-dev"
   #endif
   ```
5. `.gitignore`: dopisz `src/fw_version.h` (generowany w CI, nie do repo).

## Czego NIE robić w tym briefie
- Nie ruszać logiki web/dashboardu (panel i tak idzie do przebudowy — osobny etap).
- Nie dodawać jeszcze klienta OTA na urządzeniu (to Brief 002).
- Nie zmieniać struktury `src/` poza `config.h` + `fw_version.h`.

## Kryteria akceptacji
- `pio run -e esp32s3` kompiluje się bez błędów; rozmiar firmware mieści się w app0.
- Po pushu na `main`: workflow zielony, w repo pojawia się `firmware/firmware.bin` + `firmware/version.txt`
  z numerem builda; commit CI ma `[skip ci]` (brak pętli).
- Na płytce (`pio run -e esp32s3 -t upload -t monitor`): boot bez `Guru Meditation`, AP `ExperienceOS`
  wstaje, log pokazuje PSRAM 2 MB (QSPI).

## Weryfikacja (dla Groka przed PR)
- `pio run -e esp32s3` lokalnie zielone; wklej do PR raport rozmiaru (Flash/RAM).
- Jeśli masz płytkę pod ręką: upload + monitor, potwierdź brak crasha i PSRAM 2 MB.

## Workflow
Pracuj na gałęzi `feat/001-board-ci`. Commit + `gh pr create` do `main` z linkiem do tego briefu
w opisie. Nie merguj sam — merge robi Claude po przeglądzie.
