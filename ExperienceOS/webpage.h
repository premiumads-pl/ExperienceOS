#pragma once
// ExperienceOS - interfejs webowy (HTML/CSS/JS) w PROGMEM.
// Trzymany osobno, by preprocesor Arduino IDE nie psul wielkiego raw-stringa w .ino.

const char INDEX_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="pl"><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>ExperienceOS</title>
<style>
:root{--bg:#05070d;--cy:#22d3ee;--mg:#f472b6;--gr:#34d399;--am:#fbbf24;--tx:#cbd5e1;--dim:#64748b;--pnl:rgba(17,25,40,.72);--brd:rgba(148,163,184,.18)}
*{box-sizing:border-box;margin:0;padding:0}
html,body{height:100%}
body{background:var(--bg);color:var(--tx);font-family:ui-monospace,"SF Mono",Menlo,Consolas,monospace;overflow-x:hidden}
canvas#bg{position:fixed;inset:0;z-index:0;display:block}
.wrap{position:relative;z-index:1;max-width:1180px;margin:0 auto;padding:14px}
h1,h2{font-weight:700;letter-spacing:.06em}
.glow{text-shadow:0 0 12px currentColor}
/* boot */
#boot{position:fixed;inset:0;z-index:50;background:var(--bg);display:flex;flex-direction:column;align-items:center;justify-content:center;gap:18px;transition:opacity .7s}
#boot pre{color:var(--cy);font-size:clamp(7px,2.1vw,14px);line-height:1.15;text-shadow:0 0 14px var(--cy)}
#boot .st{color:var(--dim);letter-spacing:.3em;font-size:12px;min-height:14px}
.bar{width:min(420px,80vw);height:8px;border:1px solid var(--brd);border-radius:6px;overflow:hidden;background:rgba(255,255,255,.03)}
.bar>i{display:block;height:100%;width:0;background:linear-gradient(90deg,var(--cy),var(--mg));box-shadow:0 0 14px var(--cy);transition:width .25s}
/* header */
header{display:flex;flex-wrap:wrap;align-items:center;gap:10px 16px;padding:6px 2px 16px;border-bottom:1px solid var(--brd);margin-bottom:16px}
header .logo{font-size:20px;color:var(--cy)}
header .logo b{color:var(--mg)}
.chips{display:flex;flex-wrap:wrap;gap:6px}
.chip{font-size:11px;color:var(--dim);border:1px solid var(--brd);border-radius:20px;padding:3px 9px;white-space:nowrap}
.chip b{color:var(--tx);font-weight:600}
.up{margin-left:auto;font-size:13px;color:var(--gr)}
/* grid */
.grid{display:grid;grid-template-columns:repeat(12,1fr);gap:14px}
.panel{grid-column:span 12;background:var(--pnl);border:1px solid var(--brd);border-radius:14px;padding:16px;backdrop-filter:blur(8px);box-shadow:0 8px 30px rgba(0,0,0,.35)}
@media(min-width:820px){.c6{grid-column:span 6}.c4{grid-column:span 4}.c8{grid-column:span 8}}
.panel h2{font-size:13px;color:var(--cy);margin-bottom:14px;display:flex;align-items:center;gap:8px}
.panel h2 .dot{width:7px;height:7px;border-radius:50%;background:var(--cy);box-shadow:0 0 10px var(--cy)}
/* meters */
.meter{margin:11px 0}
.meter .lab{display:flex;justify-content:space-between;font-size:12px;margin-bottom:5px}
.meter .lab .v{color:#fff}
.track{height:10px;border-radius:6px;background:rgba(255,255,255,.05);overflow:hidden;border:1px solid var(--brd)}
.track>i{display:block;height:100%;width:0;border-radius:6px;transition:width .18s linear}
.f-cy>i{background:linear-gradient(90deg,#0891b2,var(--cy));box-shadow:0 0 10px var(--cy)}
.f-mg>i{background:linear-gradient(90deg,#be185d,var(--mg));box-shadow:0 0 10px var(--mg)}
.f-gr>i{background:linear-gradient(90deg,#059669,var(--gr));box-shadow:0 0 10px var(--gr)}
.f-am>i{background:linear-gradient(90deg,#d97706,var(--am));box-shadow:0 0 10px var(--am)}
/* buttons */
button{font-family:inherit;cursor:pointer;color:var(--tx);background:rgba(255,255,255,.04);border:1px solid var(--brd);border-radius:9px;padding:9px 14px;font-size:13px;transition:.15s}
button:hover{border-color:var(--cy);color:#fff;box-shadow:0 0 14px rgba(34,211,238,.25)}
button:active{transform:translateY(1px)}
button.on{border-color:var(--mg);color:#fff;box-shadow:0 0 14px rgba(244,114,182,.35)}
.row{display:flex;flex-wrap:wrap;gap:8px;align-items:center}
/* bench */
.bres{display:grid;grid-template-columns:repeat(3,1fr);gap:10px;margin-top:14px}
.bcard{border:1px solid var(--brd);border-radius:10px;padding:12px;text-align:center;background:rgba(255,255,255,.02)}
.bcard .n{font-size:22px;color:#fff;font-weight:700}
.bcard .u{font-size:10px;color:var(--dim);letter-spacing:.1em;text-transform:uppercase;margin-top:3px}
/* chart + game canvases */
canvas.view{width:100%;display:block;border:1px solid var(--brd);border-radius:10px;background:rgba(0,0,0,.25)}
#chart{width:100%;height:240px;display:block;border:1px solid var(--brd);border-radius:10px;background:rgba(0,0,0,.25)}
.legend{display:flex;gap:16px;font-size:11px;color:var(--dim);margin-top:8px}
.legend s{display:inline-block;width:18px;height:3px;border-radius:2px;vertical-align:middle;margin-right:6px}
/* game */
.gwrap{max-width:360px;margin:0 auto}
.dpad{display:grid;grid-template-columns:repeat(3,54px);grid-template-rows:repeat(3,54px);gap:6px;justify-content:center;margin-top:12px}
.dpad button{padding:0;font-size:20px}
.dpad .sp{visibility:hidden}
.score{display:flex;justify-content:space-between;font-size:13px;margin-top:10px}
.score b{color:var(--am)}
/* led */
input[type=color]{width:52px;height:36px;border:1px solid var(--brd);border-radius:8px;background:none;cursor:pointer;padding:2px}
select{font-family:inherit;background:rgba(255,255,255,.05);color:var(--tx);border:1px solid var(--brd);border-radius:8px;padding:7px 10px;font-size:13px}
input[type=text],input[type=password]{font-family:inherit;background:rgba(255,255,255,.05);color:var(--tx);border:1px solid var(--brd);border-radius:8px;padding:8px 10px;font-size:13px;min-width:130px}
input[type=file]{color:var(--dim);font-size:12px;max-width:100%}
#stLog{margin-top:10px;font-size:11px;color:var(--gr);background:rgba(0,0,0,.35);border:1px solid var(--brd);border-radius:8px;padding:8px 10px;max-height:160px;overflow:auto;white-space:pre-wrap;line-height:1.5}
#stLog:empty{display:none}
/* achievements */
.ach{display:grid;grid-template-columns:repeat(auto-fill,minmax(140px,1fr));gap:10px;margin-top:14px}
.badge{border:1px solid var(--brd);border-radius:10px;padding:11px;display:flex;gap:10px;align-items:center;opacity:.38;filter:grayscale(1);transition:.4s}
.badge.un{opacity:1;filter:none;border-color:rgba(52,211,153,.5);box-shadow:0 0 14px rgba(52,211,153,.12)}
.badge .ic{font-size:22px}
.badge .t{font-size:11px;line-height:1.3}
.badge .t small{color:var(--dim);display:block}
.xp{margin-top:6px}
footer{text-align:center;color:var(--dim);font-size:11px;padding:22px 0 8px}
.hide{display:none!important}
.toast{position:fixed;left:50%;bottom:22px;transform:translateX(-50%) translateY(60px);z-index:60;background:rgba(17,25,40,.95);border:1px solid var(--gr);color:#fff;padding:11px 18px;border-radius:12px;font-size:13px;box-shadow:0 0 24px rgba(52,211,153,.3);opacity:0;transition:.4s}
.toast.show{transform:translateX(-50%) translateY(0);opacity:1}
</style></head>
<body>
<canvas id="bg"></canvas>

<div id="boot">
<pre>
  _____ _  _ ___ ___ ___ ___ ___ _  _  ___ ___    ___  ___
 | __\ \/ / _ \ __| _ \_ _| __| \| |/ __| __|  / _ \/ __|
 | _| >  <|  _/ _||   /| || _|| .` | (__| _|  | (_) \__ \
 |___/_/\_\_| |___|_|_\___|___|_|\_|\___|___|  \___/|___/
</pre>
<div class="st" id="bootSt">INITIALIZING...</div>
<div class="bar"><i id="bootBar"></i></div>
</div>

<div class="wrap">
<header>
  <div class="logo glow" id="logo">EXPERIENCE<b>OS</b> <span style="font-size:12px;color:var(--dim)">v0.1</span></div>
  <div class="chips" id="chips"></div>
  <div class="up" id="uptime">--:--:--</div>
</header>

<div class="grid">

  <!-- SYSTEM MONITOR -->
  <section class="panel c6">
    <h2><span class="dot"></span>SYSTEM MONITOR <span style="color:var(--dim);font-weight:400">· 10 Hz</span></h2>
    <div class="meter"><div class="lab"><span>CPU / częstotliwość</span><span class="v" id="l_cpu">–</span></div><div class="track f-cy"><i id="b_cpu"></i></div></div>
    <div class="meter"><div class="lab"><span>Heap (RAM)</span><span class="v" id="l_heap">–</span></div><div class="track f-gr"><i id="b_heap"></i></div></div>
    <div class="meter" id="m_psram"><div class="lab"><span>PSRAM</span><span class="v" id="l_psram">–</span></div><div class="track f-mg"><i id="b_psram"></i></div></div>
    <div class="meter"><div class="lab"><span>Temperatura SoC</span><span class="v" id="l_temp">–</span></div><div class="track f-am"><i id="b_temp"></i></div></div>
    <div class="meter"><div class="lab"><span id="lab_link">Sygnał / klienci</span><span class="v" id="l_link">–</span></div><div class="track f-cy"><i id="b_link"></i></div></div>
    <div class="row" style="margin-top:12px;font-size:12px;color:var(--dim)">
      <span>Zadania FreeRTOS: <b style="color:var(--tx)" id="l_tasks">–</b></span>
      <span>Min. wolny heap: <b style="color:var(--tx)" id="l_minheap">–</b></span>
    </div>
  </section>

  <!-- CPU CONTROL -->
  <section class="panel c6">
    <h2><span class="dot"></span>CPU CONTROL · taktowanie na żywo</h2>
    <p style="font-size:12px;color:var(--dim);margin-bottom:12px">Zmień częstotliwość rdzeni i obserwuj wpływ na benchmark oraz temperaturę. Minimum 80 MHz — tego wymaga WiFi.</p>
    <div class="row">
      <button data-mhz="80" class="cpu">80 MHz</button>
      <button data-mhz="160" class="cpu">160 MHz</button>
      <button data-mhz="240" class="cpu">240 MHz</button>
    </div>
    <p style="font-size:13px;margin-top:14px">Aktualnie: <b id="cpuNow" style="color:var(--cy)">–</b> MHz</p>
  </section>

  <!-- CHART -->
  <section class="panel c6">
    <h2><span class="dot"></span>TELEMETRIA · czas rzeczywisty</h2>
    <div class="row" style="margin-bottom:10px">
      <span style="font-size:12px;color:var(--dim)">Zakres:</span>
      <button class="win on" data-w="60">1 min</button>
      <button class="win" data-w="300">5 min</button>
      <button class="win" data-w="600">10 min</button>
      <button class="win" data-w="1800">30 min</button>
      <button class="win" data-w="3600">1 h</button>
    </div>
    <canvas id="chart"></canvas>
    <div class="legend"><span><s style="background:var(--gr)"></s>Wolny heap (KB, lewa oś)</span><span><s style="background:var(--am)"></s>Temperatura (°C, prawa oś)</span></div>
  </section>

  <!-- BENCHMARK -->
  <section class="panel c6">
    <h2><span class="dot"></span>CPU BENCHMARK</h2>
    <div class="row"><button id="benchBtn">▶ Uruchom benchmark</button><span id="benchMsg" style="font-size:12px;color:var(--dim)"></span></div>
    <div class="bres">
      <div class="bcard"><div class="n" id="r_int">–</div><div class="u">MIPS · integer</div></div>
      <div class="bcard"><div class="n" id="r_flt">–</div><div class="u">MFLOPS · float</div></div>
      <div class="bcard"><div class="n" id="r_mem">–</div><div class="u">MB/s · memcpy</div></div>
    </div>
  </section>

  <!-- SPEED TEST -->
  <section class="panel c6">
    <h2><span class="dot"></span>NETWORK SPEED TEST <span style="color:var(--dim);font-weight:400">· WiFi ESP32 ↔ Ty</span></h2>
    <canvas class="view" id="gauge" height="170" style="background:transparent;border:none"></canvas>
    <div class="bres">
      <div class="bcard"><div class="n" id="stDl">–</div><div class="u">Mbps · download</div></div>
      <div class="bcard"><div class="n" id="stUl">–</div><div class="u">Mbps · upload</div></div>
      <div class="bcard"><div class="n" id="stPing">–</div><div class="u">ms · ping</div></div>
    </div>
    <div class="row" style="margin-top:12px"><button id="stBtn">▶ Zmierz łącze</button><span id="stMsg" style="font-size:12px;color:var(--dim)"></span></div>
    <pre id="stLog"></pre>
  </section>

  <!-- DSP LAB -->
  <section class="panel c12">
    <h2><span class="dot"></span>DSP LAB · FFT 256-pkt na żywo</h2>
    <div class="row" style="margin-bottom:12px">
      <span style="font-size:12px;color:var(--dim)">Sygnał:</span>
      <button data-sig="chord" class="sig on">Akord</button>
      <button data-sig="tone" class="sig">Ton 440</button>
      <button data-sig="sweep" class="sig">Sweep</button>
      <button data-sig="square" class="sig">Prostokąt</button>
      <button data-sig="noise" class="sig">Szum</button>
      <button id="dspBench">⚙ Benchmark DSP</button>
      <span id="dspMsg" style="font-size:12px;color:var(--dim)"></span>
    </div>
    <canvas class="view" id="spectrum" height="150"></canvas>
    <canvas class="view" id="waterfall" height="120" style="margin-top:10px"></canvas>
    <div class="legend"><span>Widmo (góra) · Waterfall — historia widma (dół) · zakres 0–4 kHz</span></div>
  </section>

  <!-- OSCILLOSCOPE -->
  <section class="panel c6">
    <h2><span class="dot"></span>OSCYLOSKOP ADC</h2>
    <div class="row" style="margin-bottom:10px">
      <span style="font-size:12px;color:var(--dim)">Pin ADC1:</span>
      <select id="scPin">
        <option value="1">GPIO1</option><option value="2">GPIO2</option><option value="3">GPIO3</option>
        <option value="4">GPIO4</option><option value="5">GPIO5</option><option value="6">GPIO6</option>
        <option value="7">GPIO7</option><option value="8">GPIO8</option><option value="9">GPIO9</option><option value="10">GPIO10</option>
      </select>
      <button id="scRun">▶ Start</button>
    </div>
    <canvas class="view" id="scope" height="160"></canvas>
    <div class="row" style="margin-top:10px;font-size:12px;color:var(--dim)">
      <span>Vpp: <b id="scVpp" style="color:#fff">–</b></span>
      <span>Śr: <b id="scAvg" style="color:#fff">–</b></span>
      <span>fs: <b id="scFs" style="color:#fff">–</b></span>
    </div>
    <p style="font-size:11px;color:var(--dim);margin-top:8px">Podłącz sygnał do wybranego pinu (potencjometr, PWM, czujnik). Pływający pin pokaże szum ~1,6 V.</p>
  </section>

  <!-- RGB LED -->
  <section class="panel c6 hide" id="ledPanel">
    <h2><span class="dot"></span>RGB LED · sterowanie sprzętowe</h2>
    <div class="row">
      <input type="color" id="ledColor" value="#00c8ff">
      <button data-fx="solid" class="fx">Stały</button>
      <button data-fx="rainbow" class="fx">🌈 Rainbow</button>
      <button data-fx="breathe" class="fx">Breathe</button>
      <button data-fx="off" class="fx">Wyłącz</button>
    </div>
    <p style="font-size:11px;color:var(--dim);margin-top:12px">Sterujesz wbudowaną diodą WS2812 przez sprzętowy RMT ESP32-S3.</p>
  </section>

  <!-- GAME -->
  <section class="panel c6" id="gamePanel">
    <h2><span class="dot"></span>SNAKE · rekord w pamięci NVS</h2>
    <div class="gwrap">
      <canvas class="view" id="game" width="360" height="360"></canvas>
      <div class="score"><span>Wynik: <b id="gScore" style="color:#fff">0</b></span><span>Rekord: <b id="gHigh">0</b></span></div>
      <div class="dpad">
        <span class="sp"></span><button data-d="U">▲</button><span class="sp"></span>
        <button data-d="L">◀</button><button id="gStart">⟳</button><button data-d="R">▶</button>
        <span class="sp"></span><button data-d="D">▼</button><span class="sp"></span>
      </div>
      <p style="font-size:11px;color:var(--dim);text-align:center;margin-top:8px">Klawiatura (strzałki/WASD), przyciski lub gesty na ekranie.</p>
    </div>
  </section>

  <!-- OTA -->
  <section class="panel c6">
    <h2><span class="dot"></span>OTA UPDATE · firmware przez WiFi</h2>
    <p style="font-size:12px;color:var(--dim);margin-bottom:12px">Wgraj nowy firmware (plik <b>.bin</b> z „Sketch → Export Compiled Binary" w Arduino IDE) bez podłączania kabla. Adres: <b style="color:var(--cy)">experienceos.local</b></p>
    <div class="row">
      <input type="file" id="otaFile" accept=".bin">
      <button id="otaBtn">⬆ Wgraj</button>
    </div>
    <div class="track f-cy" style="margin-top:14px"><i id="otaBar"></i></div>
    <p id="otaMsg" style="font-size:12px;color:var(--dim);margin-top:8px"></p>
  </section>

  <!-- WIFI -->
  <section class="panel c6">
    <h2><span class="dot"></span>SIEĆ WiFi · połącz z internetem</h2>
    <p style="font-size:12px;color:var(--dim);margin-bottom:12px">Status: <b id="wifiStat" style="color:var(--gr)">–</b>. Dane zapisują się w pamięci urządzenia (NVS) — nie w kodzie, więc nie trafią do repo i przeżyją aktualizację OTA.</p>
    <div class="row">
      <input type="text" id="wSsid" placeholder="Nazwa sieci (SSID)">
      <input type="password" id="wPass" placeholder="Hasło">
      <button id="wSave">Zapisz i połącz</button>
    </div>
    <p id="wMsg" style="font-size:12px;color:var(--dim);margin-top:10px"></p>
  </section>

  <!-- GITHUB UPDATE -->
  <section class="panel c6">
    <h2><span class="dot"></span>GITHUB UPDATE · OTA prosto z repo</h2>
    <p style="font-size:12px;color:var(--dim);margin-bottom:12px">ESP pobiera skompilowany <b>.bin</b> z Twojego repo GitHub i sam się przeflashuje. Wersja firmware: <b id="ghCur" style="color:var(--cy)">–</b></p>
    <div class="row"><button id="ghCheck">🔎 Sprawdź wersję</button><button id="ghUpd">⬇ Aktualizuj z GitHub</button></div>
    <p id="ghMsg" style="font-size:12px;color:var(--dim);margin-top:10px"></p>
  </section>

  <!-- ACHIEVEMENTS -->
  <section class="panel c12">
    <h2><span class="dot"></span>EXPERIENCE SCORE</h2>
    <div class="meter xp"><div class="lab"><span>Postęp odkrywania</span><span class="v" id="xpTxt">0%</span></div><div class="track f-mg"><i id="xpBar"></i></div></div>
    <div class="ach" id="achGrid"></div>
  </section>

</div>
<footer>ExperienceOS · zbudowane w całości na Twoim ESP32-S3 · <span id="fSdk"></span></footer>
</div>

<div class="toast" id="toast"></div>

<script>
"use strict";
const $=s=>document.querySelector(s);
const $$=s=>document.querySelectorAll(s);
const j=async(u)=>{const r=await fetch(u);return r.json();};
let INFO=null, statsTimer=null;

/* ---------- osiągnięcia (kolejność zgodna z ACH_KEYS w C++) ---------- */
const ACH={
  boot:['⚡','System Online','urządzenie wystartowało'],
  explorer:['🧭','Dashboard Explorer','otwarto interfejs'],
  bench:['🚀','Benchmark Run','zmierzono moc CPU'],
  led:['🎨','Painted with Light','użyto diody RGB'],
  rainbow:['🌈','Rainbow Rider','efekt tęczy'],
  snake10:['🐍','Snake: 10 pkt','pierwsze punkty'],
  snake50:['👑','Snake Master','50 punktów!'],
  marathon:['⏱️','Uptime Marathon','5 min działania'],
  konami:['🕹️','Secret Found','↑↑↓↓←→←→ B A lub 7× logo'],
  speedster:['📡','Speed Demon','zmierzono łącze WiFi'],
  overclock:['🔥','Overclocker','zmiana taktowania CPU'],
  dsp:['📊','DSP Guru','uruchomiono FFT'],
  scope:['📈','Oscyloskop','podejrzano sygnał ADC'],
  ota:['⬆️','OTA Updated','firmware przez WiFi'],
  github:['🐙','Git Pull','aktualizacja z GitHub']
};
let unlocked=new Set();

function toast(msg){const t=$('#toast');t.textContent=msg;t.classList.add('show');clearTimeout(t._h);t._h=setTimeout(()=>t.classList.remove('show'),2600);}

function buildAch(){
  const g=$('#achGrid');g.innerHTML='';
  for(const k in ACH){
    if(!INFO.rgb && (k==='led'||k==='rainbow')) continue;   // ukryj nieosiągalne bez diody
    const[ic,ti,su]=ACH[k];
    g.insertAdjacentHTML('beforeend',
      `<div class="badge" data-k="${k}"><div class="ic">${ic}</div><div class="t">${ti}<small>${su}</small></div></div>`);
  }
}
function renderAch(){
  let tot=0,got=0;
  $$('#achGrid .badge').forEach(b=>{
    tot++;const k=b.dataset.k;
    if(unlocked.has(k)){b.classList.add('un');got++;}else{b.classList.remove('un');}
  });
  const pct=tot?Math.round(got/tot*100):0;
  $('#xpBar').style.width=pct+'%';$('#xpTxt').textContent=pct+'%';
}
async function unlock(k){
  if(unlocked.has(k))return;
  const d=await j('/api/ach?set='+k);
  const before=unlocked.size;
  unlocked=new Set(d.unlocked);
  renderAch();
  if(ACH[k]&&unlocked.has(k)&&unlocked.size>before) toast('🏆 '+ACH[k][1]);
}

/* ---------- tożsamość ---------- */
function bytes(n){if(n<1024)return n+' B';if(n<1048576)return (n/1024).toFixed(0)+' KB';return (n/1048576).toFixed(n<10485760?1:0)+' MB';}
function renderInfo(){
  const c=$('#chips');
  const add=(l,v)=>c.insertAdjacentHTML('beforeend',`<span class="chip">${l} <b>${v}</b></span>`);
  add('Chip',INFO.model+' r'+INFO.rev);
  add('Rdzenie',INFO.cores+' @ '+INFO.cpuMax+'MHz');
  add('Flash',bytes(INFO.flash));
  add('PSRAM',INFO.psram?bytes(INFO.psram):'brak');
  add('MAC',INFO.mac);
  add('AP',INFO.apIp);
  if(INFO.staUp)add('STA',INFO.staIp);
  $('#fSdk').textContent='IDF '+INFO.sdk;
  if(INFO.fw)$('#ghCur').textContent=INFO.fw;
  if(!INFO.psram)$('#m_psram').classList.add('hide');
  if(INFO.rgb)$('#ledPanel').classList.remove('hide');
}

/* ---------- system monitor ---------- */
function setBar(id,pct){$(id).style.width=Math.max(0,Math.min(100,pct))+'%';}
function fmtUp(ms){let s=Math.floor(ms/1000);const h=Math.floor(s/3600);s%=3600;const m=Math.floor(s/60);s%=60;
  return String(h).padStart(2,'0')+':'+String(m).padStart(2,'0')+':'+String(s).padStart(2,'0');}
let chartBuf=[],chartWin=60,chartLastPush=0;
function chartSample(heapFree,temp){const now=Date.now()/1000;chartBuf.push({t:now,h:heapFree,c:temp});
  const cutoff=now-3600;while(chartBuf.length&&chartBuf[0].t<cutoff)chartBuf.shift();}

async function tick(){
  let s;try{s=await j('/api/stats');}catch(e){return;}
  const heapUsed=s.heapSize-s.heapFree;
  setBar('#b_cpu',s.cpuMhz/240*100);$('#l_cpu').textContent=s.cpuMhz+' MHz';
  setBar('#b_heap',heapUsed/s.heapSize*100);$('#l_heap').textContent=bytes(s.heapFree)+' wolne / '+bytes(s.heapSize);
  if(INFO.psram&&s.psramSize){setBar('#b_psram',(s.psramSize-s.psramFree)/s.psramSize*100);$('#l_psram').textContent=bytes(s.psramFree)+' wolne';}
  const tp=Math.max(0,Math.min(100,(s.temp-20)/60*100));
  setBar('#b_temp',tp);$('#l_temp').textContent=s.temp.toFixed(1)+' °C';
  if(s.staUp){setBar('#b_link',Math.max(0,Math.min(100,(s.rssi+100)*2)));$('#l_link').textContent=s.rssi+' dBm';$('#lab_link').textContent='Sygnał WiFi (STA)';}
  else{setBar('#b_link',Math.min(100,s.apClients*25));$('#l_link').textContent=s.apClients+' podłączonych';$('#lab_link').textContent='Klienci AP';}
  $('#l_tasks').textContent=s.tasks;
  $('#l_minheap').textContent=bytes(s.heapMin);
  $('#uptime').textContent=fmtUp(s.up);
  const nowMs=performance.now();
  if(nowMs-chartLastPush>=500){chartLastPush=nowMs;chartSample(s.heapFree,s.temp);drawChart();}   // probka + rysowanie 2 Hz
  if(s.up>300000)unlock('marathon');
}

/* ---------- wykres (stala wysokosc, osie, zakres czasu) ---------- */
// uzywane tez przez spektrum/oscyloskop/gauge:
function fitCanvas(cv){const r=cv.getBoundingClientRect();const dpr=window.devicePixelRatio||1;
  const hAttr=parseInt(cv.getAttribute('height'));cv.style.height=hAttr+'px';cv.width=Math.max(1,Math.round(r.width*dpr));cv.height=Math.round(hAttr*dpr);
  const ctx=cv.getContext('2d');ctx.setTransform(dpr,0,0,dpr,0,0);return {ctx,w:r.width,h:hAttr};}
// sizing odporny na petle skalowania: bierze clientWidth/clientHeight (z CSS), nie backing store:
function prepCanvas(cv){const dpr=window.devicePixelRatio||1,w=cv.clientWidth,h=cv.clientHeight;
  const bw=Math.max(1,Math.round(w*dpr)),bh=Math.max(1,Math.round(h*dpr));
  if(cv.width!==bw)cv.width=bw; if(cv.height!==bh)cv.height=bh;
  const ctx=cv.getContext('2d');ctx.setTransform(dpr,0,0,dpr,0,0);return {ctx,w,h};}
function niceNum(x){if(!(x>0))return 1;const e=Math.floor(Math.log10(x)),f=x/Math.pow(10,e);
  const n=f<1.5?1:f<3?2:f<7?5:10;return n*Math.pow(10,e);}
function drawChart(){
  const cv=$('#chart');if(!cv||!cv.clientWidth)return;
  const{ctx,w,h}=prepCanvas(cv);ctx.clearRect(0,0,w,h);
  const mL=46,mR=34,mT=10,mB=20,pw=w-mL-mR,ph=h-mT-mB;
  const now=Date.now()/1000,t0=now-chartWin;
  const pts=chartBuf.filter(p=>p.t>=t0-2);
  ctx.font='10px monospace';ctx.textBaseline='middle';ctx.lineWidth=1;
  if(pts.length<2){ctx.fillStyle='#64748b';ctx.textAlign='center';ctx.textBaseline='middle';ctx.fillText('zbieranie danych...',w/2,h/2);return;}
  // --- os Y: heap (KB) po lewej ---
  let hmin=1e18,hmax=-1e18;for(const p of pts){if(p.h<hmin)hmin=p.h;if(p.h>hmax)hmax=p.h;}
  let span=hmax-hmin;if(span<2048)span=2048;const step=niceNum(span*1.3/4);
  let lo=Math.floor((hmin-span*0.15)/step)*step,hi=Math.ceil((hmax+span*0.15)/step)*step;
  if(hi<=lo)hi=lo+step;
  ctx.textAlign='right';
  for(let v=lo;v<=hi+1;v+=step){const y=mT+ph-(v-lo)/(hi-lo)*ph;
    ctx.strokeStyle='rgba(148,163,184,.10)';ctx.beginPath();ctx.moveTo(mL,y);ctx.lineTo(mL+pw,y);ctx.stroke();
    ctx.fillStyle='#64748b';ctx.fillText((v/1024).toFixed(0),mL-6,y);}
  // --- os Y: temperatura (°C) po prawej ---
  const tlo=15,thi=70;ctx.textAlign='left';
  for(let tv=20;tv<=70;tv+=10){const y=mT+ph-(tv-tlo)/(thi-tlo)*ph;ctx.fillStyle='#8a6d3b';ctx.fillText(tv+'°',mL+pw+5,y);}
  // --- os X: czas ---
  ctx.textAlign='center';ctx.textBaseline='top';const divs=6;
  for(let i=0;i<=divs;i++){const x=mL+pw*i/divs;
    ctx.strokeStyle='rgba(148,163,184,.07)';ctx.beginPath();ctx.moveTo(x,mT);ctx.lineTo(x,mT+ph);ctx.stroke();
    let lab;if(i===divs)lab='teraz';else if(chartWin<=60)lab='-'+Math.round(chartWin*(1-i/divs))+'s';
    else lab='-'+Math.round(chartWin*(1-i/divs)/60)+'m';
    ctx.fillStyle='#64748b';ctx.fillText(lab,x,mT+ph+4);}
  // --- serie ---
  const X=t=>mL+(t-t0)/chartWin*pw;
  ctx.lineWidth=1.4;ctx.strokeStyle='#fbbf24';ctx.beginPath();
  pts.forEach((p,i)=>{const x=X(p.t),y=mT+ph-(Math.max(tlo,Math.min(thi,p.c))-tlo)/(thi-tlo)*ph;i?ctx.lineTo(x,y):ctx.moveTo(x,y);});
  ctx.stroke();
  ctx.lineWidth=2;ctx.strokeStyle='#34d399';ctx.beginPath();
  pts.forEach((p,i)=>{const x=X(p.t),y=mT+ph-(p.h-lo)/(hi-lo)*ph;i?ctx.lineTo(x,y):ctx.moveTo(x,y);});
  ctx.stroke();
}
$$('.win').forEach(b=>b.addEventListener('click',()=>{chartWin=+b.dataset.w;$$('.win').forEach(x=>x.classList.toggle('on',x===b));drawChart();}));

/* ---------- benchmark ---------- */
$('#benchBtn').addEventListener('click',async()=>{
  const b=$('#benchBtn');b.disabled=true;$('#benchMsg').textContent='liczę na krzemie...';
  try{const r=await j('/api/bench');
    animateNum('#r_int',r.intMips,0);animateNum('#r_flt',r.floatMflops,1);animateNum('#r_mem',r.memMBs,0);
    $('#benchMsg').textContent='gotowe w '+r.ms+' ms';unlock('bench');
  }catch(e){$('#benchMsg').textContent='błąd';}
  b.disabled=false;
});
function animateNum(sel,to,dec){const el=$(sel);let cur=0;const st=performance.now();
  const step=t=>{const p=Math.min(1,(t-st)/600);el.textContent=(to*(0.5-Math.cos(p*Math.PI)/2)).toFixed(dec);
    if(p<1)requestAnimationFrame(step);else el.textContent=to.toFixed(dec);};requestAnimationFrame(step);}

/* ---------- SPEED TEST (przepustowosc WiFi ESP32 <-> przegladarka) ---------- */
const gv2=$('#gauge');let gaugeMax=50;
function drawGauge(val,max){
  const{ctx,w,h}=fitCanvas(gv2);ctx.clearRect(0,0,w,h);
  const cx=w/2,cy=h-6,r=Math.min(w/2-14,h-24);
  ctx.lineWidth=12;ctx.lineCap='round';
  ctx.strokeStyle='rgba(148,163,184,.15)';
  ctx.beginPath();ctx.arc(cx,cy,r,Math.PI,2*Math.PI);ctx.stroke();
  const frac=Math.max(0,Math.min(1,val/max)),ang=Math.PI+frac*Math.PI;
  const g=ctx.createLinearGradient(cx-r,0,cx+r,0);g.addColorStop(0,'#22d3ee');g.addColorStop(1,'#f472b6');
  ctx.strokeStyle=g;ctx.beginPath();ctx.arc(cx,cy,r,Math.PI,ang);ctx.stroke();
  ctx.strokeStyle='#fff';ctx.lineWidth=2;ctx.beginPath();ctx.moveTo(cx,cy);
  ctx.lineTo(cx+Math.cos(ang)*(r-4),cy+Math.sin(ang)*(r-4));ctx.stroke();
  ctx.fillStyle='#fff';ctx.textAlign='center';ctx.font='700 30px monospace';ctx.fillText(val.toFixed(1),cx,cy-16);
  ctx.fillStyle='#64748b';ctx.font='11px monospace';ctx.fillText('Mbps download',cx,cy-2);
}
function gaugeTo(v){
  gaugeMax=v<=20?20:v<=50?50:v<=100?100:Math.ceil(v/50)*50;
  const st=performance.now();
  const step=t=>{const p=Math.min(1,(t-st)/700);drawGauge(v*(0.5-Math.cos(p*Math.PI)/2),gaugeMax);if(p<1)requestAnimationFrame(step);};
  requestAnimationFrame(step);
}
function stlog(m){const el=$('#stLog');const t=(performance.now()/1000).toFixed(2);el.textContent+='['+t+'s] '+m+'\n';el.scrollTop=el.scrollHeight;}
async function fetchT(url,opts,ms){const ctrl=new AbortController();const to=setTimeout(()=>ctrl.abort(),ms||10000);
  try{return await fetch(url,Object.assign({signal:ctrl.signal},opts));}finally{clearTimeout(to);}}
async function runSpeed(){
  const btn=$('#stBtn'),msg=$('#stMsg');btn.disabled=true;$('#stLog').textContent='';
  const hadStats=!!statsTimer,hadDsp=!!dspTimer;                    // wstrzymaj telemetrie i DSP na czas pomiaru
  if(statsTimer){clearInterval(statsTimer);statsTimer=null;}
  if(dspTimer){clearInterval(dspTimer);dspTimer=null;}
  stlog('START — wstrzymano telemetrie'+(hadDsp?' i DSP':''));
  try{
    // === PING ===
    msg.textContent='ping...';stlog('PING /api/ping x8');
    let ps=[];
    for(let i=0;i<8;i++){const t=performance.now();
      await fetchT('/api/ping?_='+Math.random(),{cache:'no-store'},4000);
      const dt=performance.now()-t;ps.push(dt);stlog('  #'+(i+1)+': '+dt.toFixed(1)+' ms');}
    ps.sort((a,b)=>a-b);$('#stPing').textContent=ps[0].toFixed(1);stlog('PING min='+ps[0].toFixed(1)+' ms');
    // === DOWNLOAD (streaming z licznikiem) ===
    const dlBytes=2621440;                                          // 2.5 MB (5x wiecej)
    msg.textContent='pobieranie...';stlog('DOWNLOAD GET /api/dl?bytes='+dlBytes+' ('+(dlBytes/1048576).toFixed(1)+' MB)');
    const ctrl=new AbortController();const to=setTimeout(()=>{stlog('  TIMEOUT 40s — przerywam');ctrl.abort();},40000);
    const t0=performance.now();let received=0,lastLog=t0;
    try{
      const resp=await fetch('/api/dl?bytes='+dlBytes+'&_='+Math.random(),{cache:'no-store',signal:ctrl.signal});
      stlog('  HTTP '+resp.status+' · Content-Length='+(resp.headers.get('content-length')||'?'));
      const reader=resp.body.getReader();
      for(;;){const{done,value}=await reader.read();if(done)break;received+=value.length;
        if(performance.now()-lastLog>300){stlog('  odebrano '+received+' B');lastLog=performance.now();}}
    }finally{clearTimeout(to);}
    const dsec=(performance.now()-t0)/1000;stlog('  KONIEC: '+received+' B w '+dsec.toFixed(2)+' s');
    if(received>0){const dl=received*8/dsec/1e6;$('#stDl').textContent=dl.toFixed(1);gaugeTo(dl);stlog('DOWNLOAD = '+dl.toFixed(2)+' Mbps');}
    else{$('#stDl').textContent='–';stlog('DOWNLOAD: 0 B! sprawdz Serial ESP: powinno byc [DL] start/done');}
    // === UPLOAD ===
    const ulBytes=1048576;msg.textContent='wysyłanie...';stlog('UPLOAD POST /api/ul '+ulBytes+' B ('+(ulBytes/1048576).toFixed(1)+' MB)');
    const t1=performance.now();
    const fd=new FormData();fd.append('data',new Blob(['X'.repeat(ulBytes)]),'up.bin');
    const res=await fetchT('/api/ul',{method:'POST',body:fd},40000);
    const jr=await res.json();const got=jr.bytes||0,usec=(performance.now()-t1)/1000;
    if(got>0){const ul=got*8/usec/1e6;$('#stUl').textContent=ul.toFixed(1);stlog('UPLOAD = '+ul.toFixed(2)+' Mbps ('+got+' B / '+usec.toFixed(2)+' s)');}
    else{$('#stUl').textContent='–';stlog('UPLOAD: serwer odebrał 0 B');}
    msg.textContent='gotowe · przepustowość WiFi ESP32 (nie internetu)';stlog('ZAKOŃCZONO ✓');unlock('speedster');
  }catch(e){const ab=e&&e.name==='AbortError';msg.textContent=ab?'timeout — brak odpowiedzi':'błąd testu';
    stlog('BŁĄD: '+(ab?'przekroczono czas oczekiwania':(e&&e.message?e.message:e)));}
  finally{if(hadStats){statsTimer=setInterval(tick,100);}if(hadDsp){dspStart();}
    stlog('wznowiono telemetrie'+(hadDsp?' i DSP':''));btn.disabled=false;}
}
$('#stBtn').addEventListener('click',runSpeed);

/* ---------- RGB LED ---------- */
function hx(h){return[parseInt(h.substr(1,2),16),parseInt(h.substr(3,2),16),parseInt(h.substr(5,2),16)];}
async function led(fx){
  const[r,g,b]=hx($('#ledColor').value);
  await j(`/api/led?fx=${fx}&r=${r}&g=${g}&b=${b}`);
  $$('.fx').forEach(x=>x.classList.toggle('on',x.dataset.fx===fx));
  unlock('led');if(fx==='rainbow')unlock('rainbow');
}
$$('.fx').forEach(x=>x.addEventListener('click',()=>led(x.dataset.fx)));
$('#ledColor').addEventListener('change',()=>led('solid'));

/* ---------- SNAKE ---------- */
const G=18,gv=$('#game'),gx=gv.getContext('2d');let cell=gv.width/G;
let snake,dir,ndir,food,gScore,alive=false,gTimer=null;
function spawn(){while(true){const f={x:(Math.random()*G)|0,y:(Math.random()*G)|0};
  if(!snake.some(s=>s.x===f.x&&s.y===f.y)){food=f;return;}}}
function gInit(){snake=[{x:9,y:9},{x:8,y:9},{x:7,y:9}];dir={x:1,y:0};ndir=dir;gScore=0;alive=true;spawn();$('#gScore').textContent=0;
  clearInterval(gTimer);gTimer=setInterval(gStep,110);gDraw();}
function gStep(){
  if(!alive)return;dir=ndir;
  const head={x:(snake[0].x+dir.x+G)%G,y:(snake[0].y+dir.y+G)%G};
  if(snake.some(s=>s.x===head.x&&s.y===head.y)){gOver();return;}
  snake.unshift(head);
  if(head.x===food.x&&head.y===food.y){gScore++;$('#gScore').textContent=gScore;
    if(gScore>=10)unlock('snake10');if(gScore>=50)unlock('snake50');spawn();}
  else snake.pop();
  gDraw();
}
function gDraw(){
  gx.clearRect(0,0,gv.width,gv.height);
  gx.fillStyle='rgba(34,211,238,.06)';
  for(let i=0;i<G;i++)for(let k=0;k<G;k++)if((i+k)&1)gx.fillRect(i*cell,k*cell,cell,cell);
  gx.fillStyle='#f472b6';gx.shadowColor='#f472b6';gx.shadowBlur=12;
  gx.fillRect(food.x*cell+2,food.y*cell+2,cell-4,cell-4);
  gx.shadowBlur=8;
  snake.forEach((s,i)=>{gx.fillStyle=i?'#22d3ee':'#a7f3d0';gx.shadowColor='#22d3ee';
    gx.fillRect(s.x*cell+1,s.y*cell+1,cell-2,cell-2);});
  gx.shadowBlur=0;
}
async function gOver(){
  alive=false;clearInterval(gTimer);
  gx.fillStyle='rgba(5,7,13,.72)';gx.fillRect(0,0,gv.width,gv.height);
  gx.fillStyle='#fff';gx.textAlign='center';gx.font='700 26px monospace';gx.fillText('GAME OVER',gv.width/2,gv.height/2-6);
  gx.font='13px monospace';gx.fillStyle='#94a3b8';gx.fillText('wynik '+gScore+' · ⟳ by zagrać',gv.width/2,gv.height/2+20);
  try{const d=await j('/api/score?s='+gScore);$('#gHigh').textContent=d.high;if(d.isNew)toast('🏆 Nowy rekord: '+d.high);}catch(e){}
}
function turn(d){const map={U:{x:0,y:-1},D:{x:0,y:1},L:{x:-1,y:0},R:{x:1,y:0}};const nd=map[d];
  if(!alive||!nd)return;if(nd.x===-dir.x&&nd.y===-dir.y)return;ndir=nd;}
$('#gStart').addEventListener('click',gInit);
$$('.dpad [data-d]').forEach(b=>b.addEventListener('click',()=>turn(b.dataset.d)));
addEventListener('keydown',e=>{const m={ArrowUp:'U',ArrowDown:'D',ArrowLeft:'L',ArrowRight:'R',w:'U',s:'D',a:'L',d:'R'};
  if(m[e.key]){turn(m[e.key]);if(['ArrowUp','ArrowDown','ArrowLeft','ArrowRight'].includes(e.key))e.preventDefault();}});
let tsx,tsy;
gv.addEventListener('touchstart',e=>{tsx=e.touches[0].clientX;tsy=e.touches[0].clientY;},{passive:true});
gv.addEventListener('touchend',e=>{if(tsx==null)return;const dx=e.changedTouches[0].clientX-tsx,dy=e.changedTouches[0].clientY-tsy;
  if(Math.abs(dx)>Math.abs(dy))turn(dx>0?'R':'L');else turn(dy>0?'D':'U');tsx=null;},{passive:true});

/* ---------- Konami ---------- */
const KON=['ArrowUp','ArrowUp','ArrowDown','ArrowDown','ArrowLeft','ArrowRight','ArrowLeft','ArrowRight','b','a'];let ki=0;
addEventListener('keydown',e=>{ki=(e.key.toLowerCase()===KON[ki].toLowerCase()||e.key===KON[ki])?ki+1:0;
  if(ki===KON.length){ki=0;unlock('konami');if(INFO.rgb)led('rainbow');}});
let logoTaps=0;   // wersja dla dotyku: 7x tapniecie w logo tez odblokowuje Secret Found
$('#logo').addEventListener('click',()=>{if(++logoTaps>=7){logoTaps=0;unlock('konami');if(INFO&&INFO.rgb)led('rainbow');}});

/* ---------- CPU CONTROL ---------- */
async function setCpu(m){
  try{const d=await j('/api/cpu?mhz='+m);$('#cpuNow').textContent=d.mhz;
    $$('.cpu').forEach(x=>x.classList.toggle('on',+x.dataset.mhz===d.mhz));unlock('overclock');}catch(e){}
}
$$('.cpu').forEach(x=>x.addEventListener('click',()=>setCpu(x.dataset.mhz)));

/* ---------- DSP LAB (widmo + waterfall) ---------- */
let dspSig='chord',dspTimer=null,wfInit=false;
function dspStart(){if(!dspTimer)dspTimer=setInterval(dspTick,80);}   // ~12 Hz
async function dspTick(){
  let d;try{d=await j('/api/fft?sig='+dspSig+'&_='+Math.random());}catch(e){return;}
  drawSpectrum(d.bins);drawWaterfall(d.bins);
}
function drawSpectrum(bins){
  const cv=$('#spectrum');if(!cv.clientWidth)return;const{ctx,w,h}=fitCanvas(cv);ctx.clearRect(0,0,w,h);
  const n=bins.length,bw=w/n;
  for(let i=0;i<n;i++){const v=bins[i],bh=v*(h-4);
    ctx.fillStyle=`hsl(${185+i/n*150},90%,${34+v*34}%)`;
    ctx.fillRect(i*bw,h-bh,Math.max(1,bw-1),bh);}
}
function drawWaterfall(bins){
  const cv=$('#waterfall');if(!cv.clientWidth)return;
  if(!wfInit){const dpr=window.devicePixelRatio||1,r=cv.getBoundingClientRect();
    cv.style.height='120px';cv.width=Math.round(r.width*dpr);cv.height=Math.round(120*dpr);cv._c=cv.getContext('2d');wfInit=true;}
  const c=cv._c,W=cv.width,H=cv.height,n=bins.length,bw=W/n;
  c.drawImage(cv,0,-1);                                   // przesun historie o 1 px w gore
  for(let i=0;i<n;i++){const v=bins[i];c.fillStyle=`hsl(${265-v*230},90%,${8+v*56}%)`;
    c.fillRect(Math.floor(i*bw),H-2,Math.ceil(bw),2);}
}
$$('.sig').forEach(x=>x.addEventListener('click',()=>{dspSig=x.dataset.sig;
  $$('.sig').forEach(y=>y.classList.toggle('on',y===x));unlock('dsp');}));
$('#dspBench').addEventListener('click',async()=>{$('#dspMsg').textContent='liczę...';
  try{const d=await j('/api/fftbench');
    $('#dspMsg').textContent=Math.round(d.fftPerSec).toLocaleString('pl-PL')+' FFT/s · '+d.n+'-pkt · '+d.us+' µs/FFT';unlock('dsp');}
  catch(e){$('#dspMsg').textContent='błąd';}});

/* ---------- OSCILLOSCOPE (ADC) ---------- */
let scTimer=null;
function scToggle(){const btn=$('#scRun');
  if(scTimer){clearInterval(scTimer);scTimer=null;btn.textContent='▶ Start';btn.classList.remove('on');return;}
  btn.textContent='■ Stop';btn.classList.add('on');scTimer=setInterval(scTick,110);}
async function scTick(){let d;try{d=await j('/api/scope?pin='+$('#scPin').value+'&_='+Math.random());}catch(e){return;}drawScope(d);unlock('scope');}
function drawScope(d){
  const cv=$('#scope');if(!cv.clientWidth)return;const{ctx,w,h}=fitCanvas(cv);ctx.clearRect(0,0,w,h);
  ctx.strokeStyle='rgba(148,163,184,.1)';for(let i=0;i<=4;i++){const y=h*i/4;ctx.beginPath();ctx.moveTo(0,y);ctx.lineTo(w,y);ctx.stroke();}
  const mv=d.mv,n=mv.length;let mn=1e9,mx=-1e9,sum=0;
  for(const v of mv){if(v<mn)mn=v;if(v>mx)mx=v;sum+=v;}
  $('#scVpp').textContent=(mx-mn)+' mV';$('#scAvg').textContent=Math.round(sum/n)+' mV';$('#scFs').textContent=(d.fs/1000).toFixed(1)+' kSa/s';
  ctx.strokeStyle='#22d3ee';ctx.lineWidth=2;ctx.beginPath();
  for(let i=0;i<n;i++){const x=w*i/(n-1),y=h-6-(mv[i]/3300)*(h-12);i?ctx.lineTo(x,y):ctx.moveTo(x,y);}
  ctx.stroke();
}
$('#scRun').addEventListener('click',scToggle);

/* ---------- OTA UPDATE ---------- */
$('#otaBtn').addEventListener('click',()=>{
  const f=$('#otaFile').files[0],msg=$('#otaMsg'),bar=$('#otaBar');
  if(!f){msg.textContent='najpierw wybierz plik .bin';return;}
  const fd=new FormData();fd.append('firmware',f,f.name);
  const xhr=new XMLHttpRequest();xhr.open('POST','/api/ota');
  xhr.upload.onprogress=e=>{if(e.lengthComputable){const p=Math.round(e.loaded/e.total*100);bar.style.width=p+'%';msg.textContent='wysyłanie '+p+'%';}};
  xhr.onload=()=>{try{const r=JSON.parse(xhr.responseText);
    if(r.ok){bar.style.width='100%';msg.textContent='✓ wgrane — ESP restartuje się, połącz się ponownie za chwilę';unlock('ota');}
    else msg.textContent='✗ błąd aktualizacji (zły plik?)';}catch(e){msg.textContent='✗ błąd odpowiedzi';}};
  xhr.onerror=()=>{msg.textContent='✗ błąd połączenia';};
  msg.textContent='wysyłanie...';xhr.send(fd);
});

/* ---------- GITHUB UPDATE (OTA z repo) ---------- */
async function ghCheck(){const m=$('#ghMsg');m.textContent='sprawdzam repo...';
  try{const d=await j('/api/ghcheck');if(d.current)$('#ghCur').textContent=d.current;
    if(!d.configured){m.textContent='ustaw GH_OWNER i GH_REPO na górze szkicu';return;}
    if(!d.online){m.textContent='wymaga internetu — ustaw STA_SSID / STA_PASSWORD';return;}
    if(!d.remote){m.innerHTML='⚠ nie znalazłem wersji w repo (HTTP '+d.httpCode+'). Otwórz w przeglądarce:<br><a href="'+d.url+'" target="_blank" style="color:var(--cy)">'+d.url+'</a><br>Jeśli 404 — GitHub Actions musi najpierw zbudować firmware (patrz zakładka Actions w repo).';return;}
    m.innerHTML = d.update ? ('🟢 dostępna nowa wersja: <b style="color:var(--gr)">'+d.remote+'</b> (masz '+d.current+') — kliknij „Aktualizuj z GitHub"')
                           : ('✓ masz najnowszą wersję ('+d.current+')');
  }catch(e){m.textContent='błąd sprawdzania';}}
async function ghUpdate(){const m=$('#ghMsg');
  m.textContent='pobieram firmware z GitHub i flashuję — ESP zrestartuje się, połącz ponownie za chwilę...';
  try{const d=await j('/api/ghupdate');if(!d.ok)m.textContent='✗ '+(d.msg||'błąd');else unlock('github');}
  catch(e){m.textContent='ESP pobiera i restartuje się — połącz ponownie za chwilę';unlock('github');}}
$('#ghCheck').addEventListener('click',ghCheck);
$('#ghUpd').addEventListener('click',ghUpdate);

/* ---------- SIEĆ WiFi ---------- */
async function wifiLoad(){try{const d=await j('/api/wifi');$('#wSsid').value=d.ssid||'';
  $('#wifiStat').textContent=d.staUp?('połączono · '+d.ip):'tryb AP (bez internetu)';
  $('#wifiStat').style.color=d.staUp?'var(--gr)':'var(--am)';}catch(e){}}
$('#wSave').addEventListener('click',async()=>{
  const ssid=$('#wSsid').value.trim(),pass=$('#wPass').value;
  if(!ssid){$('#wMsg').textContent='podaj nazwę sieci (SSID)';return;}
  $('#wMsg').textContent='zapisuję i restartuję ESP — za chwilę połącz się ponownie z siecią ExperienceOS...';
  const body=new URLSearchParams();body.set('ssid',ssid);body.set('pass',pass);
  try{await fetch('/api/wifi',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:body.toString()});}catch(e){}
});

/* ---------- tło ---------- */
function initBg(){
  const cv=$('#bg'),c=cv.getContext('2d');let W,H,dots=[];
  function rs(){W=cv.width=innerWidth;H=cv.height=innerHeight;dots=Array.from({length:70},()=>({
    x:Math.random()*W,y:Math.random()*H,vx:(Math.random()-.5)*.3,vy:(Math.random()-.5)*.3,r:Math.random()*1.6+.4}));}
  rs();addEventListener('resize',rs);
  (function loop(){c.clearRect(0,0,W,H);
    for(const d of dots){d.x=(d.x+d.vx+W)%W;d.y=(d.y+d.vy+H)%H;
      c.fillStyle='rgba(34,211,238,.5)';c.beginPath();c.arc(d.x,d.y,d.r,0,7);c.fill();}
    c.strokeStyle='rgba(34,211,238,.05)';
    for(let i=0;i<dots.length;i++)for(let k=i+1;k<dots.length;k++){const a=dots[i],b=dots[k];
      const dx=a.x-b.x,dy=a.y-b.y,l=dx*dx+dy*dy;if(l<9000){c.globalAlpha=1-l/9000;
        c.beginPath();c.moveTo(a.x,a.y);c.lineTo(b.x,b.y);c.stroke();c.globalAlpha=1;}}
    requestAnimationFrame(loop);})();
}

/* ---------- rozruch ---------- */
async function boot(){
  const st=$('#bootSt'),bar=$('#bootBar');
  const steps=['DETECTING CHIP...','MOUNTING NVS...','STARTING WIFI AP...','SPAWNING WEB SERVER...','LOADING MODULES...','READY.'];
  for(let i=0;i<steps.length;i++){st.textContent=steps[i];bar.style.width=((i+1)/steps.length*100)+'%';await new Promise(r=>setTimeout(r,260));}
  try{INFO=await j('/api/info');}catch(e){INFO={model:'ESP32-S3',rev:0,cores:2,cpuMax:240,flash:0,psram:0,mac:'?',apIp:'192.168.4.1',staIp:'',staUp:false,sdk:'?',rgb:false};}
  renderInfo();buildAch();
  try{const a=await j('/api/ach');unlocked=new Set(a.unlocked);$('#gHigh').textContent=a.high;}catch(e){}
  await unlock('boot');await unlock('explorer');
  initBg();gInit();
  try{drawGauge(0,50);}catch(e){}
  dspStart();
  try{const c=await j('/api/cpu');$('#cpuNow').textContent=c.mhz;$$('.cpu').forEach(x=>x.classList.toggle('on',+x.dataset.mhz===c.mhz));}catch(e){}
  wifiLoad();
  statsTimer=setInterval(tick,100);tick();
  const bd=$('#boot');bd.style.opacity='0';setTimeout(()=>bd.classList.add('hide'),720);
}
boot();
</script>
</body></html>)HTML";
