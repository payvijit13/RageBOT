
const cameraURL = "http://192.168.4.2:81/stream";

document.getElementById("camera").src = cameraURL;

async function move(direction) {
  try {
    const response = await fetch("/move?dir=" + direction);
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
