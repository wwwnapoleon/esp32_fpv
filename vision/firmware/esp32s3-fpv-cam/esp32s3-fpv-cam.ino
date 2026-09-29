#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"
#include "camera_pins.h"
#include "servo_ctrl.h"
#include "web_ui.h"

// ==== Настройки сети (STA) ====
const char* STA_SSID = "4G-MiFi-96E0";      // SSID вашего роутера
const char* STA_PASS = "1234567890";         // Пароль Wi-Fi

httpd_handle_t stream_httpd = NULL;
httpd_handle_t ctrl_httpd   = NULL;

unsigned long framesCount = 0;
unsigned long lastFpsTime = 0;
int currentFps = 0;

// ================== CAMERA ==================
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

// ================== HANDLERS ==================
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

static int get_query_int(const char* q, const char* key, int def) {
  const char* p = strstr(q, key);
  if (!p) return def;
  p += strlen(key);
  if (*p == '=') p++;
  return atoi(p);
}

static esp_err_t servo_handler(httpd_req_t *req) {
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

static esp_err_t status_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");
  char out[160];
  snprintf(out, sizeof(out),
    "{\"fps\":%d,\"rssi\":%d,\"pan\":%d,\"tilt\":%d,\"heap\":%u}",
    currentFps,
    WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0,
    panAngle, tiltAngle, ESP.getFreeHeap());
  return httpd_resp_send(req, out, HTTPD_RESP_USE_STRLEN);
}

// ================== SERVER ==================
void startServers() {
  httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
  cfg.max_uri_handlers = 8;
  cfg.server_port = 80;

  httpd_uri_t uri_index   = { "/",         HTTP_GET, index_handler,   NULL };
  httpd_uri_t uri_stream  = { "/stream",   HTTP_GET, stream_handler,  NULL };
  httpd_uri_t uri_capture = { "/capture",  HTTP_GET, capture_handler, NULL };
  httpd_uri_t uri_servo   = { "/servo",    HTTP_GET, servo_handler,   NULL };
  httpd_uri_t uri_status  = { "/status",   HTTP_GET, status_handler,  NULL };

  if (httpd_start(&ctrl_httpd, &cfg) == ESP_OK) {
    httpd_register_uri_handler(ctrl_httpd, &uri_index);
    httpd_register_uri_handler(ctrl_httpd, &uri_stream);
    httpd_register_uri_handler(ctrl_httpd, &uri_capture);
    httpd_register_uri_handler(ctrl_httpd, &uri_servo);
    httpd_register_uri_handler(ctrl_httpd, &uri_status);
  }
}

// ================== SETUP / LOOP ==================
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== ESP32-S3 FPV boot ===");

  servoInit();

  if (!initCamera()) {
    Serial.println("Camera FAIL");
    while (true) delay(1000);
  }
  Serial.println("Camera OK");

  // Подключаемся к роутеру M9S как STA
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_SSID, STA_PASS);
  Serial.print("Connecting to WiFi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) {
    delay(500); Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Stream: http://%s/stream\n", WiFi.localIP().toString().c_str());
    Serial.printf("Status: http://%s/status\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nWiFi failed! Restarting...");
    ESP.restart();
  }

  startServers();
}

void loop() {
  servoUpdate();
  delay(15);
}
