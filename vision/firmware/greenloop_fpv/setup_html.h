#pragma once

const char SETUP_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>GreenLoop · Настройка Wi-Fi</title>
<style>
  * { box-sizing: border-box; }
  body { background: #0d1117; color: #e6edf3; font-family: -apple-system, sans-serif;
         display: flex; justify-content: center; align-items: center;
         min-height: 100vh; margin: 0; padding: 20px; }
  .card { background: #161b22; border: 1px solid #30363d; border-radius: 16px;
          padding: 32px; max-width: 440px; width: 100%; }
  h1 { margin: 0 0 8px; color: #3fb950; }
  .sub { opacity: 0.7; margin: 0 0 24px; font-size: 0.9rem; }
  label { display: block; margin: 16px 0 6px; font-weight: 600; font-size: 0.9rem; }
  select, input { width: 100%; padding: 12px 14px; background: #0d1117;
                  border: 1px solid #30363d; border-radius: 8px;
                  color: #e6edf3; font-size: 1rem; font-family: inherit; }
  select:focus, input:focus { outline: none; border-color: #3fb950; }
  button { width: 100%; margin-top: 16px; padding: 14px; background: #3fb950;
           color: #fff; border: none; border-radius: 8px; font-size: 1rem;
           font-weight: 700; cursor: pointer; }
  button:hover { background: #2ea043; }
  .refresh { background: transparent; border: 1px solid #30363d; color: #e6edf3;
             font-size: 0.85rem; padding: 8px; margin-top: 8px; }
  .status { margin-top: 16px; padding: 12px; border-radius: 8px;
            font-size: 0.9rem; text-align: center; display: none; }
  .status.ok { background: rgba(63,185,80,0.15); color: #3fb950; display: block; }
  .status.err { background: rgba(248,81,73,0.15); color: #f85149; display: block; }
  .status.wait { background: rgba(88,166,255,0.15); color: #58a6ff; display: block; }
  .back { display: inline-block; margin-top: 20px; color: #58a6ff;
          text-decoration: none; font-size: 0.9rem; }
  .info { margin-top: 20px; padding: 12px; background: rgba(88,166,255,0.08);
          border: 1px solid rgba(88,166,255,0.2); border-radius: 8px;
          font-size: 0.85rem; line-height: 1.5; }
</style>
</head>
<body>
<div class="card">
  <h1>🌿 GreenLoop</h1>
  <p class="sub">Ручное подключение к другой сети</p>

  <form id="wifiForm">
    <label>Wi-Fi сеть</label>
    <select id="ssid" name="ssid" required>
      <option value="">— сканирование... —</option>
    </select>
    <button type="button" class="refresh" onclick="scanWifi()">🔄 Обновить список</button>

    <label>Пароль</label>
    <input type="password" id="pass" name="pass" placeholder="••••••••">

    <button type="submit">📡 Подключиться</button>
  </form>

  <div id="status" class="status"></div>
  <a href="/" class="back">← Вернуться к камере</a>

  <div class="info">
    <strong>ℹ️ Что это:</strong><br>
    ESP32 автоматически подключается к <code>RGB_Route</code>.<br>
    Здесь можно подключиться к <strong>другой</strong> сети.
  </div>
</div>

<script>
function scanWifi() {
  var sel = document.getElementById('ssid');
  sel.innerHTML = '<option value="">— сканирование... —</option>';
  fetch('/scan')
    .then(function(r) { return r.json(); })
    .then(function(nets) {
      sel.innerHTML = '';
      if (nets.length === 0) {
        sel.innerHTML = '<option value="">— сетей не найдено —</option>';
        return;
      }
      nets.forEach(function(n) {
        var opt = document.createElement('option');
        opt.value = n.ssid;
        opt.textContent = n.ssid + '  (' + n.rssi + ' dBm)';
        sel.appendChild(opt);
      });
    })
    .catch(function(e) {
      sel.innerHTML = '<option value="">— ошибка —</option>';
    });
}

document.getElementById('wifiForm').addEventListener('submit', function(e) {
  e.preventDefault();
  var ssid = document.getElementById('ssid').value;
  var pass = document.getElementById('pass').value;
  var st = document.getElementById('status');

  if (!ssid) { st.className='status err'; st.textContent='❌ Выберите сеть'; return; }

  st.className = 'status wait';
  st.textContent = '⏳ Подключение...';

  fetch('/connect', {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: 'ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass)
  })
    .then(function(res) {
      if (res.ok) {
        st.className = 'status ok';
        st.textContent = '✅ Подключено! Переход...';
        return res.text();
      } else {
        return res.json().then(function(err) {
          throw new Error(err.error || 'Ошибка подключения');
        });
      }
    })
    .then(function(html) {
      document.open();
      document.write(html);
      document.close();
    })
    .catch(function(err) {
      st.className = 'status err';
      st.textContent = '❌ ' + err.message;
    });
});

scanWifi();
</script>
</body>
</html>
)HTML";