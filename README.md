
# RageBOT

ESP32-based robotics project with:
- L298N motor control
- HC-SR04 distance sensor
- ESP32-CAM stream integration
- Web-based movement dashboard
- Obstacle detection

## Wi-Fi

Access point:
- SSID: RageBOT
- Password: RageBOT123

Dashboard:
- http://192.168.4.1

## Important configuration

Check the GPIO assignments in RageBOT.ino and Sensor.cpp against the actual circuit before connecting power.

The ESP32-CAM must have a working camera streaming server. Configure its stream URL in Web/script.js and Camera/camera.cpp.

The web files must be served by the ESP32 before the dashboard can load.
