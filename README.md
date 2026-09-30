# Test-Esp32cam

This repository contains C++ code for an ESP32-CAM (AI Thinker module) project using Arduino framework/PlatformIO. The project connects to a WiFi network, initializes the camera (using RGB565 pixel format with VGA frame size), and hosts a web server that serves captured images as JPEGs. 

## Features
- Captures frames from the ESP32-CAM module
- Converts RGB565 frames to JPEG format
- Hosts an HTTP server on port 80 for individual JPEG captures via WiFi

## Wiring & Pin Definitions (AI Thinker Module)

The following diagram and pinouts describe the internal connections of the ESP32-CAM module to the OV2640 camera sensor.

### Pinout Table

| Camera Pin | ESP32 GPIO | Description     |
|:----------:|:----------:|:----------------|
| **PWDN**   | GPIO 32    | Power Down      |
| **RESET**  | -1         | Reset           |
| **XCLK**   | GPIO 0     | External Clock  |
| **SIOD**   | GPIO 26    | SDA (I2C Data)  |
| **SIOC**   | GPIO 27    | SCL (I2C Clock) |
| **Y9 (D7)**| GPIO 35    | Data Pin 7      |
| **Y8 (D6)**| GPIO 34    | Data Pin 6      |
| **Y7 (D5)**| GPIO 39    | Data Pin 5      |
| **Y6 (D4)**| GPIO 36    | Data Pin 4      |
| **Y5 (D3)**| GPIO 21    | Data Pin 3      |
| **Y4 (D2)**| GPIO 19    | Data Pin 2      |
| **Y3 (D1)**| GPIO 18    | Data Pin 1      |
| **Y2 (D0)**| GPIO 5     | Data Pin 0      |
| **VSYNC**  | GPIO 25    | Vertical Sync   |
| **HREF**   | GPIO 23    | Horizontal Ref  |
| **PCLK**   | GPIO 22    | Pixel Clock     |

### Wiring Diagram

```mermaid
graph LR
    subgraph ESP32-CAM
        ESP[ESP32 Microcontroller]
        CAM[Camera Sensor OV2640]
        
        CAM -- "PWDN" --> |GPIO 32| ESP
        CAM -- "XCLK" --> |GPIO 0| ESP
        CAM -- "SIOD (SDA)" --> |GPIO 26| ESP
        CAM -- "SIOC (SCL)" --> |GPIO 27| ESP
        CAM -- "D7..D0" --> |GPIO 35,34,39,36,21,19,18,5| ESP
        CAM -- "VSYNC" --> |GPIO 25| ESP
        CAM -- "HREF" --> |GPIO 23| ESP
        CAM -- "PCLK" --> |GPIO 22| ESP
    end
```

## ESP32-S3 Smart Bin Firmware 2.5.0 compatibility

The AI Thinker pin definitions and `platformio.ini` are unchanged. RGB565/VGA capture and software JPEG conversion are retained. The Arduino framework's built-in WebServer parses requests before any capture takes place.

| Request | Response |
| --- | --- |
| `GET /` | `200 text/plain`, `ESP32-CAM ready` (503 if camera initialization failed) |
| `GET /capture` | `200 image/jpeg`, raw JPEG bytes with an explicit positive Content-Length and Cache-Control: no-store |
| Other methods on `/` or `/capture` | `405 text/plain`, Allow: GET |
| Unknown paths | `404 text/plain` |
| Capture or JPEG conversion failure | `500 text/plain` |
| Capture with camera unavailable | `503 text/plain` |

Each Wi-Fi connection attempt is limited to 15 seconds. The existing primary network is tried first. If unavailable, the camera joins the S3's **access point as a station** using SSID `ESP32-S3-N16R8`, password `change-this-password`, static address `192.168.4.2`, gateway `192.168.4.1`, and mask `255.255.255.0`. Match the fallback password to the actual S3 configuration; earlier builds used other placeholder passwords. If both attempts fail, setup returns; check the networks/credentials and restart. This minimal change does not add runtime network switching.

The S3 fallback URL is `http://192.168.4.2/capture`. The fixed address applies only on the fallback network. On primary Wi-Fi, use the camera's DHCP address printed over serial and configure the S3 URL accordingly if capturing there. Ensure the S3 DHCP pool does not allocate `.2` to the tablet or another station: a static camera address alone does not reserve it. No S3 Firmware 2.5.0 changes are included here.

The root URL no longer returns an image; update older browser clients to `/capture`. This is a single-frame endpoint, not a streaming endpoint. The camera provides bytes only; classification remains in the S3 pipeline.

### Hardware verification

1. Build with `pio run -e esp32cam`, flash, and confirm the primary Wi-Fi IP over serial.
2. Disable the primary network, start the S3 backup AP, and verify connection at `.2` after the primary timeout.
3. Download `http://192.168.4.2/capture`; verify status 200, JPEG decoding, and Content-Length matching the downloaded bytes.
4. Verify `/` returns readiness text, POST `/capture` returns 405 with Allow: GET, and an unknown path returns 404.
5. Trigger tablet fallback on the S3 and confirm it receives and processes the image; check that no other AP client has `.2`.
