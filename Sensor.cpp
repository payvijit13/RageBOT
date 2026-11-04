
#include <Arduino.h>
#include "Sensor.h"

#ifndef TRIG_PIN
#define TRIG_PIN 18
#endif

#ifndef ECHO_PIN
#define ECHO_PIN 19
#endif

#ifndef OBSTACLE_DISTANCE
#define OBSTACLE_DISTANCE 20.0
#endif

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

    unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if (duration == 0) return -1;

    return duration * 0.0343f / 2.0f;
}

bool sensorObstacleDetected() {
    float distance = sensorReadDistance();

    return distance > 0 && distance <= OBSTACLE_DISTANCE;
}
