// app.js.h — JavaScript
#pragma once

const char APP_JS[] PROGMEM = R"JS(
// ============ Снимок ============
document.getElementById("snapBtn").addEventListener("click", function() {
  window.open("/capture?t=" + Date.now(), "_blank");
});

// ============ Статус ============
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

        // Синхронизируем угол, если изменился извне
        if (typeof j.angle === "number" && j.angle !== currentAngle) {
          currentAngle = j.angle;
          updateAngleDisplay();
        }
      } catch(e) {}
    }
  };
  try { xhr.send(); } catch(e) {}
}

// ============ Серво — джойстик ============
var currentAngle = 90;
var holdTimer    = null;
var holdInterval = null;
var lastSent     = -1;
var sendTimer    = null;

function updateAngleDisplay() {
  document.getElementById("angleBig").textContent = currentAngle + "°";
  var center = document.querySelector(".joy-center");
  if (center) center.textContent = currentAngle + "°";
}

function clampAngle(a) {
  if (a < 0)   return 0;
  if (a > 180) return 180;
  return a;
}

function sendAngle() {
  if (currentAngle === lastSent) return;

  if (sendTimer) return;
  sendTimer = setTimeout(function() {
    sendTimer = null;
    lastSent = currentAngle;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/servo?angle=" + currentAngle, true);
    xhr.timeout = 1000;
    try { xhr.send(); } catch(e) {}
  }, 50);
}

function changeAngle(delta) {
  currentAngle = clampAngle(currentAngle + delta);
  updateAngleDisplay();
  sendAngle();
}

// --- Одиночное нажатие + удержание ---
function startHold(delta) {
  changeAngle(delta);

  // Через 300 мс — авто-повтор каждые 50 мс
  holdTimer = setTimeout(function() {
    holdInterval = setInterval(function() {
      changeAngle(delta);
    }, 50);
  }, 300);
}

function stopHold() {
  if (holdTimer)    clearTimeout(holdTimer);
  if (holdInterval) clearInterval(holdInterval);
  holdTimer = null;
  holdInterval = null;
}

document.querySelectorAll(".joy-btn[data-dir]").forEach(function(btn) {
  var dir = parseInt(btn.getAttribute("data-dir"), 10);

  btn.addEventListener("mousedown", function(e) {
    e.preventDefault();
    startHold(dir);
  });

  btn.addEventListener("touchstart", function(e) {
    e.preventDefault();
    startHold(dir);
  }, { passive: false });

  btn.addEventListener("mouseup",  stopHold);
  btn.addEventListener("mouseleave", stopHold);
  btn.addEventListener("touchend",   stopHold);
  btn.addEventListener("touchcancel", stopHold);
});

// --- Пресеты ---
document.querySelectorAll(".joy-preset").forEach(function(btn) {
  btn.addEventListener("click", function() {
    var a = parseInt(btn.getAttribute("data-angle"), 10);
    if (isNaN(a)) return;
    currentAngle = clampAngle(a);
    updateAngleDisplay();
    sendAngle();
  });
});

// ============ Запуск ============
updateAngleDisplay();
updateStatus();
setInterval(updateStatus, 1500);
)JS";
