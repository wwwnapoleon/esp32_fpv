// app.js.h — JavaScript
#pragma once

const char APP_JS[] PROGMEM = R"JS(
// ============ Снимок (порт 80) ============
document.getElementById("snapBtn").addEventListener("click", function() {
  window.open("/capture?t=" + Date.now(), "_blank");
});

// ============ Обновление статуса (порт 81) ============
var STATUS_URL = "http://" + location.hostname + ":81/status";

function updateStatus() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", STATUS_URL + "?t=" + Date.now(), true);
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

// Первый запрос и повтор каждую секунду
updateStatus();
setInterval(updateStatus, 1000);
)JS";
