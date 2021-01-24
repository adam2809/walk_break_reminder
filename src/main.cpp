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
#define JSON_STRING_BUFFER_LENGTH 1024

#define MAX_WIFI_NETWORKS 10

const int capacity = JSON_ARRAY_SIZE(3) + MAX_WIFI_NETWORKS*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(2);
DynamicJsonBuffer jsonBufferToDelete(capacity);
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

JsonObject& loadConfig(DynamicJsonBuffer& jsonBuffer){
	String readBuffer;

	if(readFile(SPIFFS, "/config.json",readBuffer)){
		Serial.println("Read config.json file");
    }else{
        Serial.println("Could not load config file");
	}
    
	JsonObject& loadedConfig = jsonBuffer.parseObject(readBuffer);
	if (loadedConfig.success()){
		Serial.println("Parsed saved config:");
		loadedConfig.printTo(Serial);Serial.println();
	}else{
		Serial.println("Could not parse config.json");
	}

	return loadedConfig;
}

void startServer(JsonObject& configToDelete){
	server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
		Serial.println("Got GET on /");
		request->send_P(200, "text/html",config_html);
	});

	server.on("/wifi", HTTP_GET, [&](AsyncWebServerRequest *request){
		Serial.println("Got GET on /wifi");
		DynamicJsonBuffer jsonBuffer(capacity);
		JsonObject& config = loadConfig(jsonBuffer);

		char savedWiFiArr[JSON_STRING_BUFFER_LENGTH];
		config["wifi"].printTo(savedWiFiArr);
		request->send_P(200, "application/json",savedWiFiArr);
		jsonBuffer.clear();
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

		Serial.println("Adding wifi config:");
		json.printTo(Serial);Serial.println();

		DynamicJsonBuffer jsonBuffer(capacity);
		JsonObject& config = loadConfig(jsonBuffer);

		JsonArray& currWiFiNetworks = config["wifi"].as<JsonArray&>();
		currWiFiNetworks.add(json);
		char wifiArrJsonString[JSON_STRING_BUFFER_LENGTH];
		currWiFiNetworks.printTo(wifiArrJsonString);

		char configJsonString[JSON_STRING_BUFFER_LENGTH];
		config.printTo(configJsonString);
		writeFile(SPIFFS, "/config.json", configJsonString);

		request->send_P(200, "application/json",wifiArrJsonString);
		jsonBuffer.clear();
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

		DynamicJsonBuffer jsonBuffer(capacity);
		JsonObject& config = loadConfig(jsonBuffer);
		JsonArray& currWiFiNetworks = config["wifi"].as<JsonArray&>();

		for (int i=0; i<currWiFiNetworks.size(); i++) {
			if(currWiFiNetworks[i]["ssid"] == ssidToDelete){
				Serial.print("Removing ");Serial.println(ssidToDelete);
				currWiFiNetworks.remove(i);
				break;
			}
		}

		char configJsonString[JSON_STRING_BUFFER_LENGTH];
		config.printTo(configJsonString);
		writeFile(SPIFFS, "/config.json", configJsonString);

		char wifiArrJsonString[JSON_STRING_BUFFER_LENGTH];
		currWiFiNetworks.printTo(wifiArrJsonString);

		request->send_P(200, "application/json",wifiArrJsonString);
		jsonBuffer.clear();
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
    
	JsonObject& config = jsonBufferToDelete.parseObject(readBuffer);
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