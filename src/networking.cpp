#include "networking.hpp"

bool connectToWiFi(const char* ssid,const char* password){
	log_d("Trying to connect to WiFi with SSID = %s and password %s",ssid,password);
	WiFi.begin(ssid, password);

	long startMillis = millis();
	while (millis() - startMillis < WIFI_CONN_WAIT_MILLIS && WiFi.status() != WL_CONNECTED);

	if(WiFi.status() == WL_CONNECTED){
		log_i("Connected to %s with RSSI = %d and IP = %s",WiFi.SSID().c_str(),WiFi.RSSI(),WiFi.localIP().toString().c_str());
		return true;
	}else{
		log_i("Could not connect to %s",ssid);
		return false;
	}
}

void logWifiNetworkStats(int index){
	log_d("Scan result %d: %s (%d) %s",index,WiFi.SSID(index).c_str(),WiFi.RSSI(index),(WiFi.encryptionType(index) == WIFI_AUTH_OPEN)?" ":"*");
}


int indexOfSsidInWifiArr(JsonArray& arr,String ssid){
	for (int i = 0; i < arr.size(); i++){
		String currSsid = arr.get<JsonObject&>(i)["ssid"];
		if(arr.get<JsonObject&>(i)["ssid"].as<String>() == ssid){
			return i;
		}
	}
	return -1;
}

/*
Scans for networks and returns index of found wifi network in config or -1 in none found
*/
int scanForSavedWifiNetworks(JsonArray& savedWiFiNetworks){
    int n = WiFi.scanNetworks();

    if (n == 0) {
		return -1;
    }
	for (int i = 0; i < n; ++i) {
		logWifiNetworkStats(i);
		int found = indexOfSsidInWifiArr(savedWiFiNetworks,WiFi.SSID(i));
		if(found != -1){
			return found;
		}
	}
	return -1;
}

