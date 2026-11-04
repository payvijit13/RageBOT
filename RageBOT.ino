#include <Arduino.h>
#include "RageBOT_Config.h"
#include "Sensor.h"
#include "Wifi manager.h"
#include "Camera/camera.h"

enum AutoState {
  WAIT_SENSOR,
  DRIVING_FORWARD,
  REVERSING,
  TURNING
};

String controlMode = "STOP";
String currentCommand = "STOP";

AutoState autoState = WAIT_SENSOR;

float distanceCM = -1;

unsigned long lastSensorUpdate = 0;
unsigned long stateStart = 0;

bool turnLeftNext = true;

void stopMotors() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);

  analogWrite(MOTOR_ENA, 0);
  analogWrite(MOTOR_ENB, 0);

  currentCommand = "STOP";
}

bool sensorBlocked() {
  return distanceCM < 0 ||
         distanceCM <= OBSTACLE_DISTANCE;
}

void driveForward() {
  if (sensorBlocked()) {
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

void driveBackward() {
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

void startAvoidance() {
  stopMotors();

  driveBackward();

  autoState = REVERSING;
  stateStart = millis();
}

void startAuto() {
  stopMotors();

  controlMode = "AUTO";
  autoState = WAIT_SENSOR;
}

void startManual() {
  stopMotors();

  controlMode = "MANUAL";
}

void emergencyStop() {
  controlMode = "STOP";
  autoState = WAIT_SENSOR;

  stopMotors();
}

void handleMode() {
  if (!server.hasArg("set")) {
    server.send(400, "text/plain", "Missing mode");
    return;
  }

  String requested = server.arg("set");
  requested.toUpperCase();

  if (requested == "AUTO") {
    startAuto();
  }
  else if (requested == "MANUAL") {
    startManual();
  }
  else if (requested == "STOP") {
    emergencyStop();
  }
  else {
    server.send(400, "text/plain", "Invalid mode");
    return;
  }

  server.send(200, "text/plain", controlMode);
}

void handleMove() {
  if (controlMode != "MANUAL") {
    server.send(409, "text/plain", "Enable MANUAL mode first");
    return;
  }

  if (!server.hasArg("dir")) {
    server.send(400, "text/plain", "Missing direction");
    return;
  }

  String dir = server.arg("dir");
  dir.toUpperCase();

  if (dir == "STOP") {
    stopMotors();
  }
  else if (dir == "FORWARD") {
    if (sensorBlocked()) {
      stopMotors();
      server.send(409, "text/plain", "Blocked: sensor or obstacle");
      return;
    }

    driveForward();
  }
  else if (dir == "BACKWARD") {
    driveBackward();
  }
  else if (dir == "LEFT") {
    turnLeft();
  }
  else if (dir == "RIGHT") {
    turnRight();
  }
  else {
    server.send(400, "text/plain", "Invalid direction");
    return;
  }

  server.send(200, "text/plain", currentCommand);
}

void handleStatus() {
  bool valid = distanceCM >= 0;
  bool obstacle = valid &&
                  distanceCM <= OBSTACLE_DISTANCE;

  String json = "{";

  json += "\"mode\":\"" + controlMode + "\",";
  json += "\"command\":\"" + currentCommand + "\",";
  json += "\"distance\":" + String(distanceCM, 1) + ",";
  json += "\"sensorValid\":";
  json += valid ? "true," : "false,";
  json += "\"obstacle\":";
  json += obstacle ? "true" : "false";

  json += "}";

  server.send(200, "application/json", json);
}

void autonomousLoop() {
  if (controlMode != "AUTO") {
    return;
  }

  // Invalid readings always stop the robot.
  if (distanceCM < 0) {
    stopMotors();
    autoState = WAIT_SENSOR;
    return;
  }

  switch (autoState) {

    case WAIT_SENSOR:

      if (distanceCM > OBSTACLE_DISTANCE) {
        driveForward();
        autoState = DRIVING_FORWARD;
      } else {
        startAvoidance();
      }

      break;

    case DRIVING_FORWARD:

      if (distanceCM <= OBSTACLE_DISTANCE) {
        startAvoidance();
      }

      break;

    case REVERSING:

      if (millis() - stateStart >= REVERSE_TIME_MS) {

        stopMotors();

        if (turnLeftNext) {
          turnLeft();
        } else {
          turnRight();
        }

        turnLeftNext = !turnLeftNext;

        autoState = TURNING;
        stateStart = millis();
      }

      break;

    case TURNING:

      if (millis() - stateStart >= TURN_TIME_MS) {

        stopMotors();

        autoState = WAIT_SENSOR;
      }

      break;
  }
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

  emergencyStop();

  sensorBegin();
  cameraBegin();
  wifiBegin();

  server.on("/mode", HTTP_GET, handleMode);
  server.on("/move", HTTP_GET, handleMove);
  server.on("/status", HTTP_GET, handleStatus);

  server.begin();

  Serial.println();
  Serial.println("RAGEBOT ONLINE");
  Serial.println("Autonomous controller ready");
  Serial.println("Dashboard: http://192.168.4.1");
}

void loop() {
  server.handleClient();

  if (millis() - lastSensorUpdate >=
      SENSOR_UPDATE_INTERVAL_MS) {

    lastSensorUpdate = millis();

    distanceCM = sensorReadDistance();

    digitalWrite(
      LED_PIN,
      sensorBlocked() ? HIGH : LOW
    );
  }

  autonomousLoop();
}