# Brief 002 — WiFi STA + klient OTA z GitHub

## Cel
Urządzenie łączy się z internetem (STA) i potrafi SPRAWDZIĆ oraz POBRAĆ nowy firmware z GitHub
(jak w `dashboard-v4`), sterowane przez API. Bez blokowania serwera Async.

## Kontekst (WAŻNE)
- Serwer to **ESPAsyncWebServer — handlery NIE MOGĄ blokować** (żadnych długich fetchy ani `httpUpdate`
  w handlerze). Blokujące operacje sieciowe rób w `loop()` — serwer Async działa na osobnym tasku,
  więc `loop()` może spokojnie blokować bez zawieszania UI.
- OTA jak w `dashboard-v4`: `httpUpdate` + `WiFiClientSecure(setInsecure)` + `setFollowRedirects`,
  URL z `?nc=<millis>` (cache CDN GitHuba ~5 min), `version.txt` = numer builda.
- Repo: `premiumads-pl/ExperienceOS`, gałąź `main`. Pliki: `firmware/firmware.bin`, `firmware/version.txt`.
- `httpUpdate` i `WiFiClientSecure` są wbudowane we framework — bez zmian w `lib_deps`.

## Zakres (co zrobić)
1. `src/config.h` — dodaj:
   - STA (z opcjonalnego `secrets.h`):
     ```c
     #if __has_include("secrets.h")
       #include "secrets.h"
     #endif
     #ifndef STA_SSID
       #define STA_SSID     ""
       #define STA_PASSWORD ""
     #endif
     ```
   - GitHub: `GH_OWNER "premiumads-pl"`, `GH_REPO "ExperienceOS"`, `GH_BRANCH "main"`,
     `GH_FW_PATH "firmware/firmware.bin"`, `GH_VER_PATH "firmware/version.txt"`.
2. Nowy moduł `src/ota.h` + `src/ota.cpp`:
   - stan `enum OtaState { IDLE, CHECKING, UPDATING }` + pola: `currentVer` (=FW_VERSION),
     `remoteVer`, `bool updateAvail`, `int httpCode`, `String msg`.
   - `otaRequestCheck()` / `otaRequestUpdate()` — tylko ustawiają flagę (wołane z handlerów async).
   - `otaLoop()` — wołane z `loop()`; realizuje żądania BLOKUJĄCO:
     * check: HTTPS GET `ghBase()+GH_VER_PATH+"?nc="+millis()`; zapisz `remoteVer`, `httpCode`,
       `updateAvail = (remoteVer.length() && remoteVer != String(FW_VERSION))`.
     * update: `httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS)`, `rebootOnUpdate(true)`,
       `httpUpdate.update(client, ghBase()+GH_FW_PATH+"?nc="+millis())`; przy błędzie
       `msg = "BLAD " + getLastError() + ": " + getLastErrorString()`.
   - helper `String ghBase()` = `"https://raw.githubusercontent.com/" GH_OWNER "/" GH_REPO "/" GH_BRANCH "/"`.
3. `src/main.cpp`:
   - `WiFi.mode(WIFI_AP_STA)`; po `softAP(...)`: jeśli `strlen(STA_SSID) > 0` →
     `WiFi.begin(STA_SSID, STA_PASSWORD)` (NIEBLOKUJĄCO, bez pętli czekania w setup).
   - w `loop()`: dołóż `otaLoop();` obok istniejącej diagnostyki. Wykryj połączenie STA:
     gdy `WiFi.status()==WL_CONNECTED` po raz pierwszy → zaloguj IP na Serial i ustaw wewnętrzny `staUp=true`.
4. `src/api.cpp` (async — NIE blokować):
   - `GET  /api/ota/status` → JSON `{state, current, remote, updateAvail, online:<staUp>, configured, httpCode, msg}`.
   - `POST /api/ota/check`  → `otaRequestCheck();` → `{ok:true}`.
   - `POST /api/ota/update` → `otaRequestUpdate();` → `{ok:true}`.
5. Nie dodawać paneli UI (przebudowa web to osobny brief) — same endpointy wystarczą do testu.

## Czego NIE robić
- Nie blokować w handlerach async (żaden fetch/httpUpdate w handlerze!).
- Nie ruszać partycji/CI (są z Briefu 001).
- Nie budować jeszcze UI konsoli.

## Kryteria akceptacji
- `pio run -e esp32s3` zielone.
- Z `secrets.h` (STA): po boot łączy się z siecią; `GET /api/ota/status` → `online:true`,
  `current="<numer builda>"`; po `POST /api/ota/check` → `remote` = numer z repo i `updateAvail` poprawnie.
- `POST /api/ota/update` przy dostępnej nowej wersji → pobiera firmware i restartuje (log `[GH]` na Serial).
- Bez `secrets.h`: `online:false`, brak crasha, AP dalej działa.

## Weryfikacja (dla Groka przed PR)
- `pio run` zielone; jeśli masz płytkę: upload z `secrets.h`, test endpointów `/api/ota/*`
  (curl albo przeglądarka), wklej do PR wynik `GET /api/ota/status`.

## Workflow
Gałąź `feat/002-ota-client`, commit, `gh pr create` do `main` z linkiem do tego briefu. Merge robi Claude.
