
#include <Arduino.h>
#include "camera.h"

// ESP32-CAM runs as a separate camera device.
// Set this URL to its actual stream address.
const char* CAMERA_STREAM_URL = "http://192.168.4.2:81/stream";

void cameraBegin() {
    Serial.println("Camera stream integration configured.");
    Serial.println(CAMERA_STREAM_URL);
}
