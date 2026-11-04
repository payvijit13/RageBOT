
#include <Arduino.h>
#include "RageBOT_Config.h"
#include "Sensor.h"
#include "Wifi manager.h"
#include "Camera/camera.h"

bool obstacleStop = true;
String currentCommand = "STOP";

float distanceCM = -1;
unsigned long lastSensorUpdate = 0;

void stopMotors() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);

  analogWrite(MOTOR_ENA, 0);
  analogWrite(MOTOR_ENB, 0);

  currentCommand = "STOP";
}

void moveForward() {
  if (obstacleStop &&
      distanceCM > 0 &&
      distanceCM <= OBSTACLE_DISTANCE) {
    stopMotors();
    return;
  }

  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN3, HIGH);
  digitalWrite(MOTOR_IN4, LOW);

  analogWrite(MOTOR_ENA, MOTOR_SPEED);
  analogWrite(MOTOR_ENB, MOTOR_SPEED);

  currentCommand = "FORWARD";
}

void moveBackward() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, HIGH);

  analogWrite(MOTOR_ENA, MOTOR_SPEED);
  analogWrite(MOTOR_ENB, MOTOR_SPEED);

  currentCommand = "BACKWARD";
}

void turnLeft() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  digitalWrite(MOTOR_IN3, HIGH);
  digitalWrite(MOTOR_IN4, LOW);

  analogWrite(MOTOR_ENA, TURN_SPEED);
  analogWrite(MOTOR_ENB, TURN_SPEED);

  currentCommand = "LEFT";
}

void turnRight() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, HIGH);

  analogWrite(MOTOR_ENA, TURN_SPEED);
  analogWrite(MOTOR_ENB, TURN_SPEED);

  currentCommand = "RIGHT";
}

void handleMove() {
  if (!server.hasArg("dir")) {
    server.send(400, "text/plain", "Missing direction");
    return;
  }

  String dir = server.arg("dir");
  dir.toUpperCase();

  if (dir == "STOP") {
    stopMotors();
  } else if (dir == "FORWARD") {
    moveForward();
  } else if (dir == "BACKWARD") {
    moveBackward();
  } else if (dir == "LEFT") {
    turnLeft();
  } else if (dir == "RIGHT") {
    turnRight();
  } else {
    server.send(400, "text/plain", "Invalid direction");
    return;
  }

  server.send(200, "text/plain", currentCommand);
}

void handleStatus() {
  bool obstacle =
    distanceCM > 0 && distanceCM <= OBSTACLE_DISTANCE;

  String json = "{";
  json += "\"distance\":" + String(distanceCM, 1) + ",";
  json += "\"obstacle\":" + String(obstacle ? "true" : "false") + ",";
  json += "\"command\":\"" + currentCommand + "\",";
  json += "\"wifi\":\"connected\"";
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);

  pinMode(MOTOR_ENA, OUTPUT);
  pinMode(MOTOR_ENB, OUTPUT);

  pinMode(LED_PIN, OUTPUT);

  stopMotors();
  sensorBegin();

  cameraBegin();
  wifiBegin();

  server.on("/move", HTTP_GET, handleMove);
  server.on("/status", HTTP_GET, handleStatus);

  server.begin();

  Serial.println();
  Serial.println("RAGEBOT ONLINE");
  Serial.println("Dashboard: http://192.168.4.1");
}

void loop() {
  server.handleClient();

  if (millis() - lastSensorUpdate >= 250) {
    lastSensorUpdate = millis();

    distanceCM = sensorReadDistance();

    bool obstacle =
      distanceCM > 0 && distanceCM <= OBSTACLE_DISTANCE;

    digitalWrite(LED_PIN, obstacle ? HIGH : LOW);

    if (obstacle &&
        obstacleStop &&
        currentCommand == "FORWARD") {
      stopMotors();
    }
  }
}
