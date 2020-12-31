#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define STRAVA_API_URL "https://www.strava.com/api/v3"
#define AUTH_HEADER "Authorization", "Bearer " + String(authToken)
#define ACCEPT_HEADER "Accept", "*/*"
#define ACCEPT_ENCODING_HEADER "Accept-Encoding", "gzip, deflate"
#define CONNECTION_HEADER "Connection", "keep-alive"
#define HOST_HEADER "Host", "www.strava.com"
#define USER_AGENT_HEADER "User-Agent", "ESP32StravaTracker/0.0.1"
#define MULTIPART_BOUNDARY "ESP32StravaTrackeraf39cf00aaaa"
#define MULTIPART_CONTENT_TYPE_HEADER "Content-Type", "multipart/form-data; boundary=" + String(MULTIPART_BOUNDARY)
#define MULTIPART_FORM_KEY_PREFIX "Content-Disposition: form-data; name="

String uploadActivityFormKeys[] = {
    "name",
    "description",
    "trainer",
    "commute",
    "data_type",
    "external_id",
    "file",
};

const char* authToken;
char stravaCa[2048];
HTTPClient http;
StaticJsonBuffer<1024> jsonBuffer;
String readBuffer;
String uploadPayload;
char testGpx[4096];

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
    Serial.println(payload);
	http.begin(STRAVA_API_URL + endpoint, stravaCa);
    for(int i=0;i<headersCount;i+=2){
        http.addHeader(headers[i], headers[i+1]);
    }

	int httpCode = http.sendRequest(method,uploadPayload);

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

void makeUploadPayload(){
    String uploadActivityFormVals[] = {
        "tescisko",
        "tesciskownie aktywne",
        "true",
        "false",
        "gpx",
        "ext_id",
        testGpx
    };

    for(int i=0;i<7;i++){
        String key = uploadActivityFormKeys[i];
        String val = uploadActivityFormVals[i];

        
        uploadPayload.concat("--"+String(MULTIPART_BOUNDARY));uploadPayload.concat("\r\n");
        uploadPayload.concat(String(MULTIPART_FORM_KEY_PREFIX) + "\""+ key+"\"");if(key=="file") uploadPayload.concat("; filename=\"test.gpx\"");uploadPayload.concat("\r\n");
        uploadPayload.concat("\r\n");
        uploadPayload.concat(val);uploadPayload.concat("\r\n");
    }
    uploadPayload.concat("\r\n");
    uploadPayload.concat("--"+String(MULTIPART_BOUNDARY)+"--");uploadPayload.concat("\r\n");
    uploadPayload.concat("\r\n");
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


    if(!readFile(SPIFFS, "/test.gpx")){
        Serial.println("Could not test gpx file");
    }
    readBuffer.toCharArray(testGpx,readBuffer.length());

	connectToWiFi(config["ssid"],config["password"]);

    makeUploadPayload();
    Serial.println(uploadPayload);

    String getAthleteInfoHeaders[2] = {
        AUTH_HEADER
    };
    String uploadActivityHeaders[16] = {
        ACCEPT_HEADER,
        ACCEPT_ENCODING_HEADER,
        AUTH_HEADER,
        CONNECTION_HEADER,
        MULTIPART_CONTENT_TYPE_HEADER,
        HOST_HEADER,
        USER_AGENT_HEADER
    };


	Serial.println("Performing strava request");
	performStravaApiRequest(
        "POST",
        "/uploads",
        uploadPayload,
        uploadActivityHeaders,
        7
    );
}

void loop() {
}