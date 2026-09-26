// ============================================================
//  Работа с FPV камерой ESP32-S3
// ============================================================

const user = checkAccess();
const isAdmin = user && user.role === 'admin';

document.getElementById('user-role').textContent =
  isAdmin ? '👑 Админ' : '👤 Гость';

if (isAdmin) {
  document.getElementById('servo-controls').style.display = 'block';
}

// ---- Поток ----
const streamEl = document.getElementById('stream');
const overlay  = document.getElementById('status-overlay');

function connectStream() {
  overlay.textContent = 'Подключение к ' + CONFIG.FPV_STREAM_URL + '...';
  overlay.style.display = 'block';

  streamEl.onload = () => {
    overlay.style.display = 'none';
  };
  streamEl.onerror = () => {
    overlay.textContent =
      '❌ Не удалось подключиться к камере.\n' +
      'Проверь, что ты в Wi-Fi сети ESP32-FPV и адрес верный: ' +
      CONFIG.FPV_STREAM_URL;
  };
  streamEl.src = CONFIG.FPV_STREAM_URL + '?t=' + Date.now();
}

document.getElementById('connect').addEventListener('click', connectStream);

document.getElementById('snapshot').addEventListener('click', async () => {
  try {
    const res = await fetch(CONFIG.FPV_SNAPSHOT_URL);
    const blob = await res.blob();
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'fpv-snapshot-' + Date.now() + '.jpg';
    a.click();
    URL.revokeObjectURL(url);
  } catch (e) {
    alert('Не удалось сделать снимок: ' + e.message);
  }
});

// ---- FPS / статус ----
setInterval(async () => {
  try {
    const res = await fetch(CONFIG.FPV_STATUS_URL);
    const j = await res.json();
    document.getElementById('fps-info').textContent = 'FPS: ' + j.fps;
  } catch (e) {
    document.getElementById('fps-info').textContent = 'FPS: —';
  }
}, 2000);

// ---- Управление серво (только админ) ----
if (isAdmin) {
  const pan  = document.getElementById('pan');
  const tilt = document.getElementById('tilt');
  const panVal  = document.getElementById('pan-val');
  const tiltVal = document.getElementById('tilt-val');

  let timer = null;

  function sendServo() {
    panVal.textContent  = pan.value + '°';
    tiltVal.textContent = tilt.value + '°';

    if (timer) return;
    timer = setTimeout(() => {
      timer = null;
      const base = CONFIG.FPV_STREAM_URL.replace('/stream', '');
      fetch(`${base}/servo?pan=${pan.value}&tilt=${tilt.value}`).catch(()=>{});
    }, 80);
  }

  pan.addEventListener('input', sendServo);
  tilt.addEventListener('input', sendServo);
}

// Автозапуск
connectStream();
