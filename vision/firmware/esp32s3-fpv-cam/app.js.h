// app.js.h — JavaScript для страницы камеры
#pragma once

const char APP_JS[] PROGMEM = R"JS(
document.getElementById("snapBtn").addEventListener("click", function() {
  window.open("/capture?t=" + Date.now(), "_blank");
});

function updateStatus() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/status?t=" + Date.now(), true);
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
  xhr.send();
}

updateStatus();
setInterval(updateStatus, 1000);
)JS";
