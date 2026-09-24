// ============================================================
//  ESP32-S3 FPV Camera + 2x MG90S Servo
//  Просмотр со смартфона через браузер (MJPEG + Web UI)
// ============================================================

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"
#include <ESP32Servo.h>

// ================== НАСТРОЙКИ WI-FI ==================
// Режим точки доступа (по умолчанию):
const char* AP_SSID = "ESP32-FPV";
const char* AP_PASS = "12345678";

// Если хочешь подключиться к домашнему Wi-Fi — раскомментируй:
// #define USE_STA 1
const char* STA_SSID = "YOUR_WIFI_NAME";
const char* STA_PASS = "YOUR_WIFI_PASSWORD";

// ================== ПИНЫ КАМЕРЫ (ESP32-S3 + OV2640) ==================
#define PWDN_GPIO_NUM   -1
#define RESET_GPIO_NUM  -1
#define XCLK_GPIO_NUM   15
#define SIOD_GPIO_NUM   4
#define SIOC_GPIO_NUM   5
#define Y9_GPIO_NUM     16
#define Y8_GPIO_NUM     17
#define Y7_GPIO_NUM     18
#define Y6_GPIO_NUM     12
#define Y5_GPIO_NUM     10
#define Y4_GPIO_NUM     8
#define Y3_GPIO_NUM     9
#define Y2_GPIO_NUM     11
#define VSYNC_GPIO_NUM  6
#define HREF_GPIO_NUM   7
#define PCLK_GPIO_NUM   13

// ================== ПИНЫ СЕРВО ==================
#define SERVO_PAN_PIN   38
#define SERVO_TILT_PIN  39

Servo servoPan;
Servo servoTilt;
int panAngle  = 90;
int tiltAngle = 90;
int targetPan  = 90;
int targetTilt = 90;

// ================== HTML UI ==================
const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>ESP32-S3 FPV</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    background:#111; color:#eee;
    font-family:-apple-system,Roboto,sans-serif;
    display:flex; flex-direction:column; height:100vh; overflow:hidden;
  }
  header {
    padding:8px 12px; background:#1c1c1c;
    display:flex; justify-content:space-between;
    align-items:center; font-size:14px;
  }
  #stream { flex:1; width:100%; object-fit:contain; background:#000; }
  .controls {
    padding:12px; background:#1c1c1c;
    display:flex; flex-direction:column; gap:10px;
  }
  .row { display:flex; align-items:center; gap:10px; }
  .row label { width:44px; font-size:13px; color:#9cf; }
  input[type=range] { flex:1; accent-color:#4af; height:28px; }
  .btns { display:grid; grid-template-columns:repeat(5,1fr); gap:6px; }
  .btns button {
    padding:10px 0; background:#2a2a2a; border:1px solid #444;
    color:#eee; border-radius:8px; font-size:14px;
  }
  .btns button:active { background:#4af; }
  .status { font-size:11px; color:#888; text-align:center; }
</style>
</head>
<body>
<header>
  <span>📷 ESP32-S3 FPV</span>
  <span id="rssi">…</span>
</header>

<img id="stream" src="/stream">

<div class="controls">
  <div class="row">
    <label>PAN</label>
    <input id="pan" type="range" min="0" max="180" value="90">
  </div>
  <div class="row">
    <label>TILT</label>
    <input id="tilt" type="range" min="0" max="180" value="90">
  </div>
  <div class="btns">
    <button onclick="preset(0,90)">◀</button>
    <button onclick="preset(90,0)">▲</button>
    <button onclick="preset(90,90)">●</button>
    <button onclick="preset(90,180)">▼</button>
    <button onclick="preset(180,90)">▶</button>
  </div>
  <div class="status" id="fps">FPS: —</div>
</div>

<script>
const pan  = document.getElementById('pan');
const tilt = document.getElementById('tilt');
let sendTimer = null;

function send() {
  fetch(`/servo?pan=${pan.value}&tilt=${tilt.value}`).catch(()=>{});
}
function throttledSend() {
  if (sendTimer) return;
  sendTimer = setTimeout(() => { sendTimer = null; send(); }, 60);
}
pan.addEventListener('input',  throttledSend);
tilt.addEventListener('input', throttledSend);

function preset(p, t) {
  pan.value = p; tilt.value = t; send();
}

setInterval(async () => {
  try {
    const r = await fetch('/status');
    const j = await r.json();
    document.getElementById('rssi').textContent = j.rssi + ' dBm';
    document.getElementById('fps').textContent =
      'FPS: ' + j.fps + ' | PAN ' + j.pan + '° TILT ' + j.tilt + '°';
  } catch(e) {}
}, 1500);
</script>
</body>
</html>
)HTML";

// ================== СОСТОЯНИЕ ==================
httpd_handle_t server = NULL;
unsigned long framesCount = 0;
unsigned long lastFpsTime = 0;
int currentFps = 0;

// ================== КАМЕРА ==================
bool initCamera() {
  camera_config_t cfg = {};
  cfg.ledc_channel = LEDC_CHANNEL_0;
  cfg.ledc_timer   = LEDC_TIMER_0;

  cfg.pin_d0 = Y2_GPIO_NUM;
  cfg.pin_d1 = Y3_GPIO_NUM;
  cfg.pin_d2 = Y4_GPIO_NUM;
  cfg.pin_d3 = Y5_GPIO_NUM;
  cfg.pin_d4 = Y6_GPIO_NUM;
  cfg.pin_d5 = Y7_GPIO_NUM;
  cfg.pin_d6 = Y8_GPIO_NUM;
  cfg.pin_d7 = Y9_GPIO_NUM;

  cfg.pin_xclk     = XCLK_GPIO_NUM;
  cfg.pin_pclk     = PCLK_GPIO_NUM;
  cfg.pin_vsync    = VSYNC_GPIO_NUM;
  cfg.pin_href     = HREF_GPIO_NUM;
  cfg.pin_sccb_sda = SIOD_GPIO_NUM;
  cfg.pin_sccb_scl = SIOC_GPIO_NUM;
  cfg.pin_pwdn     = PWDN_GPIO_NUM;
  cfg.pin_reset    = RESET_GPIO_NUM;

  cfg.xclk_freq_hz = 20000000;
  cfg.pixel_format = PIXFORMAT_JPEG;
  cfg.grab_mode    = CAMERA_GRAB_LATEST;
  cfg.fb_location  = CAMERA_FB_IN_PSRAM;

  if (psramFound()) {
    cfg.frame_size   = FRAMESIZE_SVGA;   // 800x600
    cfg.jpeg_quality = 12;
    cfg.fb_count     = 2;
  } else {
    cfg.frame_size   = FRAMESIZE_QVGA;   // 320x240
    cfg.jpeg_quality = 15;
    cfg.fb_count     = 1;
  }

  esp_err_t err = esp_camera_init(&cfg);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }

  sensor_t* s = esp_camera_sensor_get();
  if (s) {
    s->set_vflip(s, 0);
    s->set_hmirror(s, 0);
  }
  return true;
}

// ================== HTTP HANDLERS ==================
esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  return httpd_resp_send(req, INDEX_HTML, HTTPD_RESP_USE_STRLEN);
}

esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  char part_buf[64];

  res = httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");
  if (res != ESP_OK) return res;

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      res = ESP_FAIL;
      break;
    }

    size_t hlen = snprintf(part_buf, sizeof(part_buf),
      "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
      fb->len);

    res = httpd_resp_send_chunk(req, part_buf, hlen);
    if (res == ESP_OK)
      res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
    if (res == ESP_OK)
      res = httpd_resp_send_chunk(req, "\r\n", 2);

    esp_camera_fb_return(fb);
    if (res != ESP_OK) break;

    framesCount++;
    unsigned long now = millis();
    if (now - lastFpsTime >= 1000) {
      currentFps  = framesCount;
      framesCount = 0;
      lastFpsTime = now;
    }
  }
  return res;
}

esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  esp_err_t r = httpd_resp_send(req, (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return r;
}

int get_query_int(const char* q, const char* key, int def) {
  const char* p = strstr(q, key);
  if (!p) return def;
  p += strlen(key);
  if (*p == '=') p++;
  return atoi(p);
}

esp_err_t servo_handler(httpd_req_t *req) {
  char buf[128];
  int len = httpd_req_get_url_query_len(req) + 1;
  if (len > 1 && len < (int)sizeof(buf)) {
    if (httpd_req_get_url_query_str(req, buf, len) == ESP_OK) {
      targetPan  = constrain(get_query_int(buf, "pan",  targetPan),  0, 180);
      targetTilt = constrain(get_query_int(buf, "tilt", targetTilt), 0, 180);
    }
  }
  httpd_resp_set_type(req, "application/json");
  char out[96];
  snprintf(out, sizeof(out), "{\"pan\":%d,\"tilt\":%d}", targetPan, targetTilt);
  return httpd_resp_send(req, out, HTTPD_RESP_USE_STRLEN);
}

esp_err_t status_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");
  char out[192];
  snprintf(out, sizeof(out),
    "{\"fps\":%d,\"rssi\":%d,\"pan\":%d,\"tilt\":%d,\"heap\":%u}",
    currentFps,
    WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0,
    panAngle, tiltAngle, ESP.getFreeHeap());
  return httpd_resp_send(req, out, HTTPD_RESP_USE_STRLEN);
}

void startServer() {
  httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
  cfg.max_uri_handlers = 8;
  cfg.server_port      = 80;

  if (httpd_start(&server, &cfg) == ESP_OK) {
    httpd_uri_t uri_index   = { "/",         HTTP_GET, index_handler,   NULL };
    httpd_uri_t uri_stream  = { "/stream",   HTTP_GET, stream_handler,  NULL };
    httpd_uri_t uri_capture = { "/capture",  HTTP_GET, capture_handler, NULL };
    httpd_uri_t uri_servo   = { "/servo",    HTTP_GET, servo_handler,   NULL };
    httpd_uri_t uri_status  = { "/status",   HTTP_GET, status_handler,  NULL };

    httpd_register_uri_handler(server, &uri_index);
    httpd_register_uri_handler(server, &uri_stream);
    httpd_register_uri_handler(server, &uri_capture);
    httpd_register_uri_handler(server, &uri_servo);
    httpd_register_uri_handler(server, &uri_status);
  }
}

// ================== СЕРВО ==================
void servoInit() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servoPan.setPeriodHertz(50);
  servoTilt.setPeriodHertz(50);

  servoPan.attach(SERVO_PAN_PIN,   500, 2400);
  servoTilt.attach(SERVO_TILT_PIN, 500, 2400);

  servoPan.write(panAngle);
  servoTilt.write(tiltAngle);
}

void servoUpdate() {
  if (panAngle != targetPan) {
    panAngle += (targetPan > panAngle) ? 1 : -1;
    servoPan.write(panAngle);
  }
  if (tiltAngle != targetTilt) {
    tiltAngle += (targetTilt > tiltAngle) ? 1 : -1;
    servoTilt.write(tiltAngle);
  }
}

// ================== SETUP / LOOP ==================
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== ESP32-S3 FPV boot ===");

  // 1. Серво
  servoInit();
  Serial.println("Servo OK");

  // 2. Камера
  if (!initCamera()) {
    Serial.println("Camera FAIL");
    while (true) delay(1000);
  }
  Serial.println("Camera OK");

  // 3. Wi-Fi
#ifdef USE_STA
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_SSID, STA_PASS);
  Serial.print("Connecting to WiFi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nSTA IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nSTA failed, fallback to AP");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
  }
#else
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
#endif

  Serial.printf("AP SSID: %s\n", AP_SSID);
  Serial.printf("AP PASS: %s\n", AP_PASS);
  Serial.printf("Open:    http://%s/\n", WiFi.softAPIP().toString().c_str());

  // 4. HTTP-сервер
  startServer();
  Serial.println("HTTP server started");

  Serial.printf("PSRAM found: %s\n", psramFound() ? "YES" : "NO");
Serial.printf("PSRAM size:  %u bytes\n", ESP.getPsramSize());
Serial.printf("Flash size:  %u bytes\n", ESP.getFlashChipSize());
}

void loop() {
  servoUpdate();
  delay(15);
}