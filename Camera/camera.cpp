

#include <Arduino.h>
#include "camera.h"

const char* CAMERA_STREAM_URL =
  "http://192.168.4.2:81/stream";

void cameraBegin() {
  Serial.println("Camera integration configured.");
  Serial.print("Expected camera stream: ");
  Serial.println(CAMERA_STREAM_URL);
}

