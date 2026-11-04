#include <Arduino.h>
#include "../RageBOT_Config.h"
#include "camera.h"

void cameraBegin() {
  Serial.println("ESP32-CAM stream address:");
  Serial.println(CAMERA_STREAM_URL);
}