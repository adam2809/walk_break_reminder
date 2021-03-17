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
unsigned long walkStartMilis = -1;

bool attemptConnectionToSavedWifi(){
	log_d("Trying to connect to a saved wifi network");

	DynamicJsonBuffer jsonBuffer(capacity);
	JsonObject& config = loadConfig(jsonBuffer);

	int found = scanForSavedWifiNetworks(config["wifi"].as<JsonArray&>());
	if(found == -1){
		log_d("No available saved networks");
		return false;
	}
	log_d("Found (index=%d) network with ssid: %s",found,config["wifi"].as<JsonArray&>()[found].as<JsonObject&>()["ssid"].as<String>());
	connectToWiFi(config["wifi"][found]["ssid"],config["wifi"][found]["password"]);

	jsonBuffer.clear();

	return true;
}

void setup() {
	Serial.begin(115200);
  	// configureMPU(1);

	if(!SPIFFS.begin()){ 
		log_e("An Error has occurred while mounting SPIFFS");  
	}
    
	WiFi.mode(WIFI_AP_STA);
	attemptConnectionToSavedWifi();
}

void loop() {
	unsigned long currentMillis = millis();

	if(walkStartMilis == -1){
		if(WiFi.status() != WL_CONNECTED){
			walkStartMilis = millis();
			WiFi.disconnect();
			log_i("Started walk at: %d",walkStartMilis);
		}
	}else{
		if(currentMillis - prevMillis >= WIFI_CONN_POLLING_INTERVAL) {
			if(attemptConnectionToSavedWifi()){
				log_i("Finished walk duration was: %d",millis() - walkStartMilis);
				walkStartMilis = -1;
			}
			prevMillis = millis();
		}
	}
	WiFi.status();
}