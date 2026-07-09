# ExperienceOS

Mini „system operacyjny” w przeglądarce dla **ESP32‑S3** — pokaz możliwości mikrokontrolera dostępny po WiFi. Wgrywasz jeden szkic, łączysz się z płytką i dostajesz dashboard z monitorem systemu, DSP, grą, benchmarkami i aktualizacją bezprzewodową (OTA z pliku oraz z GitHub).

Bez zewnętrznych bibliotek, bez LittleFS, bez PlatformIO — wszystko oparte na wbudowanym rdzeniu Arduino‑ESP32.

## Funkcje

- **System Monitor** (10×/s): heap, PSRAM, CPU, temperatura SoC, zadania FreeRTOS
- **Telemetria** — przewijany wykres z wyborem zakresu czasu (1 / 5 / 10 / 30 min, 1 h) i skalą osi
- **CPU Control** — przełączanie taktowania rdzeni 80 / 160 / 240 MHz na żywo
- **CPU Benchmark** — MIPS (int), MFLOPS (float), przepustowość memcpy
- **DSP Lab** — własny FFT 256‑pkt: widmo + waterfall + benchmark DSP
- **Oscyloskop ADC** — próbkowanie pinu ADC1 i podgląd przebiegu na żywo
- **Network Speed Test** — pomiar przepustowości WiFi (ESP ↔ przeglądarka) z pełnym logiem
- **Sterowanie diodą RGB** (WS2812) — kolor + efekty rainbow / breathe
- **Snake** — gra sterowana klawiaturą / dotykiem, rekord w NVS
- **Experience Score** — odblokowywane osiągnięcia (trzymane na urządzeniu)
- **WiFi z panelu** — dane sieci zapisywane w NVS (przeżywają OTA)
- **OTA** — aktualizacja firmware z pliku `.bin` oraz **prosto z GitHub**

## Sprzęt

Dowolny **ESP32‑S3** (kod sam wykrywa PSRAM i wbudowaną diodę RGB). Testowane na wariancie S3‑R2 (4 MB Flash + 2 MB PSRAM, USB‑Serial/JTAG) pod Arduino‑ESP32 core 3.x (ESP‑IDF 5.x).

## Struktura repo

```
.
├── ExperienceOS/
│   ├── ExperienceOS.ino       # logika (WiFi, serwer, API, DSP, OTA)
│   ├── webpage.h              # interfejs webowy (HTML/CSS/JS)
│   └── secrets.h.example      # szablon danych WiFi (skopiuj do secrets.h)
├── firmware/                  # publikowane przez GitHub Actions
│   ├── firmware.bin
│   └── version.txt
├── .github/workflows/build-firmware.yml
├── INSTRUKCJA.md              # pełna instrukcja krok po kroku
├── .gitignore
├── LICENSE.md
└── README.md
```

## Szybki start

1. W Arduino IDE otwórz `ExperienceOS/ExperienceOS.ino` (plik `webpage.h` pojawi się jako druga zakładka).
2. Wybierz płytkę **ESP32S3 Dev Module**, ustaw **Partition: Minimal SPIFFS (1.9MB APP with OTA)**, **PSRAM: QSPI PSRAM**, **USB CDC On Boot: Enabled**.
3. Wgraj (przy problemach ze wgrywaniem: przytrzymaj BOOT, tapnij RESET, puść BOOT; Upload Speed 115200).
4. Połącz się z siecią WiFi `ExperienceOS-XXXX` → `http://192.168.4.1`, albo po adresie IP z Monitora Portu, jeśli podłączysz się do sieci domowej.

Pełne kroki: **[INSTRUKCJA.md](INSTRUKCJA.md)**.

## OTA z GitHub

Cykl: `git push` → GitHub Actions kompiluje `firmware.bin` i podbija `version.txt` → na urządzeniu w panelu **GITHUB UPDATE** klikasz „Sprawdź wersję” i „Aktualizuj z GitHub”. Bez kabla.

> **Repozytorium musi być publiczne** — ESP pobiera pliki po `raw.githubusercontent.com`. Włącz też `Settings → Actions → General → Workflow permissions → Read and write`. Szczegóły w [INSTRUKCJA.md](INSTRUKCJA.md).

## Licencja

MIT — patrz [LICENSE.md](LICENSE.md).
