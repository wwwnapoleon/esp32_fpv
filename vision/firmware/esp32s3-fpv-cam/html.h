// html.h — HTML + CSS для страницы камеры с серво
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

  /* Серво-панель */
  .servo-panel { width: 100%; max-width: 720px;
                 background: rgb(28,28,28); padding: 16px;
                 border-radius: 12px; }
  .servo-title { font-size: 14px; color: rgb(68,170,255);
                 font-weight: 600; margin-bottom: 12px; }
  .servo-row { display: flex; align-items: center; gap: 14px;
               margin-bottom: 12px; }
  .servo-row label { font-size: 13px; color: rgb(170,170,170);
                     width: 50px; }
  .servo-row input[type=range] { flex: 1; accent-color: rgb(68,170,255);
                                  height: 6px; cursor: pointer; }
  .servo-angle { min-width: 50px; text-align: right;
                 font-family: monospace; font-size: 16px;
                 font-weight: 700; color: rgb(68,170,255); }
  .servo-presets { display: grid; grid-template-columns: repeat(5, 1fr);
                   gap: 8px; }
  .servo-preset { padding: 10px 0; background: rgb(42,42,42);
                  border: 1px solid rgb(68,68,68);
                  color: rgb(238,238,238); border-radius: 8px;
                  cursor: pointer; font-size: 13px; font-weight: 600;
                  font-family: inherit; }
  .servo-preset:hover { background: rgb(58,58,58);
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

<!-- Управление серво -->
<div class="servo-panel">
  <div class="servo-title">🎛️ Управление камерой</div>

  <div class="servo-row">
    <label>Угол</label>
    <input type="range" id="servoAngle" min="0" max="180" value="90" step="1">
    <span class="servo-angle" id="servoAngleVal">90°</span>
  </div>

  <div class="servo-presets">
    <button class="servo-preset" data-angle="0">0°</button>
    <button class="servo-preset" data-angle="45">45°</button>
    <button class="servo-preset" data-angle="90">90°</button>
    <button class="servo-preset" data-angle="135">135°</button>
    <button class="servo-preset" data-angle="180">180°</button>
  </div>
</div>

<script src="/app.js"></script>

</body>
</html>
)HTML";
