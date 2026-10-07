/*
 * ESP32-S3 FPV Camera — два сервера + управление серво
 * Порт 80: HTML, JS, /stream, /capture, /servo
 * Порт 81: /status (телеметрия)
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <ESP32Servo.h>
#include "esp_http_server.h"
#include "html.h"
#include "app.js.h"

// ================== НАСТРОЙКИ Wi-Fi ==================
const char* WIFI_SSID = "RGB_Route";
const char* WIFI_PASS = "1234567890";

// ================== ПИНЫ КАМЕРЫ ==================
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

// ================== ПИН СЕРВО ==================
#define SERVO_PIN         21

Servo cameraServo;
int currentAngle = 90;
int targetAngle  = 90;

// ================== ГЛОБАЛЬНЫЕ ==================
httpd_handle_t stream_server = NULL;   // порт 80
httpd_handle_t ctrl_server   = NULL;   // порт 81
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
  if (s) {
    s->set_vflip(s, 0);
    s->set_hmirror(s, 0);
  }
  return true;
}

// ============================================================
//  HANDLERS — ПОРТ 80
// ============================================================

static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-store");
  return httpd_resp_send(req, INDEX_HTML, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t appjs_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/javascript; charset=utf-8");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-store");
  return httpd_resp_send(req, APP_JS, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  char part_buf[64];

  res = httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");
  if (res != ESP_OK) return res;

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

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
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  esp_err_t r = httpd_resp_send(req, (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return r;
}

// НОВОЕ — управление серво
static esp_err_t servo_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");

  char buf[64];
  int len = httpd_req_get_url_query_len(req) + 1;
  int angle = targetAngle;

  if (len > 1 && len < (int)sizeof(buf)) {
    if (httpd_req_get_url_query_str(req, buf, len) == ESP_OK) {
      char val[8];
      if (httpd_query_key_value(buf, "angle", val, sizeof(val)) == ESP_OK) {
        angle = atoi(val);
        if (angle < 0)   angle = 0;
        if (angle > 180) angle = 180;
        targetAngle = angle;
      }
    }
  }

  char out[64];
  snprintf(out, sizeof(out), "{\"angle\":%d}", targetAngle);
  return httpd_resp_send(req, out, HTTPD_RESP_USE_STRLEN);
}

// ============================================================
//  HANDLER — ПОРТ 81 (ТОЛЬКО СТАТУС)
// ============================================================

static esp_err_t status_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  char out[128];
  snprintf(out, sizeof(out),
    "{\"fps\":%d,\"rssi\":%d,\"heap\":%u,\"angle\":%d}",
    currentFps,
    WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0,
    ESP.getFreeHeap(),
    currentAngle);
  return httpd_resp_send(req, out, HTTPD_RESP_USE_STRLEN);
}

// ============================================================
//  СЕРВЕР 1 — ПОРТ 80
// ============================================================

void startStreamServer() {
  httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
  cfg.max_uri_handlers = 8;
  cfg.server_port      = 80;
  cfg.ctrl_port        = 32768;
  cfg.lru_purge_enable = true;
  cfg.stack_size       = 8192;

  httpd_uri_t uri_index   = { "/",        HTTP_GET, index_handler,   NULL };
  httpd_uri_t uri_appjs   = { "/app.js",  HTTP_GET, appjs_handler,   NULL };
  httpd_uri_t uri_stream  = { "/stream",  HTTP_GET, stream_handler,  NULL };
  httpd_uri_t uri_capture = { "/capture", HTTP_GET, capture_handler, NULL };
  httpd_uri_t uri_servo   = { "/servo",   HTTP_GET, servo_handler,   NULL };

  if (httpd_start(&stream_server, &cfg) == ESP_OK) {
    httpd_register_uri_handler(stream_server, &uri_index);
    httpd_register_uri_handler(stream_server, &uri_appjs);
    httpd_register_uri_handler(stream_server, &uri_stream);
    httpd_register_uri_handler(stream_server, &uri_capture);
    httpd_register_uri_handler(stream_server, &uri_servo);
    Serial.println("✅ Stream server: port 80 (HTML, JS, /stream, /capture, /servo)");
  } else {
    Serial.println("❌ Stream server FAILED");
  }
}

// ============================================================
//  СЕРВЕР 2 — ПОРТ 81 (только /status)
// ============================================================

void startCtrlServer() {
  httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
  cfg.max_uri_handlers = 2;
  cfg.server_port      = 81;
  cfg.ctrl_port        = 32769;
  cfg.lru_purge_enable = true;
  cfg.stack_size       = 4096;

  httpd_uri_t uri_status = { "/status", HTTP_GET, status_handler, NULL };

  if (httpd_start(&ctrl_server, &cfg) == ESP_OK) {
    httpd_register_uri_handler(ctrl_server, &uri_status);
    Serial.println("✅ Ctrl server: port 81 (/status)");
  } else {
    Serial.println("❌ Ctrl server FAILED");
  }
}

// ============================================================
//  SETUP
// ============================================================

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== ESP32-S3 FPV Camera ===");

  // --- СЕРВО ---
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  cameraServo.setPeriodHertz(50);
  cameraServo.attach(SERVO_PIN, 500, 2400);
  cameraServo.write(currentAngle);
  Serial.println("Servo OK");

  // --- КАМЕРА ---
  if (!initCamera()) {
    Serial.println("Camera FAIL");
    while (true) delay(1000);
  }
  Serial.println("Camera OK");

  // --- Wi-Fi ---
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
    Serial.print("Web:     http://");
    Serial.println(WiFi.localIP());
    Serial.printf("Stream:  http://%s/stream\n", WiFi.localIP().toString().c_str());
    Serial.printf("Servo:   http://%s/servo?angle=90\n", WiFi.localIP().toString().c_str());
    Serial.printf("Status:  http://%s:81/status\n", WiFi.localIP().toString().c_str());
    Serial.printf("RSSI:    %d dBm\n", WiFi.RSSI());
  } else {
    Serial.println("\n❌ WiFi failed! Restarting...");
    ESP.restart();
  }

  startStreamServer();
  startCtrlServer();
}

// ============================================================
//  LOOP — плавное движение серво
// ============================================================

void loop() {
  if (currentAngle != targetAngle) {
    if (currentAngle < targetAngle) currentAngle++;
    else currentAngle--;
    cameraServo.write(currentAngle);
  }
  delay(15);
}
