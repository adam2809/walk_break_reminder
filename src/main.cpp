#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define STRAVA_API_URL "www.strava.com"
#define AUTH_HEADER "Authorization", "Bearer " + String(authToken)
#define ACCEPT_HEADER "Accept", "*/*"
#define ACCEPT_ENCODING_HEADER "Accept-Encoding", "gzip, deflate"
#define CONNECTION_HEADER "Connection", "keep-alive"
#define HOST_HEADER "Host", "www.strava.com"
#define USER_AGENT_HEADER "User-Agent", "ESP32StravaTracker/0.0.1"
#define MULTIPART_BOUNDARY "fad39427494e74d088217338ea717252"
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
WiFiClientSecure client;

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
	http.begin(STRAVA_API_URL + endpoint);
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
    uploadPayload.concat("--"+String(MULTIPART_BOUNDARY)+"--");
    uploadPayload.concat("\r\n");
}


void stravaUploadWifiClient(){
    client.println("POST /api/v3/uploads HTTP/1.1");
    client.println("Host: www.strava.com");
    client.println("User-Agent: ESP32Tracker");
    client.println("Accept-Encoding: gzip, deflate");
    client.println("Accept: */*");
    client.println("Connection: keep-alive");
    client.println("Authorization: Bearer f7a5b4dce1460124da2874dba3c75dd276d503a9");
    client.println("Content-Length: 3351");
    client.println("Content-Type: multipart/form-data; boundary=08978a0a5eda7bcc6bd762556a13de1d");
    client.println();
    client.println("--08978a0a5eda7bcc6bd762556a13de1d");
    client.println("Content-Disposition: form-data; name=\"name\"");
    client.println();
    client.println("value");
    client.println("--08978a0a5eda7bcc6bd762556a13de1d");
    client.println("Content-Disposition: form-data; name=\"description\"");
    client.println();
    client.println("value");
    client.println("--08978a0a5eda7bcc6bd762556a13de1d");
    client.println("Content-Disposition: form-data; name=\"trainer\"");
    client.println();
    client.println("value");
    client.println("--08978a0a5eda7bcc6bd762556a13de1d");
    client.println("Content-Disposition: form-data; name=\"commute\"");
    client.println();
    client.println("value");
    client.println("--08978a0a5eda7bcc6bd762556a13de1d");
    client.println("Content-Disposition: form-data; name=\"data_type\"");
    client.println();
    client.println("gpx");
    client.println("--08978a0a5eda7bcc6bd762556a13de1d");
    client.println("Content-Disposition: form-data; name=\"external_id\"");
    client.println();
    client.println("value");
    client.println("--08978a0a5eda7bcc6bd762556a13de1d");
    client.println("Content-Disposition: form-data; name=\"file\"; filename=\"test.gpx\"");
    client.println();
    client.println("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>");
    client.println("<gpx version=\"1.0\" creator=\"GPS Visualizer https://www.gpsvisualizer.com/\" xmlns=\"http://www.topografix.com/GPX/1/0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">");
    client.println("<trk>");
    client.println("  <name>output</name>");
    client.println("  <trkseg>");
    client.println("    <trkpt lat=\"52.531466667\" lon=\"13.426316667\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:14.058Z</time>");
    client.println("      <course>268.6</course>");
    client.println("      <speed>4183.41</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("    <trkpt lat=\"52.53\" lon=\"13.364516667\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:15.058Z</time>");
    client.println("      <course>184.5</course>");
    client.println("      <speed>3395.80</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("    <trkpt lat=\"52.4995\" lon=\"13.362116667\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:16.058Z</time>");
    client.println("      <course>109.8</course>");
    client.println("      <speed>8421.61</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("    <trkpt lat=\"52.461033333\" lon=\"13.469233333\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:17.058Z</time>");
    client.println("      <course>48.3</course>");
    client.println("      <speed>5868.37</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("    <trkpt lat=\"52.504516667\" lon=\"13.518333333\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:18.058Z</time>");
    client.println("      <course>20.3</course>");
    client.println("      <speed>1262.60</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("    <trkpt lat=\"52.5156\" lon=\"13.52245\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:19.058Z</time>");
    client.println("      <course>295.4</course>");
    client.println("      <speed>8852.30</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("    <trkpt lat=\"52.56445\" lon=\"13.419116667\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:20.058Z</time>");
    client.println("      <course>266.5</course>");
    client.println("      <speed>9608.95</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("    <trkpt lat=\"52.5559\" lon=\"13.277666667\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:21.058Z</time>");
    client.println("      <course>168.9</course>");
    client.println("      <speed>10975.21</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("    <trkpt lat=\"52.4579\" lon=\"13.296883333\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:22.058Z</time>");
    client.println("      <course>91.2</course>");
    client.println("      <speed>21695.61</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("    <trkpt lat=\"52.450983333\" lon=\"13.616866667\">");
    client.println("      <ele>0.0</ele>");
    client.println("      <time>2020-12-25T20:20:23.058Z</time>");
    client.println("      <course>91.2</course>");
    client.println("      <speed>21695.61</speed>");
    client.println("      <sat>12</sat>");
    client.println("      <hdop>1.0</hdop>");
    client.println("    </trkpt>");
    client.println("  </trkseg>");
    client.println("</trk>");
    client.println("</gpx>");
    client.println();
    client.println("--08978a0a5eda7bcc6bd762556a13de1d--");
    client.println();
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


    // if(!readFile(SPIFFS, "/test.gpx")){
    //     Serial.println("Could not test gpx file");
    // }
    // readBuffer.toCharArray(testGpx,readBuffer.length());

	connectToWiFi(config["ssid"],config["password"]);

    // makeUploadPayload();
    // Serial.println(uploadPayload);

    // String getAthleteInfoHeaders[2] = {
    //     AUTH_HEADER
    // };
    // String uploadActivityHeaders[16] = {
    //     ACCEPT_HEADER,
    //     ACCEPT_ENCODING_HEADER,
    //     AUTH_HEADER,
    //     CONNECTION_HEADER,
    //     MULTIPART_CONTENT_TYPE_HEADER,
    //     HOST_HEADER,
    //     USER_AGENT_HEADER
    // };


	// performStravaApiRequest(
    //     "POST",
    //     "/uploads",
    //     uploadPayload,
    //     uploadActivityHeaders,
    //     7
    // );

    client.setCACert(stravaCa);

    if (!client.connect(STRAVA_API_URL, 443)){
        Serial.println("Connection failed!");
    } else {
        Serial.println("Connected to server!");
	    Serial.println("Performing strava request");
        stravaUploadWifiClient();
        
        while (!client.available()){
            delay(50); //
            Serial.print(".");
        }  
        /* if data is available then receive and print to Terminal */
        while (client.available()) {
            char c = client.read();
            Serial.write(c);
        }

        /* if the server disconnected, stop the client */
        if (!client.connected()) {
            Serial.println();
            Serial.println("Server disconnected");
            client.stop();
        }
    }
}

void loop() {
}