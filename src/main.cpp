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
char testGpx[4096];
HTTPClient http;
AsyncWebServer server(80);
char currSsid[32];

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
        "",
        uploadActivityHeaders,
        6,
		testGpx
    );
}
void notFound(AsyncWebServerRequest *request){
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}

String templateProcessor(const String& var){
	if(var == "CURRENT_WIFI_SSID"){
		return currSsid;
	}
	return String();
}

JsonObject& parseJson(DynamicJsonBuffer& jsonBuffer,String json){
	JsonObject& parsedJson = jsonBuffer.parseObject(json);

	if (parsedJson.success()){
		Serial.println("Parsed json:");
		parsedJson.printTo(Serial);Serial.println();
	}else{
		Serial.println("Could not parse json");
	}

	return parsedJson;
}

JsonObject& loadConfig(DynamicJsonBuffer& jsonBuffer){
	String readBuffer;
	readFile(SPIFFS, "/config.json",readBuffer);
	return parseJson(jsonBuffer,readBuffer);
}



void startServer(){
	server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
		Serial.println("Got GET on /");
		request->send_P(200, "text/html",config_html,templateProcessor);
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

	server.on("/submit_esp_code", HTTP_POST, [&](AsyncWebServerRequest *request){
		Serial.println("Got POST on /submit_esp_code");
		if(!request->hasParam("esp_code")){
			request->send_P(400, "application/json","Error: missing required parameter esp_code");
			return;
		}
		const char* espCode = request->getParam("esp_code")->value().c_str();

		http.begin(SUPPORT_SERVER_URL + String("/tokens?esp_code=") + espCode);
		int httpResponseCode = http.GET();
		if (httpResponseCode < 0){
			request->send_P(500, "text/plain","Error while making request for tokens");
		}
		
		String resJson = http.getString();

		if(httpResponseCode == 200){
			DynamicJsonBuffer currConfigJsonBuffer(capacity);
			DynamicJsonBuffer responseJsonBuffer(capacity);

			JsonObject& config = loadConfig(currConfigJsonBuffer);
			JsonObject& response = parseJson(responseJsonBuffer,resJson);
			
			if(!(response.containsKey("refresh_token") && response.containsKey("access_token"))){
				request->send_P(500, "text/plain","The tokens request response does not contain required data");
				return;
			}

			config["refresh_token"] = response["refresh_token"];
			config["access_token"] = response["access_token"];

			char configJsonString[JSON_STRING_BUFFER_LENGTH];
			config.printTo(configJsonString);
			writeFile(SPIFFS, "/config.json", configJsonString);
			currConfigJsonBuffer.clear();
			responseJsonBuffer.clear();
		}

		request->send_P(httpResponseCode, "application/json",resJson.c_str());
		http.end();
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

    readFile(SPIFFS, "/config.json",readBuffer);
    
	DynamicJsonBuffer jsonBuffer(capacity);
	JsonObject& config = jsonBuffer.parseObject(readBuffer);
	if (config.success()){
		Serial.println("Parsed saved config:");
		config.printTo(Serial);Serial.println();
	}else{
		Serial.println("Could not parse config.json");
		return;
	}
	
	WiFi.mode(WIFI_AP_STA); 
	connectToWiFi(config["wifi"][0]["ssid"],config["wifi"][0]["password"]);
	config["wifi"][0]["ssid"].printTo(currSsid);

	Serial.print("Setting AP…");
	WiFi.softAP(config["ap"]["ssid"], config["ap"]["password"]);

	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");Serial.println(IP);

	jsonBuffer.clear();
	startServer();
}

void loop() {
}