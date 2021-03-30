#include "onboard_web_server.hpp"
#include "motion_detection.hpp"

#define ACCEPT_HEADER "Accept", "*/*"
#define ACCEPT_ENCODING_HEADER "Accept-Encoding", "gzip, deflate"
#define CONNECTION_HEADER "Connection", "keep-alive"
#define HOST_HEADER "Host", "192.168.0.87"
#define USER_AGENT_HEADER "User-Agent", "ESP32StravaTracker/0.0.1"
#define CONTENT_TYPE_HEADER "Content-Type", "application/xml"
#define WIFI_CONN_POLLING_INTERVAL 10000
#define MAX_RETRY_COUNT 10

char testGpx[4096];
unsigned long prevMillis = 8000;
unsigned long walkStartMilis = -1;
String walkStartTimestamp;
ESP32Time rtc;
int rssiEstimateFailCount = 0;


Retryer createWalkActivityRetry = {
	[] () -> bool {
		return true;
	},
	millis(),
	10*1000,
	20,
	0
};
Retryer* retries[MAX_RETRY_COUNT];

bool attemptConnectionToSavedWifi(){
	if(WiFi.status() == WL_CONNECTED){
		log_i("Already connected to %s with rssi %d",WiFi.SSID().c_str(),WiFi.RSSI());
		return true;
	}
	log_i("Trying to connect to a saved wifi network");

	DynamicJsonBuffer jsonBuffer(capacity);
	JsonObject& config = loadConfig(jsonBuffer);

	int found = scanForSavedWifiNetworks(config["wifi"].as<JsonArray&>());
	if(found == -1){
		log_i("No available saved networks");
		return false;
	}
	log_i("Found (index=%d) network with ssid: %s",found,config["wifi"].as<JsonArray&>()[found].as<JsonObject&>()["ssid"].as<String>().c_str());

	bool res = connectToWiFi(config["wifi"][found]["ssid"],config["wifi"][found]["password"]);

	jsonBuffer.clear();

	return res;
}

void addRetry(Retryer* retry){
	for(int i=0;i<MAX_RETRY_COUNT;i++){
		if(retries[i] == NULL){
			retries[i] = retry;
			return;
		}
	}
}

bool checkForWalkEnd(){
	if(attemptConnectionToSavedWifi()){
		int rssi = WiFi.RSSI();
		if(rssi < -55){
			log_d("Connection to saved WiFi too weak (%d) aborting walk finish",WiFi.RSSI());
			return false;
		}

		if(rssi == 0){
			log_d("Rssi could not have been estinamted aborting walk end");
			rssiEstimateFailCount++;
			if(rssiEstimateFailCount < 10){
				return false;
			}
		}

		int duration = (millis() - walkStartMilis)/1000;
		log_i("Finished walk duration was: %d",duration);
		walkStartMilis = -1;
		createWalkActivityRetry.retryFun = [duration] () -> bool {
			return createStravaWalkActivity(duration);
		};
		addRetry(&createWalkActivityRetry);
		return true;
	}
	return false;
}


Retryer walkEndRetry = {
	checkForWalkEnd,
	millis(),
	10*1000,
	10000,
	0
};

void setup() {
	Serial.begin(115200);
	configureMPU(1); 

	for(int i=0;i<MAX_RETRY_COUNT;i++){
		retries[i] = NULL;
	}

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
		addRetry(&walkEndRetry);
		log_i("Starting walk at %d",walkStartMilis);
	}, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

	if(attemptConnectionToSavedWifi()){
		startServer(&rtc);

		long time = getCurrentEpoch();
		if(time == 0){
			log_e("Could not set RTC time");
		}
		
		rtc.setTime(time+60*60);
		log_i("Set RTC time to: %s",rtc.getTime("%Y-%m-%dT%H:%M:%SZ").c_str());
	}
}


void loop() {
	for(int i=0;i<MAX_RETRY_COUNT;i++){
		Retryer* currRetry = retries[i];
		if (currRetry == NULL){
			continue;
		}

		if(currRetry->currRetryCount >= currRetry->maxRetryCount){
			log_d("Retry at index %d and interval %d reached its maximum retry count",i,currRetry->retryInterval);
			retries[i] = NULL;
			continue;
		}

		if(millis() - currRetry->prevRetryMillis > currRetry->retryInterval){
			log_d("Retrying entry at index %d and interval %d",i,currRetry->retryInterval);
			if(currRetry->retryFun()){
				log_d("Retry at index %d and interval %d succeded and was removed",i,currRetry->retryInterval);
				retries[i] = NULL;
			}
			currRetry->currRetryCount++;
			currRetry->prevRetryMillis = millis();
		}
	}
}