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
<title>RageBOT</title>
<link rel="stylesheet" href="/style.css">
</head>

<body>

<h1>RAGEBOT</h1>
<p>Autonomous Robotics System</p>

<section class="card">
<h2>Camera</h2>
<img id="camera" alt="Camera unavailable">
</section>

<section class="card">
<h2>Robot Status</h2>
<p>Mode: <b id="mode">STOP</b></p>
<p>Movement: <b id="command">STOP</b></p>
<p>Distance: <b id="distance">--</b></p>
<p>Obstacle: <b id="obstacle">--</b></p>
<p id="connection">Connecting...</p>
</section>

<section class="card">
<h2>Autonomous Control</h2>
<button onclick="setMode('AUTO')">START AUTO</button>
<button onclick="setMode('MANUAL')">MANUAL MODE</button>
<button class="stop" onclick="setMode('STOP')">EMERGENCY STOP</button>
</section>

<section class="card">
<h2>Manual Controls</h2>

<button onclick="move('FORWARD')">FORWARD</button>

<div class="row">
<button onclick="move('LEFT')">LEFT</button>
<button onclick="move('STOP')">STOP</button>
<button onclick="move('RIGHT')">RIGHT</button>
</div>

<button onclick="move('BACKWARD')">BACKWARD</button>
</section>

<script src="/script.js"></script>
</body>
</html>
)rawliteral";

const char STYLE_CSS[] PROGMEM = R"rawliteral(
* {
  box-sizing: border-box;
}

body {
  background: #10131a;
  color: white;
  font-family: Arial, sans-serif;
  text-align: center;
  padding: 12px;
}

h1 {
  color: #55d6ff;
}

.card {
  background: #202633;
  padding: 16px;
  border-radius: 12px;
  margin: 15px auto;
  max-width: 500px;
}

button {
  background: #168bd0;
  color: white;
  border: 0;
  border-radius: 8px;
  padding: 15px;
  margin: 5px;
  font-weight: bold;
  min-width: 85px;
}

.stop {
  background: #d43c3c;
}

.row {
  display: flex;
  justify-content: center;
}

#camera {
  width: 100%;
  max-width: 450px;
  background: black;
  border-radius: 8px;
}

#connection {
  color: #55d6ff;
}
)rawliteral";

const char SCRIPT_JS[] PROGMEM = R"rawliteral(
const cameraURL = "http://192.168.4.2:81/stream";

document.getElementById("camera").src = cameraURL;

async function setMode(mode) {
  try {
    const response = await fetch("/mode?set=" + mode);
    const result = await response.text();

    document.getElementById("mode").textContent = result;
  } catch (error) {
    document.getElementById("connection").textContent =
      "Request failed";
  }
}

async function move(direction) {
  try {
    const response = await fetch("/move?dir=" + direction);
    const result = await response.text();

    document.getElementById("command").textContent = result;

    if (!response.ok) {
      document.getElementById("connection").textContent = result;
    }
  } catch (error) {
    document.getElementById("connection").textContent =
      "Movement request failed";
  }
}

async function updateStatus() {
  try {
    const response = await fetch("/status");
    const data = await response.json();

    document.getElementById("mode").textContent = data.mode;
    document.getElementById("command").textContent = data.command;

    document.getElementById("distance").textContent =
      data.sensorValid ? data.distance + " cm" : "No reading";

    document.getElementById("obstacle").textContent =
      !data.sensorValid ? "UNKNOWN" :
      data.obstacle ? "DETECTED" : "CLEAR";

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
  Serial.println(WiFi.softAPIP());
}