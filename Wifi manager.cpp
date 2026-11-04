
#include <Arduino.h>
#include <WiFi.h>
#include "Wifi manager.h"

WebServer server(80);

const char* AP_SSID = "RageBOT";
const char* AP_PASSWORD = "RageBOT123";

void wifiBegin() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    Serial.println("RageBOT WiFi started");
    Serial.print("Dashboard: http://");
    Serial.println(WiFi.softAPIP());
}
