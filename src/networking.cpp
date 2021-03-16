#include "networking.hpp"

void connectToWiFi(const char* ssid,const char* password){
	Serial.print("Trying to connect to WiFi with SSID = ");Serial.print(ssid);Serial.print(" and password ");Serial.print(password);Serial.println();
	WiFi.begin(ssid, password);
	Serial.print("Connecting to ");Serial.println(ssid);

	uint8_t i = 0;
	while (WiFi.status() != WL_CONNECTED){
		Serial.print('.');
		delay(500);

		if ((++i % 16) == 0){
			Serial.println(F(" still trying to connect"));
		}
	}

	Serial.print(F("Connected. My IP address is: "));Serial.println(WiFi.localIP());
}

void printWifiNetworkStats(int index){
	Serial.print(index + 1);
	Serial.print(": ");
	Serial.print(WiFi.SSID(index));
	Serial.print(" (");
	Serial.print(WiFi.RSSI(index));
	Serial.print(")");
	Serial.println((WiFi.encryptionType(index) == WIFI_AUTH_OPEN)?" ":"*");
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
    Serial.println("Starting to scan for networks");
    int n = WiFi.scanNetworks();

    if (n == 0) {
		return -1;
    }

	Serial.print(n);
	Serial.println(" networks found:");
	for (int i = 0; i < n; ++i) {
		printWifiNetworkStats(i);
		String currSsid = WiFi.SSID(i);
		if(indexOfSsidInWifiArr(savedWiFiNetworks,currSsid) != -1){
			return i;
		}
	}
	return -1;
}

String performSupportServerRequest(HTTPClient& http,const char* method,String endpoint,String requestParams,String headers[],int headersCount,String payload){
    Serial.println(payload);
	http.begin(SUPPORT_SERVER_URL + endpoint);
    for(int i=0;i<headersCount;i+=2){
        http.addHeader(headers[i], headers[i+1]);
    }

	int httpCode = http.sendRequest(method,payload);

	if (httpCode > 0) {
		String payload = http.getString();
		Serial.println(httpCode);
		Serial.println(payload);
	}
	else {
		Serial.println("Error on HTTP request");
	}

	http.end();
	return "";
}

