#include <Arduino.h>
#include "RageBOT_Config.h"
#include "Sensor.h"

void sensorBegin() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);
}

float sensorReadDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    SENSOR_TIMEOUT_US
  );

  if (duration == 0) {
    return -1.0f;
  }

  return duration * 0.0343f / 2.0f;
}

bool sensorObstacleDetected() {
  float distance = sensorReadDistance();

  return distance < 0 ||
         distance <= OBSTACLE_DISTANCE;
}