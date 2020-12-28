#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define STRAVA_API_URL "https://www.strava.com/api/v3"
#define AUTH_HEADER "Authorization", "Bearer " + String(authToken)

const char* authToken;
char stravaCa[2048];
HTTPClient http;
StaticJsonBuffer<1024> jsonBuffer;
String readBuffer;

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

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
        Serial.println(curr);
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

void performStravaApiRequest(const char* method,String endpoint,String payload,String headers[],int headersCount){
	http.begin(STRAVA_API_URL + endpoint, stravaCa);
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
    
    if(!readFile(SPIFFS, "/strava_ca.pem")){
        Serial.println("Could not load strava certificate file");
    }
    readBuffer.toCharArray(stravaCa,readBuffer.length());

        
    if(!readFile(SPIFFS, "/config.json")){
        Serial.println("Could not load config file");
    }
    JsonObject& config = jsonBuffer.parseObject(readBuffer);
    authToken = config["auth_token"];
    Serial.print("Auth token is: ");Serial.println(authToken);

	connectToWiFi(config["ssid"],config["password"]);

	Serial.println("Performing strava request");

    String getAthleteInfoHeaders[2] = {
        AUTH_HEADER
    };

	performStravaApiRequest(
        "GET",
        "/athlete",
        "",
        getAthleteInfoHeaders,
        1
    );
}

void loop() {
}