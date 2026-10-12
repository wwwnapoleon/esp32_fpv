/*
 * ESP32-S3 GreenLoop FPV · FINAL v8
 *
 * HTML вынесен в отдельные .h файлы:
 *  - stream_html.h — страница стрима (SVG-иконки)
 *  - setup_html.h  — страница настройки Wi-Fi
 *
 * ДВУХПОРТОВАЯ СХЕМА:
 *  - Порт 80 → UI: /, /setup, /scan, /connect, /status, /servo
 *  - Порт 81 → ТОЛЬКО стрим: /stream
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <ESP32Servo.h>
#include "esp_http_server.h"
#include <ESPmDNS.h>

#include "stream_html.h"
#include "setup_html.h"

// ================== AP ==================
const char* AP_SSID = "GreenLoop-Setup";
const char* AP_PASS = "12345678";

// ================== STA ==================
const char* STA_SSID = "RGB_Route";
const char* STA_PASS = "1234567890";

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
httpd_handle_t ui_server     = NULL;
httpd_handle_t stream_server = NULL;
unsigned long framesCount = 0;
unsigned long lastFpsTime = 0;
int currentFps = 0;
bool cameraOK = false;

// ================== КАМЕРА ==================
bool initCamera() {
  camera_config_t cfg = {};
  cfg.ledc_channel = LEDC_CHANNEL_0;
  cfg.ledc_timer   = LEDC_TIMER_0;
  cfg.pin_d0 = Y2_GPIO_NUM;  cfg.pin_d1 = Y3_GPIO_NUM;
  cfg.pin_d2 = Y4_GPIO_NUM;  cfg.pin_d3 = Y5_GPIO_NUM;
  cfg.pin_d4 = Y6_GPIO_NUM;  cfg.pin_d5 = Y7_GPIO_NUM;
  cfg.pin_d6 = Y8_GPIO_NUM;  cfg.pin_d7 = Y9_GPIO_NUM;
  cfg.pin_xclk = XCLK_GPIO_NUM;  cfg.pin_pclk = PCLK_GPIO_NUM;
  cfg.pin_vsync = VSYNC_GPIO_NUM;  cfg.pin_href = HREF_GPIO_NUM;
  cfg.pin_sccb_sda = SIOD_GPIO_NUM;  cfg.pin_sccb_scl = SIOC_GPIO_NUM;
  cfg.pin_pwdn = PWDN_GPIO_NUM;  cfg.pin_reset = RESET_GPIO_NUM;
  cfg.xclk_freq_hz = 10000000;
  cfg.pixel_format = PIXFORMAT_JPEG;
  cfg.grab_mode    = CAMERA_GRAB_LATEST;
  cfg.fb_location  = CAMERA_FB_IN_PSRAM;

  if (psramFound()) {
    cfg.frame_size   = FRAMESIZE_VGA;
    cfg.jpeg_quality = 22;
    cfg.fb_count     = 1;
  }

  esp_err_t err = esp_camera_init(&cfg);
  if (err != ESP_OK) {
    Serial.printf("❌ esp_camera_init: 0x%x\n", err);
    return false;
  }
  return true;
}

// ================== URL-DECODE ==================
void url_decode(char *s) {
  char *p = s;
  while (*s) {
    if (*s == '%' && s[1] && s[2]) {
      char hex[3] = { s[1], s[2], 0 };
      *p++ = (char)strtol(hex, NULL, 16);
      s += 3;
    } else if (*s == '+') { *p++ = ' '; s++; }
    else { *p++ = *s++; }
  }
  *p = 0;
}

// ============================================================
//  UI HANDLERS — ПОРТ 80
// ============================================================

static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  return httpd_resp_send(req, STREAM_HTML, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t setup_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  return httpd_resp_send(req, SETUP_HTML, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t scan_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
  }
  json += "]";
  WiFi.scanDelete();

  return httpd_resp_send(req, json.c_str(), HTTPD_RESP_USE_STRLEN);
}

static esp_err_t connect_handler(httpd_req_t *req) {
  char body[256] = {0};
  int ret = httpd_req_recv(req, body, sizeof(body) - 1);
  if (ret <= 0) {
    httpd_resp_set_status(req, "400");
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req,
      "{\"error\":\"Нет данных от формы\"}", HTTPD_RESP_USE_STRLEN);
  }
  body[ret] = 0;

  char ssid[64] = {0}, pass[64] = {0};
  char *p = strstr(body, "ssid=");
  if (p) {
    p += 5;
    char *amp = strchr(p, '&');
    int len = amp ? (amp - p) : (int)strlen(p);
    if (len > 63) len = 63;
    strncpy(ssid, p, len);
  }
  p = strstr(body, "pass=");
  if (p) {
    p += 5;
    int len = strlen(p);
    if (len > 63) len = 63;
    strncpy(pass, p, len);
  }

  url_decode(ssid);
  url_decode(pass);

  Serial.printf("📥 Ручное подключение к '%s'\n", ssid);

  WiFi.disconnect(false, false);
  delay(100);
  WiFi.begin(ssid, pass);

  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    String lanIP = WiFi.localIP().toString();
    Serial.printf("✅ STA подключено: %s\n", lanIP.c_str());

    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    String html =
      "<!DOCTYPE html><html lang='ru'><head><meta charset='UTF-8'>"
      "<meta name='viewport' content='width=device-width,initial-scale=1'>"
      "<title>GreenLoop · Подключено</title>"
      "<style>"
      "*{box-sizing:border-box}"
      "body{background:#0d1117;color:#e6edf3;font-family:-apple-system,sans-serif;"
      "display:flex;justify-content:center;align-items:center;"
      "min-height:100vh;margin:0;padding:20px}"
      ".card{background:#161b22;border:1px solid #30363d;border-radius:16px;"
      "padding:32px;max-width:480px;width:100%;text-align:center}"
      "h1{color:#3fb950;margin:0 0 8px}"
      ".ok{background:rgba(63,185,80,0.15);color:#3fb950;padding:12px;"
      "border-radius:8px;margin:16px 0;font-weight:600}"
      ".ip{background:#0d1117;padding:10px;border-radius:8px;"
      "font-family:monospace;color:#58a6ff;margin:8px 0;font-size:0.95rem}"
      ".btn{display:block;margin:12px 0;padding:16px;background:#3fb950;"
      "color:#fff;text-decoration:none;border-radius:8px;font-weight:700;"
      "font-size:1rem;transition:background 0.2s}"
      ".btn:hover{background:#2ea043}"
      ".btn--alt{background:#21262d;border:1px solid #30363d}"
      ".btn--alt:hover{background:#30363d;border-color:#3fb950}"
      ".back{display:inline-block;margin-top:16px;color:#58a6ff;"
      "text-decoration:none;font-size:0.9rem}"
      "</style></head><body>"
      "<div class='card'>"
      "<h1>✅ Подключено!</h1>"
      "<div class='ok'>ESP32 успешно подключена к сети</div>"

      "<p style='opacity:0.7;margin:8px 0'>LAN-адрес:</p>"
      "<div class='ip'>" + lanIP + "</div>"
      "<a href='http://" + lanIP + "/' class='btn'>🚀 Открыть через LAN</a>"

      "<p style='opacity:0.7;margin:16px 0 8px'>AP-адрес:</p>"
      "<div class='ip'>192.168.4.1</div>"
      "<a href='http://192.168.4.1/' class='btn btn--alt'>📡 Открыть через AP</a>"

      "<p style='opacity:0.7;margin:16px 0 8px'>mDNS:</p>"
      "<div class='ip'>greenloop.local</div>"
      "<a href='http://greenloop.local/' class='btn btn--alt'>🌐 Открыть через mDNS</a>"

      "<a href='/setup' class='back'>← Назад к настройкам</a>"
      "</div></body></html>";

    return httpd_resp_send(req, html.c_str(), HTTPD_RESP_USE_STRLEN);
  }

  Serial.println("❌ STA FAIL");
  httpd_resp_set_status(req, "400");
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  String errJson = "{\"error\":\"Неверный пароль или сеть '" + String(ssid) + "' недоступна\"}";
  return httpd_resp_send(req, errJson.c_str(), HTTPD_RESP_USE_STRLEN);
}

static esp_err_t servo_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  char buf[64];
  int len = httpd_req_get_url_query_len(req) + 1;
  if (len > 1 && len < (int)sizeof(buf)) {
    if (httpd_req_get_url_query_str(req, buf, len) == ESP_OK) {
      char val[8];
      if (httpd_query_key_value(buf, "angle", val, sizeof(val)) == ESP_OK) {
        int angle = atoi(val);
        if (angle < 0) angle = 0;
        if (angle > 180) angle = 180;
        targetAngle = angle;
        Serial.printf("🎛️ Servo -> %d°\n", angle);
      }
    }
  }
  char out[32];
  snprintf(out, sizeof(out), "{\"angle\":%d}", targetAngle);
  return httpd_resp_send(req, out, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t status_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  String ip = (WiFi.status() == WL_CONNECTED)
    ? WiFi.localIP().toString()
    : WiFi.softAPIP().toString();

  int rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;

  char out[200];
  snprintf(out, sizeof(out),
    "{\"fps\":%d,\"rssi\":%d,\"ip\":\"%s\",\"angle\":%d,\"cam\":%d,\"heap\":%u}",
    currentFps, rssi, ip.c_str(), currentAngle,
    cameraOK ? 1 : 0, ESP.getFreeHeap());
  return httpd_resp_send(req, out, HTTPD_RESP_USE_STRLEN);
}

// ============================================================
//  STREAM HANDLER — ПОРТ 81
// ============================================================

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  char part_buf[64];

  res = httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");
  if (res != ESP_OK) return res;
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("⚠️ fb NULL");
      res = ESP_FAIL;
      break;
    }

    size_t hlen = snprintf(part_buf, sizeof(part_buf),
      "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);

    res = httpd_resp_send_chunk(req, part_buf, hlen);
    if (res == ESP_OK) res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
    if (res == ESP_OK) res = httpd_resp_send_chunk(req, "\r\n", 2);

    esp_camera_fb_return(fb);
    if (res != ESP_OK) break;

    framesCount++;
  }
  return res;
}

// ============================================================
//  ЗАПУСК UI-СЕРВЕРА — ПОРТ 80
// ============================================================

void startUIServer() {
  httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
  cfg.max_uri_handlers  = 12;
  cfg.server_port       = 80;
  cfg.ctrl_port         = 32768;
  cfg.lru_purge_enable  = true;
  cfg.stack_size        = 8192;

  cfg.max_open_sockets  = 13;
  cfg.backlog_conn      = 5;
  cfg.recv_wait_timeout = 5;
  cfg.send_wait_timeout = 5;

  httpd_uri_t uri_index   = { "/",        HTTP_GET,  index_handler,   NULL };
  httpd_uri_t uri_setup   = { "/setup",   HTTP_GET,  setup_handler,   NULL };
  httpd_uri_t uri_scan    = { "/scan",    HTTP_GET,  scan_handler,    NULL };
  httpd_uri_t uri_connect = { "/connect", HTTP_POST, connect_handler, NULL };
  httpd_uri_t uri_servo   = { "/servo",   HTTP_GET,  servo_handler,   NULL };
  httpd_uri_t uri_status  = { "/status",  HTTP_GET,  status_handler,  NULL };

  if (httpd_start(&ui_server, &cfg) == ESP_OK) {
    httpd_register_uri_handler(ui_server, &uri_index);
    httpd_register_uri_handler(ui_server, &uri_setup);
    httpd_register_uri_handler(ui_server, &uri_scan);
    httpd_register_uri_handler(ui_server, &uri_connect);
    httpd_register_uri_handler(ui_server, &uri_servo);
    httpd_register_uri_handler(ui_server, &uri_status);
    Serial.println("✅ UI server:     port 80");
  } else {
    Serial.println("❌ UI server FAILED");
  }
}

// ============================================================
//  ЗАПУСК STREAM-СЕРВЕРА — ПОРТ 81
// ============================================================

void startStreamServer() {
  httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
  cfg.max_uri_handlers  = 2;
  cfg.server_port       = 81;
  cfg.ctrl_port         = 32769;
  cfg.lru_purge_enable  = true;
  cfg.stack_size        = 8192;

  cfg.max_open_sockets  = 7;
  cfg.backlog_conn      = 5;
  cfg.recv_wait_timeout = 5;
  cfg.send_wait_timeout = 10;

  httpd_uri_t uri_stream = { "/stream", HTTP_GET, stream_handler, NULL };

  if (httpd_start(&stream_server, &cfg) == ESP_OK) {
    httpd_register_uri_handler(stream_server, &uri_stream);
    Serial.println("✅ Stream server: port 81");
  } else {
    Serial.println("❌ Stream server FAILED");
  }
}

// ================== SETUP ==================
void setup() {
  #if ARDUINO_USB_CDC_ON_BOOT
    Serial.begin(115200);
    unsigned long t = millis();
    while (!Serial && millis() - t < 3000) delay(10);
  #else
    Serial.begin(115200);
    delay(1000);
  #endif

  delay(500);
  Serial.println("\n=== BOOT ===");
  Serial.println("=== GreenLoop FPV · FINAL v8 (split headers) ===");
  Serial.printf("PSRAM found: %d\n", psramFound());
  Serial.printf("Free heap: %u\n", ESP.getFreeHeap());

  // === СЕРВО ===
  Serial.println("⏳ Servo init...");
  ESP32PWM::allocateTimer(1);
  cameraServo.setPeriodHertz(50);
  cameraServo.attach(SERVO_PIN, 500, 2400);
  cameraServo.write(currentAngle);
  Serial.println("✅ Servo OK");

  // === КАМЕРА ===
  Serial.println("⏳ Camera init...");
  if (!initCamera()) {
    Serial.println("❌ Camera FAIL");
    cameraOK = false;
  } else {
    cameraOK = true;
    Serial.println("✅ Camera init OK");

    sensor_t *s = esp_camera_sensor_get();
    if (s) {
      Serial.printf("   Sensor PID: 0x%02X\n", s->id.PID);
      Serial.printf("   Sensor VER: 0x%02X\n", s->id.VER);
    }

    camera_fb_t *test_fb = esp_camera_fb_get();
    if (test_fb) {
      Serial.printf("✅ Test frame: %u bytes\n", test_fb->len);
      esp_camera_fb_return(test_fb);
    } else {
      Serial.println("❌ Test frame FAIL");
      cameraOK = false;
    }
  }

  // === AP + STA ===
  Serial.println("⏳ WiFi AP + STA...");
  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.printf("📡 AP:  %s / %s\n", AP_SSID, AP_PASS);
  Serial.printf("   IP:  http://%s\n", WiFi.softAPIP().toString().c_str());

  Serial.printf("🔌 STA: подключение к '%s'...\n", STA_SSID);
  WiFi.begin(STA_SSID, STA_PASS);

  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("✅ STA подключено: http://%s\n", WiFi.localIP().toString().c_str());
    Serial.printf("   RSSI: %d dBm\n", WiFi.RSSI());

    if (MDNS.begin("greenloop")) {
      MDNS.addService("http", "tcp", 80);
      Serial.println("🌐 mDNS: http://greenloop.local");
    } else {
      Serial.println("⚠️ mDNS FAIL");
    }
  } else {
    Serial.println("⚠️ STA не подключилось — только AP");
  }

  // === СЕРВЕРЫ ===
  Serial.println("⏳ HTTP servers...");
  startUIServer();
  startStreamServer();

  Serial.println("🚀 Готово!");
  Serial.println("   UI:     http://greenloop.local/");
  Serial.println("   Stream: http://greenloop.local:81/stream");
}

// ================== LOOP ==================
void loop() {
  unsigned long now = millis();
  if (now - lastFpsTime >= 1000) {
    currentFps = framesCount;
    framesCount = 0;
    lastFpsTime = now;
  }

  if (currentAngle != targetAngle) {
    if (currentAngle < targetAngle) currentAngle++;
    else currentAngle--;
    cameraServo.write(currentAngle);
  }
  delay(5);
}