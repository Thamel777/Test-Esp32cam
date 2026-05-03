#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "esp_camera.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "Test - Student";
const char* password = "BCI#campus";

// AI Thinker Camera Pins
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;

  // Set up for RGB565 since this sensor doesn't support JPEG
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size = FRAMESIZE_VGA; // Use VGA to avoid memory crash
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }
  
  // Diagnostic output
  sensor_t * s = esp_camera_sensor_get();
  if (s) {
      Serial.printf("Sensor PID: 0x%x\n", s->id.PID);
  }
  
  if (psramFound()) {
    Serial.println("PSRAM Status: DETECTED!");
  } else {
    Serial.println("PSRAM Status: NOT DETECTED");
  }

  server.begin();
  Serial.println("==================================");
  Serial.print("Camera Ready! Go to: http://");
  Serial.println(WiFi.localIP());
  Serial.println("==================================");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    camera_fb_t * fb = esp_camera_fb_get();
    if(!fb) {
      Serial.println("Camera capture failed");
      client.stop();
      return;
    }
    
    // Attempt to convert to JPEG (since the sensor doesn't do it)
    uint8_t * out_buf = NULL;
    size_t out_len = 0;
    bool jpeg_converted = frame2jpg(fb, 80, &out_buf, &out_len);
    
    // Send HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: image/jpeg");
    client.println("Connection: close");
    client.println();
    
    // Send image data
    if (jpeg_converted) {
      client.write(out_buf, out_len);
      free(out_buf); // Free the software-encoded JPEG buffer
    } else {
      Serial.println("JPEG compression failed");
    }
    
    esp_camera_fb_return(fb); // Free the camera frame buffer
    client.stop();
  }
}