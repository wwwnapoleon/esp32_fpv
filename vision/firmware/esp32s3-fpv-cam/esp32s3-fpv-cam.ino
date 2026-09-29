/*
 * ESP32-S3 FPV Camera — только видео
 * Плата: ESP32-S3-EYE (или любая ESP32-S3 с камерой на шлейфе)
 * Подключение: Wi-Fi STA к роутеру M9S
 */

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"

// ================== НАСТРОЙКИ Wi-Fi ==================
const char* WIFI_SSID = "4G-MiFi-96E0";
const char* WIFI_PASS = "1234567890";

// ================== ПИНЫ КАМЕРЫ (ESP32-S3-EYE) ==================
// Для ESP32-S3-EYE — распиновка фиксированная, соответствует шлейфу
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM     4
#define SIOC_GPIO_NUM     5

#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM       8
#define Y3_GPIO_NUM       9
#define Y2_GPIO_NUM       11

#define VSYNC_GPIO_NUM    6
#define HREF_GPIO_NUM     7
#define PCLK_GPIO_NUM     13

// ================== HTML ==================
const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32-S3 FPV</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body { background:#111; color:#eee; font-family:-apple-system,Roboto,sans-serif;
         display:flex; flex-direction:column; height:100vh; overflow:hidden; }
  header { padding:10px 14px; background:#1c1c1c; display:flex;
           justify-content:space-between; align-items:center; font-size:14px; }
  #stream { flex:1; width:100%; object-fit:contain; background:#000; }
  .status { padding:8px; background:#1c1c1c; font-size:12px; color:#888;
            text-align:center; }
</style>
</head>
<body>
<header>
  <span>📷 ESP32-S3 FPV</span>
  <span id="status">…</span>
</header>
<img id="stream" src="/stream">
<div class="status" id="fps">FPS: —</div>
<script>
setInterval(async () => {
  try {
    const r = await fetch('/status');
    const j = await r.json();
    document.getElementById('status').textContent = j.rssi + ' dBm';
    document.getElementById('fps').textContent = 'FPS: ' + j.fps;
  } catch(e) {}
}, 2000);
</script>
</body>
</html>
)HTML";

// ================== ГЛОБАЛЬНЫЕ ==================
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
  cfg.pin_xclk = XCLK_GPIO_NUM;
  cfg.pin_pclk = PCLK_GPIO_NUM;
  cfg.pin_vsync = VSYNC_GPIO_NUM;
  cfg.pin_href  = HREF_GPIO_NUM;
  cfg.pin_sccb_sda = SIOD_GPIO_NUM;
  cfg.pin_sccb_scl = SIOC_GPIO_NUM;
  cfg.pin_pwdn  = PWDN_GPIO_NUM;
  cfg.pin_reset = RESET_GPIO_NUM;
  cfg.xclk_freq_hz = 20000000;
  cfg.pixel_format = PIXFORMAT_JPEG;
  cfg.grab_mode = CAMERA_GRAB_LATEST;
  cfg.fb_location = CAMERA_FB_IN_PSRAM;

  if (psramFound()) {
    cfg.frame_size   = FRAMESIZE_SVGA;
    cfg.jpeg_quality = 12;
    cfg.fb_count     = 2;
  } else {
    cfg.frame_size   = FRAMESIZE_QVGA;
    cfg.jpeg_quality = 15;
    cfg.fb_count     = 1;
  }

  esp_err_t err = esp_camera_init(&cfg);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }

  sensor_t* s = esp_camera_sensor_get();
  s->set_vflip(s, 0);
  s->set_hmirror(s, 0);
  return true;
}

// ================== HTTP HANDLERS ==================
static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  return httpd_resp_send(req, INDEX_HTML, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  char part_buf[64];

  res = httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");
  if (res != ESP_OK) return res;

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) { res = ESP_FAIL; break; }

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
      currentFps = framesCount;
      framesCount = 0;
      lastFpsTime = now;
    }
  }
  return res;
}

static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) { httpd_resp_send_500(req); return ESP_FAIL; }
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  esp_err_t r = httpd_resp_send(req, (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return r;
}

static esp_err_t status_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");
  char out[128];
  snprintf(out, sizeof(out),
    "{\"fps\":%d,\"rssi\":%d,\"heap\":%u}",
    currentFps,
    WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0,
    ESP.getFreeHeap());
  return httpd_resp_send(req, out, HTTPD_RESP_USE_STRLEN);
}

// ================== SERVER ==================
void startServer() {
  httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
  cfg.max_uri_handlers = 6;
  cfg.server_port = 80;
  cfg.lru_purge_enable = true;

  httpd_uri_t uri_index   = { "/",         HTTP_GET, index_handler,   NULL };
  httpd_uri_t uri_stream  = { "/stream",   HTTP_GET, stream_handler,  NULL };
  httpd_uri_t uri_capture = { "/capture",  HTTP_GET, capture_handler, NULL };
  httpd_uri_t uri_status  = { "/status",   HTTP_GET, status_handler,  NULL };

  if (httpd_start(&server, &cfg) == ESP_OK) {
    httpd_register_uri_handler(server, &uri_index);
    httpd_register_uri_handler(server, &uri_stream);
    httpd_register_uri_handler(server, &uri_capture);
    httpd_register_uri_handler(server, &uri_status);
    Serial.println("HTTP server started on port 80");
  }
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== ESP32-S3 FPV Camera ===");

  if (!initCamera()) {
    Serial.println("Camera FAIL");
    while (true) delay(1000);
  }
  Serial.println("Camera OK");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Connecting to %s", WIFI_SSID);

  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi connected!");
    Serial.print("ESP32 IP: http://");
    Serial.println(WiFi.localIP());
    Serial.printf("Stream:   http://%s/stream\n", WiFi.localIP().toString().c_str());
    Serial.printf("Status:   http://%s/status\n", WiFi.localIP().toString().c_str());
    Serial.printf("RSSI:     %d dBm\n", WiFi.RSSI());
  } else {
    Serial.println("\n❌ WiFi failed! Restarting...");
    ESP.restart();
  }

  startServer();
}

void loop() {
  delay(1000);
}
