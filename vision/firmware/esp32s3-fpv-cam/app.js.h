// app.js.h — JavaScript
#pragma once

const char APP_JS[] PROGMEM = R"JS(
// ============================================================
//  ГЛОБАЛЬНЫЕ
// ============================================================
var currentAngle = 90;
var lastSent     = -1;
var holdTimer    = null;
var holdInterval = null;
var sendTimer    = null;

// ============================================================
//  СНИМОК
// ============================================================
document.getElementById("snapBtn").addEventListener("click", function() {
  window.open("/capture?t=" + Date.now(), "_blank");
});

// ============================================================
//  СЕРВО — ДЖОЙСТИК
// ============================================================
function updateAngleDisplay() {
  var big = document.getElementById("angleBig");
  if (big) big.textContent = currentAngle + "\u00B0";

  var center = document.querySelector(".joy-center");
  if (center) center.textContent = currentAngle + "\u00B0";
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

function startHold(delta) {
  changeAngle(delta);

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

// --- Кнопки джойстика ---
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

  btn.addEventListener("mouseup",     stopHold);
  btn.addEventListener("mouseleave",  stopHold);
  btn.addEventListener("touchend",    stopHold);
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

// ============================================================
//  СТАТУС (FPS / RSSI / Heap) — порт 81
// ============================================================
function updateStatus() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "http://" + location.hostname + ":81/status?t=" + Date.now(), true);
  xhr.timeout = 2000;
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      try {
        var j = JSON.parse(xhr.responseText);
        var rssiEl = document.getElementById("rssi");
        var fpsEl  = document.getElementById("fps");
        var heapEl = document.getElementById("heap");

        if (rssiEl) rssiEl.textContent = j.rssi + " dBm";
        if (fpsEl)  fpsEl.textContent  = j.fps;
        if (heapEl) heapEl.textContent = Math.round(j.heap / 1024) + " KB";

        if (typeof j.angle === "number" && j.angle !== currentAngle) {
          currentAngle = j.angle;
          updateAngleDisplay();
        }
      } catch(e) {}
    }
  };
  try { xhr.send(); } catch(e) {}
}

// ============================================================
//  ЗАПУСК
// ============================================================
updateAngleDisplay();
updateStatus();
setInterval(updateStatus, 1500);
)JS";
