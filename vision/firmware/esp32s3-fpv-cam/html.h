// html.h — HTML + CSS для страницы камеры
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
              align-items: center; }
  .btn { padding: 10px 20px; background: rgb(42,42,42);
         border: 1px solid rgb(68,68,68); color: rgb(238,238,238);
         border-radius: 8px; cursor: pointer; font-size: 14px; }
  .status { margin-left: auto; display: flex; gap: 16px;
            font-size: 13px; color: rgb(170,170,170); }
  .status .val { color: rgb(68,170,255); font-weight: 600; }
</style>
</head>
<body>

<header>
  <span>📷 ESP32-S3 FPV</span>
  <span id="rssi">— dBm</span>
</header>

<div class="video-wrap">
  <img id="stream" src="http://192.168.100.7:81/stream" alt="FPV">
</div>

<div class="controls">
  <button class="btn" id="snapBtn">📸 Снимок</button>
  <div class="status">
    <span>FPS: <span class="val" id="fps">—</span></span>
    <span>Heap: <span class="val" id="heap">—</span></span>
  </div>
</div>

<script src="/app.js"></script>

</body>
</html>
)HTML";
