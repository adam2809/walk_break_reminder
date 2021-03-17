#include "networking.hpp"

void connectToWiFi(const char* ssid,const char* password){
	log_d("Trying to connect to WiFi with SSID = %s and password %s",ssid,password);
	WiFi.begin(ssid, password);

	uint8_t i = 0;
	while (WiFi.status() != WL_CONNECTED){
		// TODO remove the delay here
		delay(500);

		if ((++i % 16) == 0){
			log_d("Still trying to connect");
		}
	}

	log_i("Connected to %s. My IP address is: %s",ssid,WiFi.localIP().toString());
}

void logWifiNetworkStats(int index){
	log_d("Scan result %d: %s (%d) %s",index,WiFi.SSID(index),WiFi.RSSI(index),(WiFi.encryptionType(index) == WIFI_AUTH_OPEN)?" ":"*");
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
Scans for networks and returns index of found wifi network or -1 in none found
*/
int scanForSavedWifiNetworks(JsonArray& savedWiFiNetworks){
    int n = WiFi.scanNetworks();

    if (n == 0) {
		return -1;
    }
	for (int i = 0; i < n; ++i) {
		logWifiNetworkStats(i);
		if(indexOfSsidInWifiArr(savedWiFiNetworks,WiFi.SSID(i)) != -1){
			return i;
		}
	}
	return -1;
}

