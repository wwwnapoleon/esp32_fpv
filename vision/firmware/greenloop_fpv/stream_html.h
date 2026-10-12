#pragma once

const char STREAM_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>GreenLoop FPV</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    background: #0d1117; color: #e6edf3;
    font-family: -apple-system, 'Segoe UI', Roboto, sans-serif;
    padding: 12px; min-height: 100vh;
  }
  .wrap { max-width: 1200px; margin: 0 auto; }

  .header {
    display: flex; justify-content: space-between; align-items: center;
    margin-bottom: 12px; padding-bottom: 10px;
    border-bottom: 1px solid #21262d;
  }
  .header h1 {
    font-size: 1.15rem; color: #3fb950;
    font-weight: 700; letter-spacing: -0.02em;
  }
  .header .settings-btn {
    padding: 8px 16px;
    background: #21262d;
    color: #58a6ff;
    border: 1px solid #30363d;
    border-radius: 8px;
    text-decoration: none;
    font-size: 0.85rem;
    font-weight: 600;
    transition: all 0.15s;
    white-space: nowrap;
  }
  .header .settings-btn:hover {
    background: #30363d;
    border-color: #3fb950;
    color: #3fb950;
  }
  @media (max-width: 600px) {
    .header h1 { font-size: 1rem; }
    .header .settings-btn { padding: 6px 12px; font-size: 0.78rem; }
  }

  .layout {
    display: grid;
    grid-template-columns: 1fr 360px;
    grid-template-areas: "stream sidebar";
    gap: 12px;
    align-items: start;
  }

  .stream-wrap { grid-area: stream; }
  .sidebar     { grid-area: sidebar; }

  @media (max-width: 900px) {
    .layout {
      grid-template-columns: 1fr;
      grid-template-areas:
        "tabs"
        "stream"
        "panel";
    }
    .sidebar { display: contents; }
    .tabs    { grid-area: tabs; }
    .stream-wrap { grid-area: stream; }
    .panel   { grid-area: panel; }
  }

  .stream-wrap {
    background: #000; border-radius: 12px; overflow: hidden;
    border: 1px solid #30363d;
    aspect-ratio: 4/3;
    display: flex; align-items: center; justify-content: center;
  }
  .stream-wrap img {
    max-width: 100%; max-height: 100%;
    transition: transform 0.3s ease;
    transform-origin: center center;
  }

  .tabs {
    display: grid;
    grid-template-columns: repeat(4, 1fr);
    gap: 6px;
    background: #161b22;
    padding: 6px;
    border-radius: 12px;
    border: 1px solid #30363d;
    margin-bottom: 12px;
  }
  .tab {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 4px;
    padding: 10px 6px;
    background: transparent;
    color: #8b949e;
    border: none;
    border-radius: 8px;
    cursor: pointer;
    font-size: 0.72rem;
    font-weight: 600;
    font-family: inherit;
    letter-spacing: -0.01em;
    transition: all 0.15s;
    white-space: nowrap;
  }
  .tab svg {
    width: 44px;
    height: 44px;
    display: block;
    transition: filter 0.15s;
    filter: grayscale(1) brightness(0.6);
  }
  .tab:hover {
    background: rgba(255,255,255,0.05);
    color: #e6edf3;
  }
  .tab:hover svg {
    filter: grayscale(0.5) brightness(0.9);
  }
  .tab.active {
    background: rgba(63,185,80,0.15);
    color: #3fb950;
    box-shadow: inset 0 0 0 1px #3fb950;
  }
  .tab.active svg {
    filter: none;
  }
  @media (max-width: 600px) {
    .tab svg { width: 38px; height: 38px; }
    .tab { font-size: 0.66rem; padding: 8px 4px; }
  }

  .panel {
    background: #161b22; border: 1px solid #30363d;
    border-radius: 12px; padding: 18px;
    min-height: 200px;
  }

  .section { display: none; }
  .section.active { display: block; animation: fadeIn 0.2s ease; }

  @keyframes fadeIn {
    from { opacity: 0; transform: translateY(-4px); }
    to   { opacity: 1; transform: translateY(0); }
  }

  .section-title {
    font-size: 1rem;
    font-weight: 700;
    color: #e6edf3;
    margin-bottom: 16px;
    letter-spacing: -0.01em;
  }

  .angle-display {
    font-size: 2.25rem;
    font-weight: 800;
    text-align: center;
    color: #58a6ff;
    margin: 8px 0 16px;
    font-family: 'JetBrains Mono', monospace;
    letter-spacing: -0.03em;
  }
  .dpad {
    display: grid; grid-template-columns: repeat(3, 1fr);
    gap: 8px; max-width: 200px; margin: 0 auto 16px;
  }
  .dpad button {
    padding: 18px;
    background: #21262d;
    color: #e6edf3;
    border: 1px solid #30363d;
    border-radius: 10px;
    font-size: 1.25rem;
    font-weight: 700;
    cursor: pointer;
    transition: all 0.15s;
    font-family: inherit;
  }
  .dpad button:hover {
    background: #30363d;
    border-color: #3fb950;
    color: #3fb950;
  }
  .dpad .empty { visibility: hidden; }

  .buttons {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 8px;
  }
  .buttons.cols-4 { grid-template-columns: repeat(4, 1fr); }
  .buttons button {
    padding: 16px 12px;
    background: #21262d;
    color: #e6edf3;
    border: 1px solid #30363d;
    border-radius: 10px;
    cursor: pointer;
    font-size: 0.95rem;
    font-weight: 600;
    font-family: inherit;
    letter-spacing: -0.01em;
    transition: all 0.15s;
    white-space: nowrap;
  }
  .buttons button:hover {
    background: #30363d;
    border-color: #3fb950;
    color: #3fb950;
  }
  .buttons button.active {
    background: rgba(63,185,80,0.18);
    border-color: #3fb950;
    color: #3fb950;
    box-shadow: inset 0 0 0 1px #3fb950;
  }

  .footer {
    margin-top: 12px;
    padding: 10px 14px;
    background: #161b22;
    border: 1px solid #30363d;
    border-radius: 10px;
    display: flex;
    justify-content: center;
    align-items: center;
    font-size: 0.78rem;
    font-family: 'JetBrains Mono', monospace;
    color: #8b949e;
  }
  .footer .stats span { color: #58a6ff; font-weight: 600; }
  .footer .stats .sep { opacity: 0.4; margin: 0 8px; }
</style>
</head>
<body>
<div class="wrap">

  <div class="header">
    <h1>GreenLoop FPV</h1>
    <a href="/setup" class="settings-btn">⚙️ Настройки Wi-Fi</a>
  </div>

  <div class="layout">

    <div class="tabs">
      <button class="tab active" data-tab="servo">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
          <defs>
            <linearGradient id="t1-outer" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#2d9cf0"/>
              <stop offset="100%" stop-color="#1373d1"/>
            </linearGradient>
            <linearGradient id="t1-inner" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#3bb3ff"/>
              <stop offset="100%" stop-color="#1d85e0"/>
            </linearGradient>
          </defs>
          <circle cx="100" cy="100" r="80" fill="url(#t1-outer)"/>
          <circle cx="100" cy="100" r="68" fill="url(#t1-inner)"/>
          <g fill="#e0e0e0">
            <path d="M 100 42 L 120 64 L 110 64 L 110 90 L 90 90 L 90 64 L 80 64 Z"/>
            <path d="M 100 158 L 120 136 L 110 136 L 110 110 L 90 110 L 90 136 L 80 136 Z"/>
            <path d="M 42 100 L 64 80 L 64 90 L 90 90 L 90 110 L 64 110 L 64 120 Z"/>
            <path d="M 158 100 L 136 80 L 136 90 L 110 90 L 110 110 L 136 110 L 136 120 Z"/>
          </g>
        </svg>
        <span>Серво</span>
      </button>

      <button class="tab" data-tab="zoom">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
          <defs>
            <linearGradient id="t2-outer" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#2d9cf0"/>
              <stop offset="100%" stop-color="#1373d1"/>
            </linearGradient>
            <clipPath id="t2-clip">
              <circle cx="100" cy="100" r="68"/>
            </clipPath>
          </defs>
          <circle cx="100" cy="100" r="80" fill="url(#t2-outer)"/>
          <circle cx="100" cy="100" r="68" fill="#2d9cf0"/>
          <g clip-path="url(#t2-clip)">
            <path d="M 32 168 L 168 32 L 168 168 Z" fill="#1373d1"/>
            <path d="M 32 168 L 168 32 L 32 32 Z" fill="#3bb3ff"/>
          </g>
          <g fill="#e0e0e0">
            <g transform="translate(55, 51)">
              <rect x="16" y="0" width="16" height="48" rx="2"/>
              <rect x="0" y="16" width="48" height="16" rx="2"/>
            </g>
            <rect x="100" y="116" width="40" height="16" rx="2"/>
          </g>
        </svg>
        <span>Зум</span>
      </button>

      <button class="tab" data-tab="rotate">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
          <defs>
            <linearGradient id="t3-outer" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#2d9cf0"/>
              <stop offset="100%" stop-color="#1373d1"/>
            </linearGradient>
            <linearGradient id="t3-inner" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#3bb3ff"/>
              <stop offset="100%" stop-color="#1d85e0"/>
            </linearGradient>
          </defs>
          <circle cx="100" cy="100" r="80" fill="url(#t3-outer)"/>
          <circle cx="100" cy="100" r="68" fill="url(#t3-inner)"/>
          <g fill="#e0e0e0">
            <g>
              <path d="M 50 100 A 50 50 0 0 1 100 50 L 100 76 A 24 24 0 0 0 74 100 Z"/>
              <path d="M 100 42 L 152 65 L 100 88 Z"/>
            </g>
            <g>
              <path d="M 150 100 A 50 50 0 0 1 100 150 L 100 124 A 24 24 0 0 0 126 100 Z"/>
              <path d="M 100 112 L 48 135 L 100 158 Z"/>
            </g>
          </g>
        </svg>
        <span>Поворот</span>
      </button>

      <button class="tab" data-tab="flip">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
          <defs>
            <linearGradient id="t4-outer" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#2d9cf0"/>
              <stop offset="100%" stop-color="#1373d1"/>
            </linearGradient>
            <linearGradient id="t4-inner" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#3bb3ff"/>
              <stop offset="100%" stop-color="#1d85e0"/>
            </linearGradient>
            <clipPath id="t4-clip">
              <circle cx="100" cy="95" r="32"/>
            </clipPath>
          </defs>
          <circle cx="100" cy="100" r="80" fill="url(#t4-outer)"/>
          <circle cx="100" cy="100" r="68" fill="url(#t4-inner)"/>
          <g transform="translate(0, -10)">
            <ellipse cx="100" cy="158" rx="22" ry="5" fill="#e0e0e0"/>
            <rect x="78" y="150" width="44" height="8" rx="3" fill="#d0d0d0"/>
            <rect x="96" y="130" width="8" height="20" fill="#e0e0e0"/>
            <path d="M 55 105 Q 55 140 100 140 Q 145 140 145 105" fill="none" stroke="#e0e0e0" stroke-width="8" stroke-linecap="round"/>
            <rect x="53" y="80" width="8" height="28" rx="4" fill="#e0e0e0"/>
            <rect x="139" y="80" width="8" height="28" rx="4" fill="#e0e0e0"/>
            <rect x="62" y="88" width="10" height="4" fill="#d0d0d0"/>
            <rect x="128" y="88" width="10" height="4" fill="#d0d0d0"/>
            <circle cx="100" cy="95" r="38" fill="#e0e0e0"/>
            <circle cx="100" cy="95" r="34" fill="#d0d0d0"/>
            <circle cx="100" cy="95" r="32" fill="#ffffff"/>
            <g clip-path="url(#t4-clip)">
              <path d="M 60 60 L 130 60 L 90 140 L 20 140 Z" fill="#a8e6f0" opacity="0.5"/>
            </g>
          </g>
        </svg>
        <span>Зеркало</span>
      </button>
    </div>

    <div class="stream-wrap">
      <img id="stream-img" src="" alt="FPV поток">
    </div>

    <div class="sidebar">
      <div class="panel">

        <div class="section active" data-section="servo">
          <div class="section-title">Поворот башни</div>
          <div class="angle-display" id="angle">90°</div>
          <div class="dpad">
            <div class="empty"></div>
            <button onclick="setAngle(currentAngle + 5)">▲</button>
            <div class="empty"></div>
            <button onclick="setAngle(currentAngle - 5)">◀</button>
            <button onclick="setAngle(90)">●</button>
            <button onclick="setAngle(currentAngle + 5)">▶</button>
            <div class="empty"></div>
            <button onclick="setAngle(currentAngle - 5)">▼</button>
            <div class="empty"></div>
          </div>
          <div class="buttons cols-4">
            <button onclick="setAngle(0)">0°</button>
            <button onclick="setAngle(45)">45°</button>
            <button onclick="setAngle(90)">90°</button>
            <button onclick="setAngle(180)">180°</button>
          </div>
        </div>

        <div class="section" data-section="zoom">
          <div class="section-title">Цифровой зум</div>
          <div class="buttons cols-4">
            <button onclick="setCssZoom(1)"   id="zoom-1">1x</button>
            <button onclick="setCssZoom(1.5)" id="zoom-15">1.5x</button>
            <button onclick="setCssZoom(2)"   id="zoom-2">2x</button>
            <button onclick="setCssZoom(3)"   id="zoom-3">3x</button>
          </div>
        </div>

        <div class="section" data-section="rotate">
          <div class="section-title">Поворот изображения</div>
          <div class="buttons cols-4">
            <button onclick="setRotation(0)"   id="rot-0">0°</button>
            <button onclick="setRotation(90)"  id="rot-90">90°</button>
            <button onclick="setRotation(180)" id="rot-180">180°</button>
            <button onclick="setRotation(270)" id="rot-270">270°</button>
          </div>
        </div>

        <div class="section" data-section="flip">
          <div class="section-title">Зеркало</div>
          <div class="buttons">
            <button onclick="toggleFlip('h')" id="flip-h">↔️ По горизонтали</button>
            <button onclick="toggleFlip('v')" id="flip-v">↕️ По вертикали</button>
          </div>
          <div style="margin-top:8px;">
            <div class="buttons">
              <button onclick="resetAll()" style="grid-column: span 2;">⟲ Сбросить всё</button>
            </div>
          </div>
        </div>

      </div>
    </div>

  </div>

  <div class="footer">
    <div class="stats">
      FPS: <span id="fps">—</span>
      <span class="sep">·</span>
      RSSI: <span id="rssi">—</span> dBm
      <span class="sep">·</span>
      IP: <span id="ip">—</span>
    </div>
  </div>

</div>

<script>
var currentAngle = 90;

function setAngle(a) {
  if (a < 0) a = 0;
  if (a > 180) a = 180;
  currentAngle = a;
  document.getElementById('angle').textContent = a + '°';
  fetch('/servo?angle=' + a).catch(function(e) {});
}

var rotDeg = 0, flipH = false, flipV = false, cssZoom = 1;

function applyTransform() {
  var img = document.getElementById('stream-img');
  var t = 'rotate(' + rotDeg + 'deg) scale(' + cssZoom + ')';
  if (rotDeg === 90 || rotDeg === 270) t += ' scale(0.75)';
  if (flipH) t += ' scaleX(-1)';
  if (flipV) t += ' scaleY(-1)';
  img.style.transform = t;

  document.querySelectorAll('[id^="rot-"]').forEach(function(b) {
    b.classList.toggle('active', b.id === 'rot-' + rotDeg);
  });
  var fh = document.getElementById('flip-h');
  var fv = document.getElementById('flip-v');
  if (fh) fh.classList.toggle('active', flipH);
  if (fv) fv.classList.toggle('active', flipV);
}

function setCssZoom(s) {
  cssZoom = s;
  var pairs = [['zoom-1', 1], ['zoom-15', 1.5], ['zoom-2', 2], ['zoom-3', 3]];
  pairs.forEach(function(pair) {
    var b = document.getElementById(pair[0]);
    if (b) b.classList.toggle('active', pair[1] === s);
  });
  applyTransform();
}

function setRotation(d) { rotDeg = d; applyTransform(); }

function toggleFlip(ax) {
  if (ax === 'h') flipH = !flipH;
  if (ax === 'v') flipV = !flipV;
  applyTransform();
}

function resetAll() {
  rotDeg = 0; flipH = false; flipV = false;
  setCssZoom(1); applyTransform();
}

document.querySelectorAll('.tab').forEach(function(btn) {
  btn.addEventListener('click', function() {
    var name = this.dataset.tab;
    document.querySelectorAll('.tab').forEach(function(t) { t.classList.remove('active'); });
    this.classList.add('active');
    document.querySelectorAll('.section').forEach(function(s) { s.classList.remove('active'); });
    var section = document.querySelector('[data-section="' + name + '"]');
    if (section) section.classList.add('active');
  });
});

function updateStatus() {
  fetch('/status')
    .then(function(r) { return r.json(); })
    .then(function(j) {
      document.getElementById('fps').textContent = j.fps;
      document.getElementById('rssi').textContent = j.rssi;
      document.getElementById('ip').textContent = j.ip;
    })
    .catch(function(e) {});
}
setInterval(updateStatus, 2000);
updateStatus();

document.getElementById('stream-img').src =
  'http://' + location.hostname + ':81/stream';

setCssZoom(1);
</script>
</body>
</html>
)HTML";