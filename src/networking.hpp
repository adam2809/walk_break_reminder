#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define SUPPORT_SERVER_URL "http://192.168.0.87:8000"

void connectToWiFi(const char* ssid,const char* password);
int scanForSavedWifiNetworks(JsonArray& savedWiFiNetworks);
