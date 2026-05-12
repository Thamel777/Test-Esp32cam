# Test-Esp32cam

This repository contains C++ code for an ESP32-CAM (AI Thinker module) project using Arduino framework/PlatformIO. The project connects to a WiFi network, initializes the camera (using RGB565 pixel format with VGA frame size), and hosts a web server that serves captured images as JPEGs. 

## Features
- Captures frames from the ESP32-CAM module
- Converts RGB565 frames to JPEG format
- Hosts an HTTP server on port 80 to view the camera stream via WiFi

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
