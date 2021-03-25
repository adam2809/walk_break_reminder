#include "onboard_web_server.hpp"

AsyncWebServer server(PORT);
HTTPClient http;
ESP32Time* rtcRef;


void startServer(ESP32Time* _rtc){
	log_i("Starting the onboard web server on port %d",PORT);
	rtcRef = _rtc;

	server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
		log_i("Got GET on /");
		request->send_P(200, "text/html",config_html,templateProcessor);
	});


	server.on("/sleep", HTTP_GET, [&](AsyncWebServerRequest *request){
		log_i("Got GET on /sleep");
		request->send_P(200, "text/html","goin to slp");


		log_i("Sleeping");
		adc_power_off();  // adc power off disables wifi entirely, upstream bug
		esp_sleep_enable_ext0_wakeup(GPIO_NUM_15,0); delay(1500);//1 = High, 0 = Low
		adc_power_off();
		esp_deep_sleep_start();
	});

	server.on("/wifi", HTTP_GET, getAllWifiNetworks);
	server.on("/wifi", HTTP_DELETE, deleteWifiNetwork);
	AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/wifi", addNewWifiNetwork);
	handler->setMethod(HTTP_POST);
	server.addHandler(handler);

	server.on("/submit_esp_code", HTTP_POST, submitEspCode);

	server.onNotFound(notFound);

	server.begin();
}


void notFound(AsyncWebServerRequest *request){
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}

String templateProcessor(const String& var){
	if(var == "CURRENT_WIFI_SSID"){
		return "currSsid";
	}
	return String();
}


void getAllWifiNetworks(AsyncWebServerRequest *request){
		log_i("Got GET on /wifi");
		DynamicJsonBuffer jsonBuffer(capacity);
		JsonObject& config = loadConfig(jsonBuffer);

		char savedWiFiArr[JSON_STRING_BUFFER_LENGTH];
		config["wifi"].printTo(savedWiFiArr);
		request->send_P(200, "application/json",savedWiFiArr);
		jsonBuffer.clear();
}

void addNewWifiNetwork(AsyncWebServerRequest *request, JsonVariant &jsonVar){
	log_i("Got POST on /wifi");
	JsonObject& json = jsonVar.as<JsonObject&>();

	if(json.size() != 2){
		request->send_P(400, "application/json","Error: wrong field count");
	}

	if(
		!json.containsKey("ssid") ||
		!json.containsKey("password")
	){
		request->send_P(400, "application/json","Error: missing required field ssid or password");
		return;
	}

	String newConfig;
	json.printTo(newConfig);
	log_i("Adding wifi config: %s",newConfig);

	DynamicJsonBuffer jsonBuffer(capacity);
	JsonObject& config = loadConfig(jsonBuffer);

	JsonArray& currWiFiNetworks = config["wifi"].as<JsonArray&>();
	currWiFiNetworks.add(json);
	char wifiArrJsonString[JSON_STRING_BUFFER_LENGTH];
	currWiFiNetworks.printTo(wifiArrJsonString);

	char configJsonString[JSON_STRING_BUFFER_LENGTH];
	config.printTo(configJsonString);
	writeFile(SPIFFS, "/config.json", configJsonString);

	request->send_P(200, "application/json",wifiArrJsonString);
	jsonBuffer.clear();
}

void deleteWifiNetwork(AsyncWebServerRequest *request){
	log_i("Got DELETE on /wifi");
	if(!request->hasParam("ssid")){
		request->send_P(400, "application/json","Error: missing required parameter ssid");
		return;
	}
	const char* ssidToDelete = request->getParam("ssid")->value().c_str();

	DynamicJsonBuffer jsonBuffer(capacity);
	JsonObject& config = loadConfig(jsonBuffer);
	JsonArray& currWiFiNetworks = config["wifi"].as<JsonArray&>();

	for (int i=0; i<currWiFiNetworks.size(); i++) {
		if(currWiFiNetworks[i]["ssid"] == ssidToDelete){
			log_i("Removing %s",ssidToDelete);
			currWiFiNetworks.remove(i);
			break;
		}
	}

	char configJsonString[JSON_STRING_BUFFER_LENGTH];
	config.printTo(configJsonString);
	writeFile(SPIFFS, "/config.json", configJsonString);

	char wifiArrJsonString[JSON_STRING_BUFFER_LENGTH];
	currWiFiNetworks.printTo(wifiArrJsonString);

	request->send_P(200, "application/json",wifiArrJsonString);
	jsonBuffer.clear();
}

void submitEspCode(AsyncWebServerRequest *request){
	log_i("Got POST on /submit_esp_code");
	if(!request->hasParam("esp_code")){
		log_w("Missing esp_code parameter");
		request->send_P(400, "application/json","Error: missing required parameter esp_code");
		return;
	}
	const char* espCode = request->getParam("esp_code")->value().c_str();

	http.begin(SUPPORT_SERVER_URL + String("/tokens?esp_code=") + espCode);
	int httpResponseCode = http.GET();
	if (httpResponseCode < 0){
		log_w("Failed to perform esp code submittion request");
		request->send_P(500, "text/plain","Error while making request for tokens");
	}
	
	String resJson = http.getString();

	if(httpResponseCode >= 200 && httpResponseCode < 300){
		log_i("Esp code submittion request was successful");

		DynamicJsonBuffer currConfigJsonBuffer(capacity);
		DynamicJsonBuffer responseJsonBuffer(capacity);

		JsonObject& config = loadConfig(currConfigJsonBuffer);
		JsonObject& response = parseJson(responseJsonBuffer,resJson);
		
		if(!(response.containsKey("refresh_token") && response.containsKey("access_token"))){
			log_w("Missing parameter in esp code submitton response");
			request->send_P(500, "text/plain","The tokens request response does not contain required data");
			return;
		}

		config["refresh_token"] = response["refresh_token"];
		config["access_token"] = response["access_token"];

		char configJsonString[JSON_STRING_BUFFER_LENGTH];
		config.printTo(configJsonString);
		writeFile(SPIFFS, "/config.json", configJsonString);
		currConfigJsonBuffer.clear();
		responseJsonBuffer.clear();
	}else{
		log_w("Esp code submittion response has non-2xx code (%d)",httpResponseCode);
	}
	log_i("Esp code submitton response was: %s",resJson.c_str());

	request->send_P(httpResponseCode, "application/json",resJson.c_str());
	http.end();
}

long getCurrentEpoch(){
	HTTPClient timeHttp;
	timeHttp.begin(SUPPORT_SERVER_URL+String("/time"));
	int httpResponseCode = timeHttp.GET();
	long res = 0;

	if (httpResponseCode < 0){
		log_w("Failed to perform current time request");
	}else if(httpResponseCode < 200 || httpResponseCode >= 300){
		log_w("Current time request resulted in non-2xx response (%d)",httpResponseCode);
	}else{
		res = atol(timeHttp.getString().c_str());
		log_i("Successful current time request with timestamp %d",res);
	}

	timeHttp.end();

	return res;
}

bool performStravaWalkCreationRequest(int walkDuration,String accessToken,String timestamp){
	HTTPClient stravaHttp;
	bool res;

	char millisStr[100];
	sprintf(millisStr,"%lu",millis());
	char durationStr[100];
	sprintf(durationStr,"%d",walkDuration);

	stravaHttp.begin(
		STRAVA_SERVER_URL + String("/activities?") + 
		"name="+millisStr+"&" +
		"type=Walk&" +
		"start_date_local="+timestamp+"&" +
		"elapsed_time="+durationStr+"&"
		"description=This\%20activity\%20was\%20created\%20by\%20prototype\%20of\%20ESP\%20Strava\%20Tracker\%20for\%20testing\%20purpuses"
	);
	stravaHttp.addHeader("Authorization","Bearer " + accessToken);

	int httpResponseCode = stravaHttp.POST("");
	if (httpResponseCode < 0){
		log_w("Request to create strava walk activity failed");
		res = false;
	}else if(httpResponseCode >= 200 && httpResponseCode < 300){
		log_i("Successfully created strava walk activity with duration=%s and name=%s",durationStr,millisStr);
		res = true;
	}else{
		log_i("The strava walk activity creation request responded with non-2xx (%d)",httpResponseCode);
		String resContent = stravaHttp.getString();
		res = false;
	}

	stravaHttp.end();
	return res;

}

bool createStravaWalkActivity(int walkDurationInSecs){
	log_i("Attempting to create walk activity");

	DynamicJsonBuffer currConfigJsonBuffer(capacity);
	JsonObject& config = loadConfig(currConfigJsonBuffer);

	time_t startTime = rtcRef->getEpoch() - walkDurationInSecs;
	tm* timeinfo = localtime(&startTime);
	char timestamp[25];
	strftime(timestamp,25, "%Y-%m-%dT%H:%M:%SZ", timeinfo);
	log_i("Activity start time is: %s",timestamp);

	bool res = performStravaWalkCreationRequest(walkDurationInSecs,config["access_token"].as<String>(),timestamp);

	currConfigJsonBuffer.clear();

	return res;
}
