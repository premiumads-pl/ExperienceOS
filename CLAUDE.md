# ExperienceOS — CLAUDE.md (projekt + workflow)

## Role
- **Claude = architekt / reviewer.** Pisze briefy zmian, robi przegląd kodu (diff / PR),
  scala i `git push`. NIE pisze kodu aplikacji.
- **Grok = developer.** Implementuje briefy w tym repo, pracuje na gałęzi/worktree, otwiera PR.
- **Maciej** uruchamia Grok w terminalu, podając nazwę briefu; jest właścicielem decyzji.

## Workflow (oparty o PR)
1. Claude pisze brief → `briefs/NNN-nazwa.md` (cel, zakres, pliki, kryteria akceptacji, testy).
2. Maciej: `grok "wykonaj brief briefs/NNN-nazwa.md"` — Grok pracuje na gałęzi `feat/NNN-nazwa`.
3. Grok commituje i otwiera PR (`gh pr create`), w opisie linkuje brief.
4. Claude robi przegląd diffa PR → uwagi albo akceptacja.
5. Po akceptacji: merge do `main` → CI (PlatformIO) buduje firmware → OTA z GitHub.

Zasady: `main` zawsze się kompiluje; każda zmiana = brief + PR; jeden brief = jedna gałąź.

## Cel projektu (po pivocie)
Retro-konsola na ESP32-S3: uruchamianie **natywnych portów gier** (start: Another World / rawgl),
ekran przez przeglądarkę → docelowo ST7789 2.8", klawiatura BLE, aktualizacja OTA z GitHub (jak dotąd).

## WERYFIKACJA WYKONALNOŚCI — co blokuje
- ❌ **Pełna emulacja Amigi 500 (Kickstart 1.3 + Workbench + dowolne gry): NIEWYKONALNE na ESP32-S3.**
  Amiga = 68000 + custom chipset (Agnus/Denise/Paula: blitter, copper, sprity, DMA) — zbyt złożone.
  UAE wymaga min. ~10 MB RAM; nawet PiStorm (Raspberry Pi, mocniejszy) NIE emuluje 68000, tylko używa
  realnego chipsetu Amigi. Sufit emulacji ESP32 to ~IBM PC 8086 (FabGL). Kickstart ROM tu nie pomoże.
- ✅ **Another World natywnie: WYKONALNE.** Silnik `rawgl` (open-source reimplementacja, NIE emulator)
  ma już działający port na ESP32 (`ESP32TinyAnotherWorld`). To realna droga do „uruchom Another World".
- Wniosek: budujemy **natywną konsolę/launcher gier**, nie emulator Amigi. Kolejne gry = kolejne
  natywne porty (jeśli mają otwarty silnik), a nie obrazy dyskietek Amigi.

## Ograniczenia sprzętowe (WAŻNE)
- Płytka: **ESP32-S3-R2 — 4 MB flash + 2 MB QSPI PSRAM** (NIE 16 MB, NIE OPI).
- `platformio.ini` ma teraz BŁĘDNIE: `flash_size=16MB`, `default_16MB.csv`, `memory_type=qio_opi`.
  DO POPRAWY (pierwszy brief): flash 4 MB, partycja 4 MB z OTA, PSRAM QSPI (`qio_qspi` lub `dio_qspi`).
  W obecnej postaci grozi to crashem (OPI na płytce QSPI) i brakiem miejsca (16 MB na 4 MB flash).
- 4 MB flash = ciasno: OTA potrzebuje 2 slotów app + LittleFS na dane gry. Another World (~250 KB)
  się zmieści, ale budżet flash trzeba pilnować przy każdym briefie.
- **Bluetooth: ESP32-S3 ma TYLKO BLE (brak BT Classic).** Klawiatura musi być **BLE HID**, nie klasyczna.
- Ekran: 320×200 (Another World) → przeglądarka (stream WebSocket/MJPEG) teraz; ST7789 320×240 (SPI) docelowo.

## OTA z GitHub (zachowane)
Mechanizm jak wcześniej: CI buduje `.bin`, urządzenie pobiera z `raw.githubusercontent`
(`?nc=` cache-buster), `version.txt` = numer builda. DO ADAPTACJI pod PlatformIO (CI: `pio run`).

## Baseline / wersje
- Poprzedni dashboard (Arduino, repo `premiumads-pl/ExperienceOS`) — ostatnia wersja **firmware v4**,
  zachowana jako tag `dashboard-v4` (checkpoint przed pivotem).
- Nowy projekt (PlatformIO, `/Users/maciuso/ExperienceOS`) — start jako „Hardware Explorer v0.1"; panel
  do przebudowy pod konsolę gier.

## Struktura repo
`platformio.ini`, `src/` (main, web, api, diagnostics, config, gpio_map), `data/` (LittleFS: index.html,
app.js, style.css), `briefs/` (briefy Claude), `.github/workflows/` (CI PlatformIO + OTA).

## Toolchain (WAŻNE)
PlatformIO jest w LOKALNYM venv projektu — NIE globalnie. Używaj:
`source .venv/bin/activate && pio ...`  albo bezpośrednio `.venv/bin/pio ...`.
Wersja: PlatformIO Core 6.1.19. Typowe: `.venv/bin/pio run -e esp32s3` (build),
`-t upload` (wgranie firmware), `-t uploadfs` (wgranie LittleFS z data/), `-t monitor` (Serial).
