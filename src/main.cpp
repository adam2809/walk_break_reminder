#include "onboard_web_server.hpp"
#include "motion_detection.hpp"

#define ACCEPT_HEADER "Accept", "*/*"
#define ACCEPT_ENCODING_HEADER "Accept-Encoding", "gzip, deflate"
#define CONNECTION_HEADER "Connection", "keep-alive"
#define HOST_HEADER "Host", "192.168.0.87"
#define USER_AGENT_HEADER "User-Agent", "ESP32StravaTracker/0.0.1"
#define CONTENT_TYPE_HEADER "Content-Type", "application/xml"
#define WIFI_CONN_POLLING_INTERVAL 10000


char testGpx[4096];
unsigned long prevMillis = 8000;

void setup() {
	Serial.begin(115200);
  	configureMPU(1); 

	String readBuffer;

	if(!SPIFFS.begin()){ 
		Serial.println("An Error has occurred while mounting SPIFFS");  
	}
    
	WiFi.mode(WIFI_AP_STA); 
}

void loop() {
	unsigned long currentMillis = millis();

	if (currentMillis - prevMillis >= WIFI_CONN_POLLING_INTERVAL) {
		if(WiFi.status() != WL_CONNECTED){
			Serial.println("Trying to connect to a saved wifi network");

			DynamicJsonBuffer jsonBuffer(capacity);
			JsonObject& config = loadConfig(jsonBuffer);

			int found = scanForSavedWifiNetworks(config["wifi"].as<JsonArray&>());
			if(found == -1){
				Serial.println("No available saved networks");
				prevMillis = currentMillis;
				return;
			}
			Serial.print("Found network with ssid: ");Serial.println(config["wifi"].as<JsonArray&>()[found].as<JsonObject&>()["ssid"].as<String>());
			connectToWiFi(config["wifi"][found]["ssid"],config["wifi"][found]["password"]);

			jsonBuffer.clear();
		}
		prevMillis = currentMillis;
	}

}