# Brief 001b — Fix: LittleFS nie montuje (etykieta partycji)

## Problem (z płytki)
`E esp_littlefs: partition "spiffs" could not be found` → `[FS] LittleFS mount failed!`.
`LittleFS.begin()` domyślnie szuka partycji o etykiecie **`spiffs`**, a w
`partitions/otaapp_4mb.csv` partycja filesystemu nazywa się `littlefs`.

## Fix
W `partitions/otaapp_4mb.csv` zmień NAZWĘ partycji filesystemu z `littlefs` na `spiffs`
(typ `data`, subtype `spiffs`, offset i rozmiar BEZ zmian). Nic więcej nie ruszać.

## Kryteria akceptacji
- `pio run -e esp32s3` zielone.
- Po `-t upload -t uploadfs` i reboocie: brak `LittleFS mount failed`, w logu `[FS] LittleFS mounted`.
- `http://<ip>/` serwuje `index.html` (nie „Not Found").

## Workflow
Gałąź `fix/001b-littlefs-label`, PR do `main` z linkiem do briefu. Merge robi Claude.
