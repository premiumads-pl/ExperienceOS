/* ExperienceOS Hardware Explorer — Frontend */

const $ = (sel) => document.querySelector(sel);

// ── Boot ─────────────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
  loadSystemInfo();
  loadPinMap();
  $('btn-run').addEventListener('click', startDiagnostics);
});

// ── System Info ────────────────────────────────────────────────────
async function loadSystemInfo() {
  try {
    const res = await fetch('/api/info');
    const data = await res.json();
    const i = data.info;

    $('i-board').textContent   = i.board;
    $('i-cpu').textContent     = `${i.cores} cores @ ${i.cpu_mhz} MHz (rev ${i.revision})`;
    $('i-flash').textContent   = `${i.flash_mb.toFixed(1)} MB`;
    $('i-psram').textContent   = i.psram_mb > 0 ? `${i.psram_mb.toFixed(1)} MB` : 'None';
    $('i-heap').textContent    = `${(i.free_heap / 1024).toFixed(0)} KB free`;
    $('i-mac').textContent     = i.mac;
    $('i-sdk').textContent     = i.sdk;
    $('i-chipid').textContent  = i.chip_id;

    const badges = $('feature-badges');
    badges.innerHTML = '';
    const features = [
      { key: 'wifi', label: 'WiFi' },
      { key: 'ble', label: 'BLE' },
      { key: 'usb_otg', label: 'USB OTG' },
    ];
    features.forEach(f => {
      const span = document.createElement('span');
      span.className = 'badge' + (i[f.key] ? ' active' : '');
      span.textContent = f.label;
      badges.appendChild(span);
    });
  } catch (e) {
    console.error('Failed to load system info', e);
  }
}

// ── Diagnostics ────────────────────────────────────────────────────
let pollTimer = null;

async function startDiagnostics() {
  const btn = $('btn-run');
  btn.disabled = true;
  btn.textContent = 'Running...';

  $('progress-wrap').classList.remove('hidden');
  $('score-wrap').classList.add('hidden');
  $('results-grid').innerHTML = '';
  $('benchmarks').classList.add('hidden');

  try {
    await fetch('/api/diagnostics/start', { method: 'POST' });
    pollTimer = setInterval(pollDiagnostics, 300);
  } catch (e) {
    btn.disabled = false;
    btn.textContent = 'Run Diagnostics';
    console.error(e);
  }
}

async function pollDiagnostics() {
  try {
    const res = await fetch('/api/diagnostics/status');
    const data = await res.json();

    $('progress-fill').style.width = data.progress + '%';
    $('progress-label').textContent = data.current + ' (' + data.progress + '%)';

    if (data.status === 'done') {
      clearInterval(pollTimer);
      $('btn-run').disabled = false;
      $('btn-run').textContent = 'Run Again';
      renderResults(data.results);
      renderScore(data.score);
      renderBenchmarks(data.results);
    }
  } catch (e) {
    console.error(e);
  }
}

function renderResults(results) {
  const grid = $('results-grid');
  grid.innerHTML = '';
  results.forEach(r => {
    const div = document.createElement('div');
    div.className = 'result-item ' + (r.pass ? 'pass' : 'fail');
    let val = r.value;
    if (r.unit === 'MB' || r.unit === 'MHz') val = val.toFixed(1);
    else if (r.unit === 'ops/s' || r.unit === 'KB/s') val = formatNumber(val);
    else if (r.unit === '°C') val = val.toFixed(1);
    else if (r.unit === 'bytes') val = formatNumber(val);
    else if (r.unit === '' && r.detail) val = r.detail;
    else if (r.unit === 'cores') val = Math.round(val);
    else if (r.unit === 'raw') val = Math.round(val);

    div.innerHTML = `
      <div class="result-name">${r.name}</div>
      <div class="result-value">${val}${r.unit && r.unit !== '' && r.unit !== 'cores' && !r.detail ? ' ' + r.unit : ''}</div>
      ${r.detail && r.unit !== '' ? '<div class="result-detail">' + r.detail + '</div>' : ''}
    `;
    grid.appendChild(div);
  });
}

function renderScore(score) {
  $('score-wrap').classList.remove('hidden');
  $('score-bar').style.width = score.percent + '%';
  $('score-pct').textContent = Math.round(score.percent) + '%';
}

function renderBenchmarks(results) {
  const bench = results.filter(r => r.category === 'Benchmark');
  if (bench.length === 0) return;

  $('benchmarks').classList.remove('hidden');
  const canvas = $('bench-chart');
  const ctx = canvas.getContext('2d');
  const dpr = window.devicePixelRatio || 1;
  const rect = canvas.getBoundingClientRect();
  canvas.width = rect.width * dpr;
  canvas.height = 200 * dpr;
  ctx.scale(dpr, dpr);
  const W = rect.width;
  const H = 200;

  const maxVal = Math.max(...bench.map(b => b.value));
  const barW = (W - 40) / bench.length;
  const colors = ['#3b82f6', '#22c55e', '#a855f7', '#f59e0b', '#ef4444'];

  ctx.clearRect(0, 0, W, H);
  bench.forEach((b, i) => {
    const h = (b.value / maxVal) * (H - 50);
    const x = 20 + i * barW + barW * 0.15;
    const w = barW * 0.7;
    ctx.fillStyle = colors[i % colors.length];
    ctx.beginPath();
    ctx.roundRect(x, H - 30 - h, w, h, 4);
    ctx.fill();

    ctx.fillStyle = '#94a3b8';
    ctx.font = '9px sans-serif';
    ctx.textAlign = 'center';
    const label = b.name.replace('CPU ', '').replace('PSRAM ', 'PS ');
    ctx.fillText(label, x + w / 2, H - 14);
  });
}

function formatNumber(n) {
  if (n >= 1e6) return (n / 1e6).toFixed(1) + 'M';
  if (n >= 1e3) return (n / 1e3).toFixed(1) + 'K';
  return Math.round(n).toString();
}

// ── GPIO Pin Map ───────────────────────────────────────────────────
let pins = [];
let selectedPin = null;

async function loadPinMap() {
  try {
    const res = await fetch('/api/gpio');
    const data = await res.json();
    pins = data.pins;
    renderBoard();
  } catch (e) {
    console.error('Failed to load pin map', e);
  }
}

function renderBoard() {
  const container = $('board-svg');
  const W = 280, H = 510;
  const ns = 'http://www.w3.org/2000/svg';

  const svg = document.createElementNS(ns, 'svg');
  svg.setAttribute('viewBox', `0 0 ${W} ${H}`);
  svg.classList.add('board-svg');

  // Board body
  const body = document.createElementNS(ns, 'rect');
  body.setAttribute('x', '40'); body.setAttribute('y', '20');
  body.setAttribute('width', '200'); body.setAttribute('height', '470');
  body.classList.add('board-body');
  svg.appendChild(body);

  // Chip label
  const chip = document.createElementNS(ns, 'text');
  chip.setAttribute('x', '140'); chip.setAttribute('y', '260');
  chip.classList.add('chip-label');
  chip.textContent = 'ESP32-S3';
  svg.appendChild(chip);

  // USB-C hint
  const usb = document.createElementNS(ns, 'text');
  usb.setAttribute('x', '140'); usb.setAttribute('y', '480');
  usb.classList.add('chip-label');
  usb.textContent = 'USB-C';
  svg.appendChild(usb);

  // Place pins left and right
  const leftPins  = pins.filter((_, i) => i % 2 === 0);
  const rightPins = pins.filter((_, i) => i % 2 === 1);

  leftPins.forEach((p, i) => {
    const y = 50 + i * (420 / Math.max(leftPins.length - 1, 1));
    addPin(svg, ns, p, 30, y, 'left');
  });

  rightPins.forEach((p, i) => {
    const y = 50 + i * (420 / Math.max(rightPins.length - 1, 1));
    addPin(svg, ns, p, 250, y, 'right');
  });

  container.innerHTML = '';
  container.appendChild(svg);
}

function addPin(svg, ns, pin, x, y, side) {
  const dot = document.createElementNS(ns, 'circle');
  dot.setAttribute('cx', x);
  dot.setAttribute('cy', y);
  dot.setAttribute('r', '5');
  dot.classList.add('pin-dot');
  dot.classList.add(pinClass(pin));
  dot.dataset.gpio = pin.gpio;
  dot.addEventListener('click', () => selectPin(pin.gpio));
  svg.appendChild(dot);

  const label = document.createElementNS(ns, 'text');
  label.setAttribute('x', side === 'left' ? x + 14 : x - 14);
  label.setAttribute('y', y + 3);
  label.classList.add('pin-label');
  label.setAttribute('text-anchor', side === 'left' ? 'start' : 'end');
  label.textContent = pin.gpio;
  svg.appendChild(label);
}

function pinClass(pin) {
  if (pin.usb) return 'cap-usb';
  if (pin.octal) return 'cap-octal';
  if (pin.strapping) return 'cap-strapping';
  if (pin.touch) return 'cap-touch';
  if (pin.adc) return 'cap-adc';
  return 'cap-default';
}

function selectPin(gpio) {
  selectedPin = gpio;
  document.querySelectorAll('.pin-dot').forEach(d => {
    d.classList.toggle('selected', parseInt(d.dataset.gpio) === gpio);
  });

  const pin = pins.find(p => p.gpio === gpio);
  if (!pin) return;

  $('pin-detail').classList.remove('hidden');
  $('pin-title').textContent = 'GPIO ' + pin.gpio;
  $('pin-label').textContent = pin.label || '';

  const caps = [
    { key: 'adc', label: 'ADC' },
    { key: 'dac', label: 'DAC' },
    { key: 'touch', label: 'Touch' },
    { key: 'pwm', label: 'PWM' },
    { key: 'i2c', label: 'I2C' },
    { key: 'spi', label: 'SPI' },
    { key: 'uart', label: 'UART' },
    { key: 'usb', label: 'USB' },
    { key: 'jtag', label: 'JTAG' },
    { key: 'octal', label: 'Octal' },
    { key: 'strapping', label: 'Strap' },
  ];

  const container = $('pin-caps');
  container.innerHTML = '';
  caps.forEach(c => {
    const tag = document.createElement('span');
    tag.className = 'cap-tag' + (pin[c.key] ? '' : ' off');
    tag.textContent = c.label;
    container.appendChild(tag);
  });
}