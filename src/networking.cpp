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

