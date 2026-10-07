// app.js.h — JavaScript для страницы камеры
#pragma once

const char APP_JS[] PROGMEM = R"JS(
// ============ Снимок ============
document.getElementById("snapBtn").addEventListener("click", function() {
  window.open("/capture?t=" + Date.now(), "_blank");
});

// ============ Статус (FPS / RSSI / Heap) ============
function updateStatus() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/status?t=" + Date.now(), true);
  xhr.timeout = 2000;
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      try {
        var j = JSON.parse(xhr.responseText);
        document.getElementById("rssi").textContent = j.rssi + " dBm";
        document.getElementById("fps").textContent = j.fps;
        document.getElementById("heap").textContent = Math.round(j.heap / 1024) + " KB";
      } catch(e) {}
    }
  };
  try { xhr.send(); } catch(e) {}
}

// ============ Серво ============
var servoSlider = document.getElementById("servoAngle");
var servoVal    = document.getElementById("servoAngleVal");
var servoTimer  = null;

function sendServoAngle(angle) {
  var a = parseInt(angle, 10);
  if (isNaN(a)) return;
  if (a < 0)   a = 0;
  if (a > 180) a = 180;

  servoVal.textContent = a + "°";

  // Троттлинг — не чаще 60 мс
  if (servoTimer) return;
  servoTimer = setTimeout(function() {
    servoTimer = null;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/servo?angle=" + a, true);
    xhr.timeout = 1000;
    try { xhr.send(); } catch(e) {}
  }, 60);
}

servoSlider.addEventListener("input", function(e) {
  sendServoAngle(e.target.value);
});

document.querySelectorAll(".servo-preset").forEach(function(btn) {
  btn.addEventListener("click", function() {
    var angle = btn.getAttribute("data-angle");
    servoSlider.value = angle;
    sendServoAngle(angle);
  });
});

// ============ Запуск ============
updateStatus();
setInterval(updateStatus, 1500);
)JS";
