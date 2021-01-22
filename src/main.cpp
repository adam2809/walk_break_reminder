#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "file_utils.hpp"
#include "networking.hpp"
#include "html_pages.hpp"

#define ACCEPT_HEADER "Accept", "*/*"
#define ACCEPT_ENCODING_HEADER "Accept-Encoding", "gzip, deflate"
#define CONNECTION_HEADER "Connection", "keep-alive"
#define HOST_HEADER "Host", "192.168.0.87"
#define USER_AGENT_HEADER "User-Agent", "ESP32StravaTracker/0.0.1"
#define CONTENT_TYPE_HEADER "Content-Type", "application/xml"

#define MAX_WIFI_NETWORKS 10

DynamicJsonBuffer jsonBuffer(
	JSON_OBJECT_SIZE(2) + 
	JSON_ARRAY_SIZE(MAX_WIFI_NETWORKS) + 
	JSON_OBJECT_SIZE(2)
);
char testGpx[4096];
HTTPClient http;
AsyncWebServer server(80);

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

String configHtmlTemplateProcessor(const String& var){

}

void startServer(JsonObject& config){
	server.on("/wifi", HTTP_GET, [&](AsyncWebServerRequest *request){
		char configJsonString[1024];
		config.printTo(configJsonString);
		// request->send_P(200, "text/html",config_html,configHtmlTemplateProcessor);
		request->send_P(200, "application/json",configJsonString);
	});

	server.on("/wifi", HTTP_PUT, [&](AsyncWebServerRequest *request){
		if(
			!request->hasParam("ssid") ||
			!request->hasParam("password")
		){
			request->send_P(400, "application/json","Error: missing required parameters");
			return;
		}
		String ssid = request->getParam("ssid")->value();
		String password = request->getParam("password")->value();

		Serial.print("Setting new ssid: ");Serial.print(ssid);Serial.print(" and password: ");Serial.print(password);Serial.println();
		config["wifi"][2]["ssid"] = ssid;
		config["wifi"][2]["password"] = password;

		char configJsonString[1024];
		config.printTo(configJsonString);

		writeFile(SPIFFS, "/config.json", configJsonString);

		request->send_P(200, "application/json",configJsonString);
	});

	server.begin();
}



void setup() {
	Serial.begin(115200);
	String readBuffer;

	if(!SPIFFS.begin()){ 
		Serial.println("An Error has occurred while mounting SPIFFS");  
	}

    if(readFile(SPIFFS, "/config.json",readBuffer)){
		Serial.println("Read config.json file");
    }else{
        Serial.println("Could not load config file");
		return;
	}
    
	JsonObject& config = jsonBuffer.parseObject(readBuffer);
	if (config.success()){
		Serial.println("Parsed saved config:");
		config.printTo(Serial);Serial.println();
	}else{
		Serial.println("Could not parse config.json");
		return;
	}
	
    if(!readFile(SPIFFS, "/test.gpx",readBuffer)){
        Serial.println("Could not test gpx file");
    }
    readBuffer.toCharArray(testGpx,readBuffer.length());

	connectToWiFi(config["wifi"][0]["ssid"],config["wifi"][0]["password"]);

	startServer(config);
}

void loop() {
}