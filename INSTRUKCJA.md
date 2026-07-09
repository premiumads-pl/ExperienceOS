# ExperienceOS — instrukcja

Projekt ma dwa pliki szkicu w folderze `ExperienceOS/`:

- `ExperienceOS.ino` — logika (WiFi, serwer, API, DSP, OTA)
- `webpage.h` — cały interfejs (HTML/CSS/JS)

Muszą leżeć razem, w tym samym folderze. **Nie wklejaj kodu do nowego, pustego szkicu** — interfejs musi zostać w `webpage.h`, inaczej preprocesor Arduino IDE psuje wielki tekst strony.

---

## 1. Jednorazowa konfiguracja Arduino IDE

1. **Dodaj obsługę ESP32:** `File → Preferences → Additional boards manager URLs` wklej:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
2. `Tools → Board → Boards Manager…` → wyszukaj **esp32** (Espressif) → **Install**.

---

## 2. Wgranie przez USB (pierwszy raz zawsze kablem)

1. `File → Open…` i wskaż **`ExperienceOS/ExperienceOS.ino`**. `webpage.h` pojawi się jako druga zakładka.
2. Podłącz płytkę kablem **USB-C** (danych, nie tylko ładowania).
3. Ustaw w menu `Tools`:
   - **Board:** `ESP32S3 Dev Module` (albo dokładny model Twojej płytki)
   - **USB CDC On Boot:** **Enabled** ← inaczej Monitor Portu na S3 milczy
   - **Partition Scheme:** **`Minimal SPIFFS (1.9MB APP with OTA)`** ← duży szkic (89% na `default`) + działa OTA. Musi być **taki sam** jak w GitHub Actions (`min_spiffs`). Nie wybieraj „Huge APP (No OTA)”.
   - **PSRAM:** **`QSPI PSRAM`** (płytka ma 2 MB PSRAM, wariant S3‑R2). Nie wybieraj „OPI PSRAM” — to dla innych płytek i wywoła crash. Ewentualnie „Disabled” (stabilnie, ale bez PSRAM).
   - **Flash Size:** `4MB` (zgodnie z płytką)
   - **Upload Speed:** przy problemach z wgrywaniem zmniejsz do **`115200`**
   - **Port:** wybierz port, który pojawił się po podłączeniu
4. Kliknij **Upload** (strzałka →).
   - Jeśli nie wchodzi w tryb wgrywania: przytrzymaj **BOOT**, krótko naciśnij **RESET**, puść **BOOT** i wgraj ponownie.
5. Otwórz **Serial Monitor** (115200) — wypisze nazwę sieci i adresy.
6. Połącz telefon/laptop z siecią WiFi **`ExperienceOS-XXXX`** (otwarta) i wejdź na **http://192.168.4.1**.

---

## 3. Internet, `experienceos.local` i OTA z GitHub

W `ExperienceOS.ino`, na górze, ustaw swoje domowe WiFi:

```cpp
#define STA_SSID        "TwojaSiec"
#define STA_PASSWORD    "TwojeHaslo"
```

Wtedy ESP dołącza też do Twojej sieci (drugi adres w Monitorze + `http://experienceos.local`) i ma dostęp do internetu potrzebny do aktualizacji z GitHub.

Do OTA z GitHub ustaw jeszcze:

```cpp
#define GH_OWNER        "twoj-login"    // np. maciuso
#define GH_REPO         "esp32"         // nazwa repo (publiczne)
#define GH_BRANCH       "main"
```

---

## 4. Automatyczne buildy — GitHub Actions

### Układ repozytorium

```
esp32/                              <- katalog główny repo
├── .github/workflows/build-firmware.yml
├── ExperienceOS/
│   ├── ExperienceOS.ino
│   └── webpage.h
├── firmware/
│   ├── firmware.bin                <- generuje CI
│   └── version.txt                 <- generuje CI
└── INSTRUKCJA.md
```

### Włączenie (jednorazowo)

1. **Repozytorium musi być publiczne** — `Settings → General → Danger Zone → Change repository visibility → Public`. ESP pobiera pliki po publicznym `raw.githubusercontent.com`; dla prywatnego repo dostaje 404. Kod nie zawiera haseł (WiFi jest w NVS / `secrets.h`), więc to bezpieczne.
2. `Settings → Actions → General → Workflow permissions` → zaznacz **Read and write permissions** (żeby CI mógł zapisać zbudowany `.bin` i `version.txt` z powrotem do repo).

### Jak to działa

1. Robisz zmiany w `ExperienceOS/` i `git push` na `main`.
2. GitHub Actions:
   - instaluje `arduino-cli` + rdzeń ESP32,
   - nadaje wersję = numer buildu (`github.run_number`) i wpisuje ją do firmware oraz do `firmware/version.txt`,
   - kompiluje szkic,
   - zapisuje `firmware/firmware.bin` + `firmware/version.txt` do repo (commit z `[skip ci]`, więc nie zapętla się).
3. Na urządzeniu (panel **GITHUB UPDATE**): klik **Sprawdź wersję** → jeśli w repo jest nowszy numer, klik **Aktualizuj z GitHub** → ESP pobiera `.bin`, weryfikuje, flashuje i restartuje.

> Ważne: `PartitionScheme` w workflow (`build-firmware.yml`) musi być taki sam jak wybrany w Arduino IDE, żeby obraz OTA pasował do partycji na płytce. Domyślnie oba to `default`.

### Wersja lokalna vs z CI

- Zbudowane w Arduino IDE: firmware raportuje `0.3-dev`.
- Zbudowane w CI: firmware raportuje numer buildu (np. `7`), bo Actions generuje `ExperienceOS/fw_version.h`. Ten plik jest tworzony tylko w CI — nie trzymaj go w repo.

---

## 5. Alternatywa bez CI (ręcznie)

1. W Arduino IDE: `Sketch → Export Compiled Binary` — powstanie `.bin` w podfolderze `build/…`.
2. Skopiuj go do repo jako `firmware/firmware.bin`.
3. Wpisz nowy numer do `firmware/version.txt` (np. `4`) i podnieś `FW_VERSION` w szkicu na ten sam numer.
4. `git push`. Na urządzeniu: **Sprawdź wersję → Aktualizuj z GitHub**.

---

## Sekret 🕹️

Osiągnięcie „Secret Found” odblokujesz kodem Konami: **↑ ↑ ↓ ↓ ← → ← → B A** na klawiaturze, albo **7× tapnij w logo** na telefonie.
