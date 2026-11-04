
const cameraURL = "http://192.168.4.2:81/stream";

document.getElementById("camera").src = cameraURL;

async function move(direction) {
    try {
        const response = await fetch(
            `/move?dir=${encodeURIComponent(direction)}`
        );
        if (!response.ok) throw new Error("Command failed");
    } catch (error) {
        document.getElementById("connection").textContent =
            "Command connection failed";
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
