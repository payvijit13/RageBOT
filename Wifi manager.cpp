
#include <Arduino.h>
#include <WiFi.h>
#include "RageBOT_Config.h"
#include "Wifi manager.h"

WebServer server(80);

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>RageBOT Control</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <h1>RAGEBOT</h1>
  <p class="subtitle">Robot Control Dashboard</p>

  <section class="card">
    <h2>Camera</h2>
    <img id="camera" src="" alt="Camera stream unavailable">
  </section>

  <section class="card">
    <h2>Sensor Status</h2>
    <p>Distance: <strong id="distance">--</strong> cm</p>
    <p>Obstacle: <strong id="obstacle">--</strong></p>
    <p>Movement: <strong id="command">STOP</strong></p>
  </section>

  <section class="card">
    <h2>Movement</h2>
    <div class="controls">
      <button onclick="move('FORWARD')">FORWARD</button>
      <button onclick="move('LEFT')">LEFT</button>
      <button class="stop" onclick="move('STOP')">STOP</button>
      <button onclick="move('RIGHT')">RIGHT</button>
      <button onclick="move('BACKWARD')">BACKWARD</button>
    </div>
  </section>

  <p id="connection">Connecting...</p>

  <script src="/script.js"></script>
</body>
</html>
)rawliteral";

const char STYLE_CSS[] PROGMEM = R"rawliteral(
* {
  box-sizing: border-box;
}

body {
  margin: 0;
  padding: 18px;
  background: #10131a;
  color: white;
  font-family: Arial, sans-serif;
  text-align: center;
}

h1 {
  color: #55d6ff;
  margin-bottom: 5px;
}

.subtitle {
  color: #aaa;
}

.card {
  background: #202633;
  border-radius: 12px;
  padding: 16px;
  margin: 16px auto;
  max-width: 500px;
}

#camera {
  width: 100%;
  max-width: 450px;
  min-height: 100px;
  background: #080a0e;
  border-radius: 8px;
  object-fit: contain;
}

.controls {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 9px;
}

button {
  padding: 16px 5px;
  border: 0;
  border-radius: 8px;
  background: #168bd0;
  color: white;
  font-weight: bold;
  cursor: pointer;
}

button:active {
  background: #075b91;
}

.stop {
  background: #d43c3c;
}

#connection {
  color: #55d6ff;
}
)rawliteral";

const char SCRIPT_JS[] PROGMEM = R"rawliteral(
// Change this to the actual ESP32-CAM stream URL.
const cameraURL = "http://192.168.4.2:81/stream";

document.getElementById("camera").src = cameraURL;

async function move(direction) {
  try {
    const response = await fetch(
      "/move?dir=" + direction
    );

    const result = await response.text();
    document.getElementById("command").textContent = result;
  } catch (error) {
    document.getElementById("connection").textContent =
      "Movement request failed";
  }
}

async function updateStatus() {
  try {
    const response = await fetch("/status");
    const data = await response.json();

    document.getElementById("distance").textContent =
      data.distance < 0 ? "No reading" : data.distance;

    document.getElementById("obstacle").textContent =
      data.obstacle ? "DETECTED" : "CLEAR";

    document.getElementById("command").textContent =
      data.command;

    document.getElementById("connection").textContent =
      "RageBOT connected";
  } catch (error) {
    document.getElementById("connection").textContent =
      "Connection lost";
  }
}

setInterval(updateStatus, 500);
updateStatus();
)rawliteral";

void wifiBegin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });

  server.on("/style.css", HTTP_GET, []() {
    server.send_P(200, "text/css", STYLE_CSS);
  });

  server.on("/script.js", HTTP_GET, []() {
    server.send_P(200, "application/javascript", SCRIPT_JS);
  });

  Serial.println("RageBOT Wi-Fi started");
  Serial.print("Dashboard: http://");
  Serial.println(WiFi.softAPIP());
}
