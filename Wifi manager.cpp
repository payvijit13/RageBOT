
#include <Arduino.h>
#include <WiFi.h>
#include "Wifi manager.h"

WebServer server(80);

const char* AP_SSID = "RageBOT";
const char* AP_PASSWORD = "RageBOT123";

// Website HTML
const char INDEX_HTML[] PROGMEM = R"RAGEBOT(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>RageBOT Control</title>
<link rel="stylesheet" href="/style.css">
</head>
<body>
<header>
<h1>RageBOT</h1>
<p id="connection">Connecting...</p>
</header>

<main>
<section class="panel">
<h2>Camera</h2>
<img id="camera" alt="Camera stream unavailable">
<p id="cameraStatus">Waiting for camera...</p>
</section>

<section class="panel">
<h2>Distance Sensor</h2>
<div id="distance">-- cm</div>
<p id="obstacle">Waiting for sensor...</p>
</section>

<section class="panel">
<h2>Movement</h2>
<div class="controls">
<button onclick="move('FORWARD')">▲</button>
<button onclick="move('LEFT')">◀</button>
<button onclick="move('STOP')" class="stop">STOP</button>
<button onclick="move('RIGHT')">▶</button>
<button onclick="move('BACKWARD')">▼</button>
</div>
</section>
</main>
<script src="/script.js"></script>
</body>
</html>
)RAGEBOT";

// Website CSS
const char STYLE_CSS[] PROGMEM = R"RAGEBOT(
* { box-sizing: border-box; }

body {
    margin: 0;
    padding: 16px;
    background: #10131b;
    color: white;
    font-family: Arial, sans-serif;
}

header {
    text-align: center;
    padding: 15px;
}

.panel {
    background: #202633;
    padding: 18px;
    border-radius: 14px;
    margin: 15px auto;
    max-width: 500px;
    text-align: center;
}

#camera {
    width: 100%;
    border-radius: 10px;
    background: black;
    min-height: 150px;
    object-fit: contain;
}

#distance {
    font-size: 40px;
    font-weight: bold;
}

.controls {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 12px;
    max-width: 300px;
    margin: auto;
}

button {
    padding: 18px 8px;
    font-size: 20px;
    border: 0;
    border-radius: 10px;
    background: #3b82f6;
    color: white;
    font-weight: bold;
}

.stop { background: #dc2626; }
)RAGEBOT";

// Website JavaScript
const char SCRIPT_JS[] PROGMEM = R"RAGEBOT(
const cameraURL = "http://192.168.4.2:81/stream";

const camera = document.getElementById("camera");
camera.src = cameraURL;

camera.onload = () => {
    document.getElementById("cameraStatus").textContent =
        "Camera connected";
};

camera.onerror = () => {
    document.getElementById("cameraStatus").textContent =
        "Camera unavailable";
};

async function move(direction) {
    try {
        const response = await fetch(
            `/move?dir=${encodeURIComponent(direction)}`
        );

        if (!response.ok) throw new Error("Command failed");
    } catch (error) {
        document.getElementById("connection").textContent =
            "Command failed";
    }
}

async function updateStatus() {
    try {
        const response = await fetch("/status");

        if (!response.ok) throw new Error("Status unavailable");

        const data = await response.json();

        document.getElementById("connection").textContent =
            "ESP32 connected";

        document.getElementById("distance").textContent =
            data.distance < 0 ? "-- cm" : `${data.distance} cm`;

        document.getElementById("obstacle").textContent =
            data.obstacle ? "Obstacle detected" : "Path clear";

    } catch (error) {
        document.getElementById("connection").textContent =
            "ESP32 disconnected";
    }
}

setInterval(updateStatus, 500);
updateStatus();
)RAGEBOT";

void wifiBegin() {
    WiFi.mode(WIFI_AP);

    if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
        Serial.println("WiFi AP failed!");
        return;
    }

    server.on("/", HTTP_GET, []() {
        server.send_P(200, "text/html", INDEX_HTML);
    });

    server.on("/style.css", HTTP_GET, []() {
        server.send_P(200, "text/css", STYLE_CSS);
    });

    server.on("/script.js", HTTP_GET, []() {
        server.send_P(200, "application/javascript", SCRIPT_JS);
    });

    Serial.println("RageBOT WiFi started");
    Serial.print("Dashboard: http://");
    Serial.println(WiFi.softAPIP());
}
