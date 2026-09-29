#pragma once

// ===== Camera pins для ESP32-S3 + OV2640 (FPC-шлейф) =====
// Эти пины стандартные для плат ESP32-S3-EYE / DevKitC-1 с камерой.
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

// ===== Servo pins =====
#define SERVO_PAN_PIN   38
#define SERVO_TILT_PIN  39
