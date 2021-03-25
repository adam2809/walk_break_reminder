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
	log_i("Trying to connect to a saved wifi network");

	DynamicJsonBuffer jsonBuffer(capacity);
	JsonObject& config = loadConfig(jsonBuffer);

	int found = scanForSavedWifiNetworks(config["wifi"].as<JsonArray&>());
	if(found == -1){
		log_i("No available saved networks");
		return false;
	}
	log_i("Found (index=%d) network with ssid: %s",found,config["wifi"].as<JsonArray&>()[found].as<JsonObject&>()["ssid"].as<String>());
	connectToWiFi(config["wifi"][found]["ssid"],config["wifi"][found]["password"]);

	jsonBuffer.clear();

	return true;
}

void setup() {
	Serial.begin(115200);
	configureMPU(1); 

	if(!SPIFFS.begin()){ 
		log_e("An Error has occurred while mounting SPIFFS");  
	}
    
	WiFi.mode(WIFI_STA);
  	WiFi.disconnect();

	WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
		log_i("WiFi lost connection. Reason: %d",info.disconnected.reason);
		if(info.disconnected.reason == 8 || walkStartMilis != -1){
			return;
		}
		walkStartMilis = millis();
	}, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

	if(attemptConnectionToSavedWifi()){
		startServer();
	}
}

void loop() {
	unsigned long currentMillis = millis();

	if(walkStartMilis != -1 && currentMillis - prevMillis >= WIFI_CONN_POLLING_INTERVAL) {
		if(attemptConnectionToSavedWifi()){
			int duration = millis() - walkStartMilis;
			log_i("Finished walk duration was: %d",duration);
			createStravaActivity(duration);
			walkStartMilis = -1;
		}
		prevMillis = millis();
	}
}