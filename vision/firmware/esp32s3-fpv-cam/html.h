// html.h — HTML + CSS
#pragma once

const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32-S3 FPV</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body { background: rgb(17,17,17); color: rgb(238,238,238);
         font-family: -apple-system, sans-serif; min-height: 100vh;
         display: flex; flex-direction: column; align-items: center;
         padding: 12px; gap: 12px; }
  header { width: 100%; max-width: 720px; display: flex;
           justify-content: space-between; align-items: center;
           background: rgb(28,28,28); padding: 10px 14px;
           border-radius: 10px; font-size: 14px; }
  .video-wrap { width: 100%; max-width: 720px; background: black;
                border-radius: 12px; overflow: hidden; aspect-ratio: 4/3;
                display: flex; align-items: center; justify-content: center; }
  .video-wrap img { width: 100%; height: 100%; object-fit: contain; }
  .controls { width: 100%; max-width: 720px; display: flex; gap: 10px;
              background: rgb(28,28,28); padding: 12px; border-radius: 10px;
              align-items: center; flex-wrap: wrap; }
  .btn { padding: 10px 20px; background: rgb(42,42,42);
         border: 1px solid rgb(68,68,68); color: rgb(238,238,238);
         border-radius: 8px; cursor: pointer; font-size: 14px;
         font-family: inherit; }
  .btn:hover { background: rgb(58,58,58); }
  .status { margin-left: auto; display: flex; gap: 16px;
            font-size: 13px; color: rgb(170,170,170); }
  .status .val { color: rgb(68,170,255); font-weight: 600; }

  /* Джойстик серво */
  .servo-panel { width: 100%; max-width: 720px;
                 background: rgb(28,28,28); padding: 20px;
                 border-radius: 12px;
                 display: flex; flex-direction: column; align-items: center;
                 gap: 14px; }
  .servo-title { font-size: 14px; color: rgb(68,170,255);
                 font-weight: 600; }
  .servo-angle-big { font-size: 32px; font-weight: 800;
                     font-family: monospace; color: rgb(68,170,255); }
  .joystick { display: grid;
              grid-template-columns: 70px 70px 70px;
              grid-template-rows: 70px 70px 70px;
              gap: 8px; }
  .joy-btn { width: 70px; height: 70px;
             background: rgb(42,42,42);
             border: 2px solid rgb(68,68,68);
             color: rgb(238,238,238);
             border-radius: 12px; font-size: 26px; font-weight: 700;
             cursor: pointer;
             display: flex; align-items: center; justify-content: center;
             transition: background 0.1s, border-color 0.1s, transform 0.05s;
             user-select: none; -webkit-user-select: none;
             font-family: inherit; }
  .joy-btn:hover { background: rgb(58,58,58); border-color: rgb(68,170,255); }
  .joy-btn:active { background: rgb(68,170,255); color: rgb(0,0,0);
                    transform: scale(0.94); }
  .joy-up    { grid-column: 2; grid-row: 1; }
  .joy-left  { grid-column: 1; grid-row: 2; }
  .joy-right { grid-column: 3; grid-row: 2; }
  .joy-down  { grid-column: 2; grid-row: 3; }
  .joy-center { grid-column: 2; grid-row: 2;
                background: rgb(20,20,20);
                border-color: rgb(50,50,50);
                font-size: 14px; color: rgb(120,120,120);
                cursor: default; }
  .joy-center:hover { background: rgb(20,20,20);
                      border-color: rgb(50,50,50); }
  .joy-presets { display: flex; gap: 8px; margin-top: 6px; }
  .joy-preset { padding: 8px 16px; background: rgb(42,42,42);
                border: 1px solid rgb(68,68,68); color: rgb(238,238,238);
                border-radius: 8px; cursor: pointer; font-size: 13px;
                font-family: inherit; }
  .joy-preset:hover { background: rgb(58,58,58);
                      border-color: rgb(68,170,255); }
</style>
</head>
<body>

<header>
  <span>📷 ESP32-S3 FPV</span>
  <span id="rssi">— dBm</span>
</header>

<div class="video-wrap">
  <img id="stream" src="/stream" alt="FPV">
</div>

<div class="controls">
  <button class="btn" id="snapBtn">📸 Снимок</button>
  <div class="status">
    <span>FPS: <span class="val" id="fps">—</span></span>
    <span>Heap: <span class="val" id="heap">—</span></span>
  </div>
</div>

<!-- Джойстик серво -->
<div class="servo-panel">
  <div class="servo-title">🎛️ Управление камерой</div>
  <div class="servo-angle-big" id="angleBig">90°</div>

  <div class="joystick">
    <button class="joy-btn joy-up"    data-dir="+1">▲</button>
    <button class="joy-btn joy-left"  data-dir="-1">◀</button>
    <div class="joy-btn joy-center">90°</div>
    <button class="joy-btn joy-right" data-dir="+1">▶</button>
    <button class="joy-btn joy-down"  data-dir="-1">▼</button>
  </div>

  <div class="joy-presets">
    <button class="joy-preset" data-angle="0">0°</button>
    <button class="joy-preset" data-angle="45">45°</button>
    <button class="joy-preset" data-angle="90">90°</button>
    <button class="joy-preset" data-angle="135">135°</button>
    <button class="joy-preset" data-angle="180">180°</button>
  </div>
</div>

<script src="/app.js"></script>

</body>
</html>
)HTML";
