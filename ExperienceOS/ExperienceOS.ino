/*  ============================================================================
    ExperienceOS  v0.3   —   ESP32-S3 web experience
    ----------------------------------------------------------------------------
    PROJEKT MA 2 PLIKI (obydwa w tym samym folderze):
      • ExperienceOS.ino  — logika (WiFi, serwer, API, DSP, OTA)
      • webpage.h         — cały interfejs (HTML/CSS/JS)
    W Arduino IDE OTWÓRZ ExperienceOS.ino  (File -> Open). webpage.h pojawi sie
    automatycznie jako druga zakladka. NIE wklejaj kodu do nowego, pustego szkicu
    — wielki interfejs musi zostac w webpage.h, inaczej preprocesor Arduino go psuje.

    Bez zewnetrznych bibliotek, bez LittleFS, bez PlatformIO. Wszystko wbudowane.

    Po wgraniu:
      1. Monitor Portu Szeregowego (115200) wypisze adresy.
      2. Polacz sie z siecia WiFi "ExperienceOS-XXXX" (otwarta) -> http://192.168.4.1
      3. Dla internetu / GitHub OTA / experienceos.local ustaw STA_SSID i STA_PASSWORD.

    W przegladarce: monitor systemu 10x/s, wykres z zakresem czasu (1..60 min),
    benchmark CPU, przelacznik taktowania, DSP Lab (FFT + waterfall), oscyloskop ADC,
    test lacza WiFi, gra Snake (rekord w NVS), osiagniecia, sterowanie RGB,
    OTA z pliku ORAZ OTA z GitHub.

    WAZNE dla OTA (lokalnego i z GitHub): w Tools -> Partition Scheme wybierz schemat
    Z OTA (np. "Default 4MB with spiffs" albo "... with OTA"), NIE "Huge APP (No OTA)".

    Plytka: dowolny ESP32-S3. Kod sam wykrywa PSRAM i diode RGB.
    Testowane pod Arduino-ESP32 core 3.x (ESP-IDF 5.x).
    ============================================================================ */

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ------------------------- USTAWIENIA UŻYTKOWNIKA -------------------------- */
#define AP_SSID_PREFIX  "ExperienceOS"   // nazwa własnej sieci; dodany zostanie -XXXX
#define AP_PASSWORD     ""               // "" = sieć otwarta (najwygodniej). >=8 znaków dla hasła.

// ===== WiFi do sieci domowej (STA) =====================================
// NAJWYGODNIEJ: wpisz swoja siec ponizej. Puste = urzadzenie dziala tylko jako AP.
// Mozesz tez ustawic siec z panelu "SIEC WiFi" w przegladarce (zapis w NVS, MA PRIORYTET),
// albo w pliku secrets.h (jest w .gitignore). Uwaga: przy commicie do PUBLICZNEGO repo
// nie zostawiaj tu prawdziwego hasla — uzyj wtedy panelu albo secrets.h.
#if __has_include("secrets.h")
  #include "secrets.h"
#endif
#ifndef STA_SSID
  #define STA_SSID      ""                     // puste: ustaw siec w panelu "SIEC WiFi" albo w secrets.h
  #define STA_PASSWORD  ""
#endif

// Jeśli Twoja płytka ma diodę RGB, ale panel jej nie pokazuje, wpisz tu jej pin
// (typowo 48 albo 21). -1 = wyłącz. Gdy core samo definiuje RGB_BUILTIN, ma pierwszeństwo.
#define RGB_LED_PIN     -1

// --- Aktualizacja z GitHub (OTA "on-air") -------------------------------------
// ESP pobiera SKOMPILOWANY firmware (.bin) z repo i sam sie flashuje.
// Wymaga polaczenia z internetem, czyli ustawionego STA_SSID/STA_PASSWORD powyzej.
// Wersja firmware. Lokalnie (Arduino IDE): "0.3-dev". W CI plik fw_version.h jest
// generowany przez GitHub Actions i nadpisuje te wartosc numerem builda.
#if __has_include("fw_version.h")
  #include "fw_version.h"
#else
  #define FW_VERSION    "0.3-dev"
#endif
#define GH_OWNER        "premiumads-pl"                       // login/organizacja GitHub, np. "maciuso"
#define GH_REPO         "ExperienceOS"                       // nazwa repo, np. "esp32"
#define GH_BRANCH       "main"                   // galaz
#define GH_FW_PATH      "firmware/firmware.bin"  // skompilowany .bin (publikowany przez CI)
#define GH_VER_PATH     "firmware/version.txt"   // numer wersji (publikowany przez CI)
/* -------------------------------------------------------------------------- */

// --- ustalenie pinu diody RGB ---
#if defined(RGB_BUILTIN)
  #define EOS_RGB_PIN RGB_BUILTIN
#elif (RGB_LED_PIN >= 0)
  #define EOS_RGB_PIN RGB_LED_PIN
#endif

WebServer  server(80);
DNSServer  dns;
Preferences prefs;
const byte DNS_PORT = 53;

// --- statyczna tożsamość płytki (odczytana raz w setup) ---
String   gApSsid, gApIp, gStaIp, gMac;
bool     gStaUp = false;

// --- moduly rozszerzone (DSP / OTA) ---
#define  FFT_N 256
static float fftRe[FFT_N], fftIm[FFT_N];
bool     gOtaReboot = false;
uint32_t gOtaRebootAt = 0;
uint32_t gLoopCount = 0, gLoopRate = 0, gLoopT = 0;   // licznik petli/s

// --- pamięć trwała (NVS) ---
uint32_t gHigh   = 0;     // rekord w Snake
uint32_t gAchMask = 0;    // maska bitowa osiągnięć

// Kolejność MUSI być zgodna z obiektem ACH w JavaScript poniżej.
const char* ACH_KEYS[] = {
  "boot", "explorer", "bench", "led", "rainbow", "snake10", "snake50", "marathon", "konami", "speedster",
  "overclock", "dsp", "scope", "ota", "github"
};
const int ACH_COUNT = sizeof(ACH_KEYS) / sizeof(ACH_KEYS[0]);

int achIndex(const String& key) {
  for (int i = 0; i < ACH_COUNT; i++) if (key == ACH_KEYS[i]) return i;
  return -1;
}
void setAch(int i) {                                   // zapis do NVS tylko przy pierwszym odblokowaniu
  if (i < 0 || i >= ACH_COUNT) return;
  uint32_t bit = 1u << i;
  if (gAchMask & bit) return;
  gAchMask |= bit;
  prefs.putUInt("ach", gAchMask);
}

// --- stan diody RGB ---
#ifdef EOS_RGB_PIN
uint8_t  baseR = 0, baseG = 200, baseB = 255;
String   ledEffect = "off";        // off | solid | rainbow | breathe
bool     ledDirty  = true;
const uint8_t LED_BRIGHTNESS = 70; // ograniczenie jasności, by nie oślepiać

void hsv2rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t* r, uint8_t* g, uint8_t* b) {
  float S = s / 255.0f, V = v / 255.0f;
  float C = V * S;
  float X = C * (1 - fabs(fmod(h / 60.0f, 2) - 1));
  float m = V - C, rr = 0, gg = 0, bb = 0;
  if      (h < 60)  { rr = C; gg = X; }
  else if (h < 120) { rr = X; gg = C; }
  else if (h < 180) { gg = C; bb = X; }
  else if (h < 240) { gg = X; bb = C; }
  else if (h < 300) { rr = X; bb = C; }
  else              { rr = C; bb = X; }
  *r = (uint8_t)((rr + m) * 255);
  *g = (uint8_t)((gg + m) * 255);
  *b = (uint8_t)((bb + m) * 255);
}
inline void ledShow(uint8_t r, uint8_t g, uint8_t b) {
  rgbLedWrite(EOS_RGB_PIN,
              (uint16_t)r * LED_BRIGHTNESS / 255,
              (uint16_t)g * LED_BRIGHTNESS / 255,
              (uint16_t)b * LED_BRIGHTNESS / 255);
}
void updateLed() {
  static uint32_t last = 0, hue = 0;
  uint32_t now = millis();
  if (now - last < 16) return;      // ~60 FPS
  last = now;
  if (ledEffect == "rainbow") {
    hue = (hue + 3) % 360;
    uint8_t r, g, b; hsv2rgb(hue, 255, 255, &r, &g, &b); ledShow(r, g, b);
  } else if (ledEffect == "breathe") {
    float p = (sinf(now / 700.0f) * 0.5f + 0.5f);
    ledShow((uint8_t)(baseR * p), (uint8_t)(baseG * p), (uint8_t)(baseB * p));
  } else if (ledDirty) {
    if (ledEffect == "solid") ledShow(baseR, baseG, baseB);
    else                      ledShow(0, 0, 0);
    ledDirty = false;
  }
}
#endif

/* ============================  STRONA (PROGMEM)  =========================== */
// Interfejs webowy jest w osobnym pliku (zakladka webpage.h w Arduino IDE)
#include "webpage.h"

/* ============================  API / HANDLERY  ============================= */

String infoJson() {
  String s = "{";
  s += "\"model\":\"" + String(ESP.getChipModel()) + "\",";
  s += "\"rev\":" + String(ESP.getChipRevision()) + ",";
  s += "\"cores\":" + String(ESP.getChipCores()) + ",";
  s += "\"cpuMax\":" + String(getCpuFrequencyMhz()) + ",";
  s += "\"flash\":" + String((uint32_t)ESP.getFlashChipSize()) + ",";
  s += "\"psram\":" + String((uint32_t)ESP.getPsramSize()) + ",";
  s += "\"mac\":\"" + gMac + "\",";
  s += "\"apSsid\":\"" + gApSsid + "\",";
  s += "\"apIp\":\"" + gApIp + "\",";
  s += "\"staIp\":\"" + gStaIp + "\",";
  s += "\"staUp\":" + String(gStaUp ? "true" : "false") + ",";
  s += "\"fw\":\"" + String(FW_VERSION) + "\",";
  s += "\"sdk\":\"" + String(ESP.getSdkVersion()) + "\",";
#ifdef EOS_RGB_PIN
  s += "\"rgb\":true";
#else
  s += "\"rgb\":false";
#endif
  s += "}";
  return s;
}

String statsJson() {
  float t = temperatureRead();
  String s = "{";
  s += "\"up\":" + String((uint32_t)millis()) + ",";
  s += "\"heapFree\":" + String((uint32_t)ESP.getFreeHeap()) + ",";
  s += "\"heapSize\":" + String((uint32_t)ESP.getHeapSize()) + ",";
  s += "\"heapMin\":" + String((uint32_t)ESP.getMinFreeHeap()) + ",";
  s += "\"psramFree\":" + String((uint32_t)ESP.getFreePsram()) + ",";
  s += "\"psramSize\":" + String((uint32_t)ESP.getPsramSize()) + ",";
  s += "\"cpuMhz\":" + String(getCpuFrequencyMhz()) + ",";
  s += "\"temp\":" + String(t, 1) + ",";
  s += "\"tasks\":" + String((uint32_t)uxTaskGetNumberOfTasks()) + ",";
  s += "\"rssi\":" + String(gStaUp ? WiFi.RSSI() : 0) + ",";
  s += "\"apClients\":" + String(WiFi.softAPgetStationNum()) + ",";
  s += "\"maxBlock\":" + String((uint32_t)ESP.getMaxAllocHeap()) + ",";
  s += "\"sketchUsed\":" + String((uint32_t)ESP.getSketchSize()) + ",";
  s += "\"sketchTotal\":" + String((uint32_t)(ESP.getSketchSize() + ESP.getFreeSketchSpace())) + ",";
  s += "\"loopRate\":" + String(gLoopRate) + ",";
  s += "\"chan\":" + String(WiFi.channel()) + ",";
  s += "\"staUp\":" + String(gStaUp ? "true" : "false");
  s += "}";
  return s;
}

// Benchmark: liczby całkowite, zmiennoprzecinkowe i przepustowość pamięci.
String benchJson() {
  volatile uint32_t sinkI = 0;
  volatile float    sinkF = 0;
  uint32_t t0 = micros();

  const uint32_t N_INT = 20000000UL;
  uint32_t a = 1, b = 3;
  for (uint32_t i = 0; i < N_INT; i++) { a = a * 1664525u + 1013904223u; b ^= a; }
  sinkI = a ^ b;
  uint32_t tInt = micros() - t0;

  t0 = micros();
  const uint32_t N_FLT = 10000000UL;
  float x = 1.000001f, y = 0.999999f, acc = 0;
  for (uint32_t i = 0; i < N_FLT; i++) { acc += x * y; x += 1e-7f; y -= 1e-7f; }
  sinkF = acc;
  uint32_t tFlt = micros() - t0;

  // memcpy — 64 MB w kawałkach po 32 KB
  const size_t BUF = 32 * 1024;
  uint8_t* src = (uint8_t*)malloc(BUF);
  uint8_t* dst = (uint8_t*)malloc(BUF);
  float memMBs = 0;
  if (src && dst) {
    memset(src, 0xA5, BUF);
    const int REPS = 2048;                 // 2048 * 32KB = 64 MB
    t0 = micros();
    for (int i = 0; i < REPS; i++) memcpy(dst, src, BUF);
    uint32_t tMem = micros() - t0;
    if (tMem == 0) tMem = 1;
    double mb = (double)REPS * BUF / 1048576.0;
    memMBs = (float)(mb / (tMem / 1000000.0));
    sinkI ^= dst[0];
  }
  if (src) free(src);
  if (dst) free(dst);

  float psramMBs = 0;                              // przepustowosc PSRAM (jesli jest)
  if (psramFound()) {
    const size_t PB = 256 * 1024;
    uint8_t* ps  = (uint8_t*)ps_malloc(PB);
    uint8_t* ps2 = (uint8_t*)ps_malloc(PB);
    if (ps && ps2) {
      memset(ps, 0x5A, PB);
      const int R = 64;
      uint32_t tp = micros();
      for (int i = 0; i < R; i++) memcpy(ps2, ps, PB);
      uint32_t dtp = micros() - tp; if (dtp == 0) dtp = 1;
      psramMBs = (float)((double)R * PB / 1048576.0 / (dtp / 1000000.0));
    }
    if (ps)  free(ps);
    if (ps2) free(ps2);
  }

  if (tInt == 0) tInt = 1;
  if (tFlt == 0) tFlt = 1;
  float intMips    = (float)N_INT * 2.0f / tInt;      // 2 operacje/iterację, /us => M/s
  float floatMflops = (float)N_FLT * 2.0f / tFlt;

  (void)sinkI; (void)sinkF;
  String s = "{";
  s += "\"intMips\":" + String(intMips, 0) + ",";
  s += "\"floatMflops\":" + String(floatMflops, 1) + ",";
  s += "\"memMBs\":" + String(memMBs, 0) + ",";
  s += "\"psramMBs\":" + String(psramMBs, 0) + ",";
  s += "\"ms\":" + String((tInt + tFlt) / 1000);
  s += "}";
  return s;
}

void handleAch() {
  if (server.hasArg("set")) setAch(achIndex(server.arg("set")));
  String s = "{\"unlocked\":[";
  bool first = true;
  for (int i = 0; i < ACH_COUNT; i++) {
    if (gAchMask & (1u << i)) { if (!first) s += ","; s += "\""; s += ACH_KEYS[i]; s += "\""; first = false; }
  }
  s += "],\"high\":" + String(gHigh) + "}";
  server.send(200, "application/json", s);
}

void handleScore() {
  bool isNew = false;
  if (server.hasArg("s")) {
    uint32_t v = (uint32_t)server.arg("s").toInt();
    if (v > gHigh) { gHigh = v; prefs.putUInt("high", gHigh); isNew = true; }
  }
  server.send(200, "application/json",
              "{\"high\":" + String(gHigh) + ",\"isNew\":" + (isNew ? "true" : "false") + "}");
}

#ifdef EOS_RGB_PIN
void handleLed() {
  if (server.hasArg("fx")) ledEffect = server.arg("fx");
  if (server.hasArg("r"))  baseR = constrain(server.arg("r").toInt(), 0, 255);
  if (server.hasArg("g"))  baseG = constrain(server.arg("g").toInt(), 0, 255);
  if (server.hasArg("b"))  baseB = constrain(server.arg("b").toInt(), 0, 255);
  ledDirty = true;
  setAch(achIndex("led"));
  if (ledEffect == "rainbow") setAch(achIndex("rainbow"));
  server.send(200, "application/json", "{\"ok\":true}");
}
#endif

// Test przepustowosci WiFi (ESP32 <-> przegladarka). To NIE jest test internetu.
void handleDl() {
  uint32_t bytes = server.hasArg("bytes") ? (uint32_t)server.arg("bytes").toInt() : 524288UL;
  if (bytes < 1024)      bytes = 1024;
  if (bytes > 8388608UL) bytes = 8388608UL;                 // limit 8 MB
  Serial.printf("[DL] start %u B (heap=%u)\n", bytes, (uint32_t)ESP.getFreeHeap());
  String chunk; chunk.reserve(2048);
  for (int i = 0; i < 2048; i++) chunk += 'X';
  server.sendHeader("Cache-Control", "no-store");
  server.setContentLength(bytes);
  server.send(200, "application/octet-stream", "");
  uint32_t sent = 0;
  while (sent < bytes) {
    uint32_t n = (bytes - sent < 2048UL) ? (bytes - sent) : 2048UL;
    if (n == 2048UL) server.sendContent(chunk);
    else             server.sendContent(chunk.substring(0, n));
    sent += n;
  }
  Serial.printf("[DL] done, wyslano %u B\n", sent);
}

// Upload liczony przez mechanizm uploadu (jak OTA) — pewny na kazdej wersji core.
uint32_t gUlBytes = 0;
void handleUlReply() {
  server.send(200, "application/json", "{\"bytes\":" + String(gUlBytes) + "}");
}
void handleUlUpload() {
  HTTPUpload& up = server.upload();
  if (up.status == UPLOAD_FILE_START)      gUlBytes = 0;
  else if (up.status == UPLOAD_FILE_WRITE) gUlBytes += up.currentSize;
}

void handleIndex() { server.send_P(200, "text/html", INDEX_HTML); }

/* ---- CPU: zmiana taktowania rdzeni na zywo (min 80 MHz dla WiFi) ---- */
void handleCpu() {
  if (server.hasArg("mhz")) {
    int m = server.arg("mhz").toInt();
    if (m == 80 || m == 160 || m == 240) setCpuFrequencyMhz(m);
  }
  server.send(200, "application/json", "{\"mhz\":" + String(getCpuFrequencyMhz()) + "}");
}

/* ---- DSP: wlasny FFT radix-2 (bez zewnetrznych bibliotek) ---- */
void fftRadix2(float* re, float* im, int n) {
  for (int i = 1, j = 0; i < n; i++) {                 // permutacja bit-reversal
    int bit = n >> 1;
    for (; j & bit; bit >>= 1) j ^= bit;
    j ^= bit;
    if (i < j) { float t = re[i]; re[i] = re[j]; re[j] = t; t = im[i]; im[i] = im[j]; im[j] = t; }
  }
  for (int len = 2; len <= n; len <<= 1) {              // motylki
    float ang = -2.0f * PI / len;
    for (int i = 0; i < n; i += len) {
      for (int k = 0; k < len / 2; k++) {
        float c = cosf(ang * k), s = sinf(ang * k);
        int a = i + k, b = i + k + len / 2;
        float vr = re[b] * c - im[b] * s;
        float vi = re[b] * s + im[b] * c;
        re[b] = re[a] - vr; im[b] = im[a] - vi;
        re[a] += vr;        im[a] += vi;
      }
    }
  }
}

void genSignal(const String& sig) {
  const float FS = 8000.0f;
  float ph = millis() / 1000.0f;                        // ruchoma faza -> zywy waterfall
  for (int i = 0; i < FFT_N; i++) {
    float t = i / FS, v = 0;
    if (sig == "tone")        v = sinf(2 * PI * 440 * t);
    else if (sig == "square") v = (sinf(2 * PI * 300 * t) > 0) ? 1.0f : -1.0f;
    else if (sig == "noise")  v = random(-1000, 1000) / 1000.0f;
    else if (sig == "sweep")  { float f = 200 + 1600 * (0.5f + 0.5f * sinf(ph)); v = sinf(2 * PI * f * t); }
    else { v  = 0.6f * sinf(2 * PI * (330 + 20 * sinf(ph)) * t);   // "chord"
           v += 0.4f * sinf(2 * PI * 660 * t);
           v += 0.3f * sinf(2 * PI * 990 * t); }
    v += random(-120, 120) / 1000.0f;                   // szczypta szumu
    fftRe[i] = v; fftIm[i] = 0;
  }
}

void handleFft() {
  genSignal(server.hasArg("sig") ? server.arg("sig") : String("chord"));
  fftRadix2(fftRe, fftIm, FFT_N);
  String s = "{\"fs\":8000,\"bins\":[";
  for (int i = 0; i < FFT_N / 2; i++) {
    float mag = sqrtf(fftRe[i] * fftRe[i] + fftIm[i] * fftIm[i]) / (FFT_N / 2.0f);
    float norm = (20.0f * log10f(mag + 1e-4f) + 60.0f) / 60.0f;   // -60..0 dB -> 0..1
    if (norm < 0) norm = 0;
    if (norm > 1) norm = 1;
    if (i) s += ',';
    s += String(norm, 2);
  }
  s += "]}";
  server.send(200, "application/json", s);
}

void handleFftBench() {
  genSignal("chord");
  static float snapRe[FFT_N];
  memcpy(snapRe, fftRe, sizeof(snapRe));
  const int REPS = 400;
  uint32_t t0 = micros();
  for (int r = 0; r < REPS; r++) {
    memcpy(fftRe, snapRe, sizeof(snapRe));
    memset(fftIm, 0, sizeof(fftIm));
    fftRadix2(fftRe, fftIm, FFT_N);
  }
  uint32_t dt = micros() - t0; if (dt == 0) dt = 1;
  server.send(200, "application/json",
    "{\"fftPerSec\":" + String(REPS * 1000000.0f / dt, 0) + ",\"n\":" + String(FFT_N) + ",\"us\":" + String(dt / REPS) + "}");
}

/* ---- Oscyloskop: probkowanie pinu ADC1 (GPIO1..10, bezpieczne z WiFi) ---- */
void handleScope() {
  int pin = server.hasArg("pin") ? server.arg("pin").toInt() : 1;
  if (pin < 1 || pin > 10) pin = 1;
  const int N = 240;
  static uint16_t buf[240];
  uint32_t t0 = micros();
  for (int i = 0; i < N; i++) buf[i] = analogReadMilliVolts(pin);
  uint32_t dt = micros() - t0; if (dt == 0) dt = 1;
  String s = "{\"pin\":" + String(pin) + ",\"fs\":" + String(N * 1000000.0f / dt, 0) + ",\"mv\":[";
  for (int i = 0; i < N; i++) { if (i) s += ','; s += String(buf[i]); }
  s += "]}";
  server.send(200, "application/json", s);
}

/* ---- OTA: aktualizacja firmware przez przegladarke (wbudowane Update.h) ---- */
// (lokalny OTA z pliku usuniety — jedyna sciezka aktualizacji to OTA z GitHub)

/* ---- OTA z GitHub: pobiera skompilowany .bin z repo i flashuje ---- */
String ghBase() { return "https://raw.githubusercontent.com/" + String(GH_OWNER) + "/" + String(GH_REPO) + "/" + String(GH_BRANCH) + "/"; }
bool   ghConfigured() { return strlen(GH_OWNER) > 0 && strlen(GH_REPO) > 0; }

void handleGhCheck() {
  String url = ghBase() + GH_VER_PATH + "?nc=" + String(millis());   // ?nc: omija cache CDN GitHuba
  String remote = "";
  int code = 0;
  if (ghConfigured() && gStaUp) {
    WiFiClientSecure c; c.setInsecure();
    HTTPClient http;
    if (http.begin(c, url)) { code = http.GET(); if (code == 200) remote = http.getString(); http.end(); }
  }
  remote.trim();
  bool upd = remote.length() > 0 && remote != String(FW_VERSION);
  String s = "{";
  s += "\"configured\":" + String(ghConfigured() ? "true" : "false") + ",";
  s += "\"online\":"     + String(gStaUp ? "true" : "false") + ",";
  s += "\"current\":\""  + String(FW_VERSION) + "\",";
  s += "\"remote\":\""   + remote + "\",";
  s += "\"httpCode\":"   + String(code) + ",";
  s += "\"url\":\""      + url + "\",";
  s += "\"update\":"     + String(upd ? "true" : "false");
  s += "}";
  server.send(200, "application/json", s);
}

void handleGhUpdate() {
  if (!ghConfigured() || !gStaUp) {
    server.send(200, "application/json", "{\"ok\":false,\"msg\":\"brak konfiguracji GitHub lub internetu (STA)\"}");
    return;
  }
  String url = ghBase() + GH_FW_PATH + "?nc=" + String(millis());   // ?nc: omija cache CDN GitHuba
  Serial.println("[GH] pobieram firmware: " + url);
  WiFiClientSecure c; c.setInsecure();
  httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);   // GitHub potrafi przekierowac
  httpUpdate.rebootOnUpdate(true);
  httpUpdate.onProgress([](int cur, int total) {                 // podglad na Serial
    if (total) Serial.printf("[GH] %d%%\n", cur * 100 / total);
  });
  t_httpUpdate_return r = httpUpdate.update(c, url);   // blokuje; przy SUKCESIE ESP restartuje sie sam (bez odpowiedzi)
  // tu dojdziemy tylko przy bledzie:
  String msg;
  if (r == HTTP_UPDATE_FAILED)          msg = String("BLAD ") + httpUpdate.getLastError() + ": " + httpUpdate.getLastErrorString();
  else if (r == HTTP_UPDATE_NO_UPDATES) msg = "serwer nie zwrocil firmware (zly URL / brak pliku?)";
  else                                  msg = "nieznany wynik OTA";
  Serial.println("[GH] " + msg);
  server.send(200, "application/json", "{\"ok\":false,\"msg\":\"" + msg + "\"}");
}

/* ---- WiFi z panelu (zapis w NVS, bez hasel w kodzie/repo) ---- */
void handleWifiGet() {
  String ssid = prefs.getString("wssid", STA_SSID);
  String s = "{\"ssid\":\"" + ssid + "\",\"staUp\":" + String(gStaUp ? "true" : "false") + ",\"ip\":\"" + gStaIp + "\"}";
  server.send(200, "application/json", s);
}
void handleWifiSet() {
  if (server.hasArg("ssid")) {
    prefs.putString("wssid", server.arg("ssid"));
    prefs.putString("wpass", server.hasArg("pass") ? server.arg("pass") : String(""));
    server.send(200, "application/json", "{\"ok\":true}");
    gOtaReboot = true; gOtaRebootAt = millis() + 800;   // restart, by polaczyc z nowa siecia
  } else {
    server.send(200, "application/json", "{\"ok\":false}");
  }
}

/* ==============================  SETUP / LOOP  ============================= */

void setup() {
  Serial.begin(115200);
  delay(300);

  prefs.begin("eos", false);
  gHigh    = prefs.getUInt("high", 0);
  gAchMask = prefs.getUInt("ach", 0);

  // Nazwa AP z fragmentu MAC — stała dla danego egzemplarza.
  char ap[40];
  snprintf(ap, sizeof(ap), "%s-%04X", AP_SSID_PREFIX, (uint16_t)(ESP.getEfuseMac() >> 32));
  gApSsid = ap;

  // WiFi: AP zawsze wlaczony; STA (klient) tylko gdy sa dane — z NVS (panel web) lub secrets.h.
  String staSsid = prefs.getString("wssid", STA_SSID);
  String staPass = prefs.getString("wpass", STA_PASSWORD);
  bool useSta = staSsid.length() > 0;
  WiFi.mode(useSta ? WIFI_AP_STA : WIFI_AP);

  const char* apPass = (strlen(AP_PASSWORD) >= 8) ? AP_PASSWORD : nullptr;
  WiFi.softAP(gApSsid.c_str(), apPass);
  gApIp = WiFi.softAPIP().toString();
  gMac  = WiFi.macAddress();               // poprawny MAC — WiFi już zainicjalizowane

  if (useSta) {                            // laczenie NIEBLOKUJACE — status sprawdzamy w loop()
    WiFi.begin(staSsid.c_str(), staPass.c_str());
    Serial.printf("Laczenie z WiFi \"%s\" w tle...\n", staSsid.c_str());
  }

  dns.start(DNS_PORT, "*", WiFi.softAPIP());   // captive portal
  if (MDNS.begin("experienceos")) MDNS.addService("http", "tcp", 80);
  analogReadResolution(12);

  server.on("/", handleIndex);
  server.on("/api/info",  []() { server.send(200, "application/json", infoJson()); });
  server.on("/api/stats", []() { server.send(200, "application/json", statsJson()); });
  server.on("/api/bench", []() { server.send(200, "application/json", benchJson()); });
  server.on("/api/ach",   handleAch);
  server.on("/api/score", handleScore);
  server.on("/api/ping",  []() { server.sendHeader("Cache-Control", "no-store"); server.send(200, "text/plain", "pong"); });
  server.on("/api/dl",    handleDl);
  server.on("/api/ul",    HTTP_POST, handleUlReply, handleUlUpload);
  server.on("/api/cpu",   handleCpu);
  server.on("/api/fft",   handleFft);
  server.on("/api/fftbench", handleFftBench);
  server.on("/api/scope", handleScope);
  server.on("/api/ghcheck",  handleGhCheck);
  server.on("/api/ghupdate", handleGhUpdate);
  server.on("/api/wifi", HTTP_GET,  handleWifiGet);
  server.on("/api/wifi", HTTP_POST, handleWifiSet);
#ifdef EOS_RGB_PIN
  server.on("/api/led",   handleLed);
#endif
  server.onNotFound(handleIndex);              // dowolny URL -> dashboard (wyzwala portal)
  server.begin();

#ifdef EOS_RGB_PIN
  ledShow(0, 0, 0);
#endif

  Serial.println("\n==================================================");
  Serial.printf ("   ExperienceOS  v%s  ONLINE\n", FW_VERSION);
  Serial.println("==================================================");
  Serial.printf ("  Chip     : %s  rev %d  (%d rdzenie)\n", ESP.getChipModel(), ESP.getChipRevision(), ESP.getChipCores());
  Serial.printf ("  Flash    : %.1f MB   PSRAM: %s\n", ESP.getFlashChipSize()/1048576.0,
                 psramFound() ? (String(ESP.getPsramSize()/1048576.0,1)+" MB").c_str() : "brak");
  Serial.printf ("  MAC      : %s\n", gMac.c_str());
#ifdef EOS_RGB_PIN
  Serial.println("  RGB LED  : aktywna (RGB_BUILTIN)");
#else
  Serial.println("  RGB LED  : nie wykryto (ustaw RGB_LED_PIN jesli masz)");
#endif
  Serial.println("--------------------------------------------------");
  Serial.printf ("  WiFi AP  : \"%s\"  ->  http://%s\n", gApSsid.c_str(), gApIp.c_str());
  if (gStaUp) Serial.printf ("  WiFi STA : http://%s\n", gStaIp.c_str());
  Serial.println("  mDNS     : http://experienceos.local");
  Serial.printf ("  Firmware : v%s%s\n", FW_VERSION, ghConfigured() ? "   (GitHub OTA gotowe)" : "");
  Serial.println("==================================================\n");
}

void loop() {
  gLoopCount++;
  if (millis() - gLoopT >= 1000) { gLoopRate = gLoopCount; gLoopCount = 0; gLoopT = millis(); }
  dns.processNextRequest();
  server.handleClient();
#ifdef EOS_RGB_PIN
  updateLed();
#endif
  if (gOtaReboot && millis() > gOtaRebootAt) { Serial.println("OTA -> restart"); ESP.restart(); }
  if (!gStaUp && WiFi.status() == WL_CONNECTED) {    // niebokujace wykrycie polaczenia STA
    gStaUp = true; gStaIp = WiFi.localIP().toString();
    MDNS.end(); MDNS.begin("experienceos"); MDNS.addService("http", "tcp", 80);   // rozglos .local w sieci domowej
    Serial.println("WiFi STA polaczone: http://" + gStaIp + "   (albo http://experienceos.local)");
  }
}
