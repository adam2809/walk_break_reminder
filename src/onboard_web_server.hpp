#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "file_utils.hpp"
#include "networking.hpp"
#include "html_pages.hpp"
#include <driver/adc.h>
#include "time.h"
#include <ESP32Time.h>

#define JSON_STRING_BUFFER_LENGTH 1024
#define MAX_WIFI_NETWORKS 10
#define PORT 80

const int capacity = JSON_ARRAY_SIZE(3) + MAX_WIFI_NETWORKS*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(2);

void startServer(ESP32Time* _rtc);
void notFound(AsyncWebServerRequest *request);
String templateProcessor(const String& var);
void getAllWifiNetworks(AsyncWebServerRequest *request);
void addNewWifiNetwork(AsyncWebServerRequest *request, JsonVariant &jsonVar);
void deleteWifiNetwork(AsyncWebServerRequest *request);
void submitEspCode(AsyncWebServerRequest *request);
bool createStravaWalkActivity(int walkDuration);
long getCurrentTimestamp();
long getCurrentEpoch();

struct Retryer{
	bool(& retryFun)();
	unsigned long prevRetryMillis;
	unsigned long retryInterval;
};