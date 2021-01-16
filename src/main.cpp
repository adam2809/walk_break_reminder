#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define SUPPORT_SERVER_URL "http://192.168.0.87:3000"
#define ACCEPT_HEADER "Accept", "*/*"
#define ACCEPT_ENCODING_HEADER "Accept-Encoding", "gzip, deflate"
#define CONNECTION_HEADER "Connection", "keep-alive"
#define HOST_HEADER "Host", "192.168.0.87"
#define USER_AGENT_HEADER "User-Agent", "ESP32StravaTracker/0.0.1"
#define CONTENT_TYPE_HEADER "Content-Type", "application/xml"

HTTPClient http;
StaticJsonBuffer<1024> jsonBuffer;
String readBuffer;
char testGpx[4096];

bool readFile(fs::FS &fs, const char * path){
    // TODO make this like here https://github.com/adjavaherian/solar-server/blob/master/lib/Poster/Poster.cpp
    Serial.printf("Reading file: %s\r\n", path);

    readBuffer = "";
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return false;
    }

    Serial.println("- read from file:");
    while(file.available()){
        String curr = file.readString();
        readBuffer.concat(curr);
    }
    file.close();
    return true;
}
 
void connectToWiFi(const char* ssid,const char* password){
	WiFi.mode(WIFI_STA);
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

	Serial.print(F("Connected. My IP address is: "));
	Serial.println(WiFi.localIP());
}

void performSupportServerRequest(const char* method,String endpoint,String payload,String headers[],int headersCount){
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
}

void setup() {
	Serial.begin(115200);

	if(!SPIFFS.begin()){ 
		Serial.println("An Error has occurred while mounting SPIFFS");  
	}

        
    if(!readFile(SPIFFS, "/config.json")){
        Serial.println("Could not load config file");
    }
    JsonObject& config = jsonBuffer.parseObject(readBuffer);

    if(!readFile(SPIFFS, "/test.gpx")){
        Serial.println("Could not test gpx file");
    }
    readBuffer.toCharArray(testGpx,readBuffer.length());

	connectToWiFi(config["ssid"],config["password"]);

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
        "POST",
        "/uploads",
        testGpx,
        uploadActivityHeaders,
        6
    );
}

void loop() {
}