#pragma once

// PLACEHOLDER GPIOs — replace with actual circuit connections.
#define MOTOR_IN1 25
#define MOTOR_IN2 26
#define MOTOR_IN3 27
#define MOTOR_IN4 14
#define MOTOR_ENA 32
#define MOTOR_ENB 33

#define TRIG_PIN 18
#define ECHO_PIN 19
#define LED_PIN 5

#define OBSTACLE_DISTANCE 25.0f

#define MOTOR_SPEED 180
#define TURN_SPEED 160

#define SENSOR_TIMEOUT_US 30000UL
#define SENSOR_UPDATE_INTERVAL_MS 150UL

#define REVERSE_TIME_MS 350UL
#define TURN_TIME_MS 450UL

#define WIFI_SSID "RageBOT"
#define WIFI_PASSWORD "RageBOT123"

#define CAMERA_STREAM_URL "http://192.168.4.2:81/stream"