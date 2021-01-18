#include <Arduino.h>
#include <ArduinoJson.h>
#include "file_utils.hpp"
#include "networking.hpp"

#define ACCEPT_HEADER "Accept", "*/*"
#define ACCEPT_ENCODING_HEADER "Accept-Encoding", "gzip, deflate"
#define CONNECTION_HEADER "Connection", "keep-alive"
#define HOST_HEADER "Host", "192.168.0.87"
#define USER_AGENT_HEADER "User-Agent", "ESP32StravaTracker/0.0.1"
#define CONTENT_TYPE_HEADER "Content-Type", "application/xml"

StaticJsonBuffer<1024> jsonBuffer;
char testGpx[4096];
HTTPClient http;

void uploadTestGpx(){
	String uploadActivityHeaders[] = {
		ACCEPT_HEADER,
		ACCEPT_ENCODING_HEADER,
		CONNECTION_HEADER,
		CONTENT_TYPE_HEADER,
		HOST_HEADER,
		USER_AGENT_HEADER
	};


	Serial.println("Performing strava request");
	performSupportServerRequest(
		http,
        "POST",
        "/uploads",
        testGpx,
        uploadActivityHeaders,
        6
    );
}

void setup() {
	Serial.begin(115200);
	String readBuffer;

	if(!SPIFFS.begin()){ 
		Serial.println("An Error has occurred while mounting SPIFFS");  
	}

    if(!readFile(SPIFFS, "/config.json",readBuffer)){
        Serial.println("Could not load config file");
    }
	Serial.println(readBuffer);
    JsonObject& config = jsonBuffer.parseObject(readBuffer);

    if(!readFile(SPIFFS, "/test.gpx",readBuffer)){
        Serial.println("Could not test gpx file");
    }
    readBuffer.toCharArray(testGpx,readBuffer.length());

	connectToWiFi(config["ssid"],config["password"]);
}

void loop() {
}