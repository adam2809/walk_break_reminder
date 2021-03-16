#include "onboard_web_server.hpp"
#include "motion_detection.hpp"

#define ACCEPT_HEADER "Accept", "*/*"
#define ACCEPT_ENCODING_HEADER "Accept-Encoding", "gzip, deflate"
#define CONNECTION_HEADER "Connection", "keep-alive"
#define HOST_HEADER "Host", "192.168.0.87"
#define USER_AGENT_HEADER "User-Agent", "ESP32StravaTracker/0.0.1"
#define CONTENT_TYPE_HEADER "Content-Type", "application/xml"


char testGpx[4096];

void setup() {
	Serial.begin(115200);
  	configureMPU(1); 

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

	String ssid;
	config["wifi"][0]["ssid"].printTo(ssid);
	char* currSsid = (char*) malloc(32*(sizeof(char)));
	strcpy(currSsid,ssid.c_str());

	Serial.print("Setting AP…");
	WiFi.softAP(config["ap"]["ssid"], config["ap"]["password"]);

	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");Serial.println(IP);

	jsonBuffer.clear();
	startServer(currSsid);
}

void loop() {
}