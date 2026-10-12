// ============================================================
//  Работа с FPV камерой ESP32-S3
// ============================================================

const user = checkAccess();
const isAdmin = user && user.role === 'admin';

const roleEl = document.getElementById('user-role');
if (roleEl) roleEl.textContent = isAdmin ? '👑 Админ' : '👤 Гость';

// Показать управление серво для админов
if (isAdmin) {
  document.getElementById('servo-controls').style.display = 'block';
}

// ---- Элементы ----
const streamEl = document.getElementById('stream');
const overlay  = document.getElementById('status-overlay');

// ---- Подключение потока ----
function connectStream() {
  overlay.style.display = 'flex';
  overlay.textContent = '⏳ Подключение к ' + CONFIG.FPV_STREAM_URL + '...';

  const url = CONFIG.FPV_STREAM_URL + '?t=' + Date.now();

  const test = new Image();
  test.onload = () => {
    streamEl.src = url;
    overlay.style.display = 'none';
  };
  test.onerror = () => {
    overlay.textContent =
      '❌ Не удалось подключиться к камере.\n\n' +
      'Проверь:\n' +
      '1. Ты подключён к Wi-Fi «ESP32-FPV» (пароль: 12345678)\n' +
      '2. Адрес верный: ' + CONFIG.FPV_STREAM_URL + '\n' +
      '3. Плата ESP32 включена и работает';
  };
  test.src = url;
}

// ---- Отключение ----
function disconnectStream() {
  streamEl.src = '';
  overlay.style.display = 'flex';
  overlay.textContent = '⏹️ Отключено';
}

// ---- Снимок ----
async function takeSnapshot() {
  try {
    const res = await fetch(CONFIG.FPV_SNAPSHOT_URL + '?t=' + Date.now());
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
}

// ---- FPS / статус ----
setInterval(async () => {
  try {
    const res = await fetch(CONFIG.FPV_STATUS_URL + '?t=' + Date.now());
    const j = await res.json();
    const el = document.getElementById('fps-info');
    if (el) el.textContent = 'FPS: ' + j.fps;
  } catch (e) {
    const el = document.getElementById('fps-info');
    if (el) el.textContent = 'FPS: —';
  }
}, 2000);

// ---- Управление серво (только админ) ----
if (isAdmin) {
  const pan   = document.getElementById('pan');
  const tilt  = document.getElementById('tilt');
  const panV  = document.getElementById('pan-val');
  const tiltV = document.getElementById('tilt-val');

  let timer = null;

  function sendServo() {
    panV.textContent  = pan.value + '°';
    tiltV.textContent = tilt.value + '°';

    if (timer) return;
    timer = setTimeout(() => {
      timer = null;
      const base = CONFIG.FPV_STREAM_URL.replace('/stream', '');
      fetch(`${base}/servo?pan=${pan.value}&tilt=${tilt.value}`).catch(()=>{});
    }, 80);
  }

  pan?.addEventListener('input', sendServo);
  tilt?.addEventListener('input', sendServo);
}

// ---- Кнопки ----
document.getElementById('connect')?.addEventListener('click', connectStream);
document.getElementById('disconnect')?.addEventListener('click', disconnectStream);
document.getElementById('snapshot')?.addEventListener('click', takeSnapshot);

// ---- Автозапуск ----
connectStream();
