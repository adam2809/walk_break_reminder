#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
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

DynamicJsonBuffer jsonBuffer(JSON_ARRAY_SIZE(3) + MAX_WIFI_NETWORKS*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(2));
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
void notFound(AsyncWebServerRequest *request){
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}

void startServer(JsonObject& config){
	server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
		Serial.println("Got GET on /");
		request->send_P(200, "text/html",config_html);
	});

	server.on("/wifi", HTTP_GET, [&](AsyncWebServerRequest *request){
		Serial.println("Got GET on /wifi");
		char savedWiFiArr[1024];
		config["wifi"].printTo(savedWiFiArr);
		request->send_P(200, "application/json",savedWiFiArr);
	});

	AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/wifi", [&](AsyncWebServerRequest *request, JsonVariant &jsonVar) {
		Serial.println("Got POST on /wifi");
		JsonObject& json = jsonVar.as<JsonObject&>();

		if(json.size() != 2){
			request->send_P(400, "application/json","Error: wrong field count");
		}

		if(
			!json.containsKey("ssid") ||
			!json.containsKey("password")
		){
			request->send_P(400, "application/json","Error: missing required field ssid or password");
			return;
		}
		const char* ssidToAdd = json["ssid"];
		const char* passwordToAdd = json["password"];

		Serial.println("Adding wifi config:");
		json.printTo(Serial);Serial.println();
		// StaticJsonBuffer<JSON_OBJECT_SIZE(3)> tmpJsonBuffer;
		// JsonObject& newWiFi = tmpJsonBuffer.createObject();
		// newWiFi.set("ssid", ssidToAdd);
		// newWiFi.set("password",passwordToAdd);
		// newWiFi.printTo(Serial);Serial.println();

		JsonArray& currWiFiNetworks = config["wifi"].as<JsonArray&>();
		currWiFiNetworks.add(json);
		char wifiArrJsonString[1024];
		currWiFiNetworks.printTo(wifiArrJsonString);

		char configJsonString[1024];
		config.printTo(configJsonString);
		writeFile(SPIFFS, "/config.json", configJsonString);

		request->send_P(200, "application/json",wifiArrJsonString);
	});
	handler->setMethod(HTTP_POST);
	server.addHandler(handler);

	server.on("/wifi", HTTP_DELETE, [&](AsyncWebServerRequest *request){
		Serial.println("Got DELETE on /wifi");
		if(!request->hasParam("ssid")){
			request->send_P(400, "application/json","Error: missing required parameter ssid");
			return;
		}
		const char* ssidToDelete = request->getParam("ssid")->value().c_str();
		JsonArray& currWiFiNetworks = config["wifi"].as<JsonArray&>();

		for (int i=0; i<currWiFiNetworks.size(); i++) {
			if(currWiFiNetworks[i]["ssid"] == ssidToDelete){
				Serial.print("Removing ");Serial.println(ssidToDelete);
				currWiFiNetworks.remove(i);
				break;
			}
		}

		char configJsonString[1024];
		config.printTo(configJsonString);
		request->send_P(200, "application/json",configJsonString);
	});

	server.onNotFound(notFound);

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