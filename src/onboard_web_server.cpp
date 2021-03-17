#include "onboard_web_server.hpp"

AsyncWebServer server(80);
HTTPClient http;
char* currSsid;

void startServer(char* _currSsid){
	currSsid = _currSsid;

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
		return currSsid;
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
		request->send_P(400, "application/json","Error: missing required parameter esp_code");
		return;
	}
	const char* espCode = request->getParam("esp_code")->value().c_str();

	http.begin(SUPPORT_SERVER_URL + String("/tokens?esp_code=") + espCode);
	int httpResponseCode = http.GET();
	if (httpResponseCode < 0){
		request->send_P(500, "text/plain","Error while making request for tokens");
	}
	
	String resJson = http.getString();

	if(httpResponseCode == 200){
		DynamicJsonBuffer currConfigJsonBuffer(capacity);
		DynamicJsonBuffer responseJsonBuffer(capacity);

		JsonObject& config = loadConfig(currConfigJsonBuffer);
		JsonObject& response = parseJson(responseJsonBuffer,resJson);
		
		if(!(response.containsKey("refresh_token") && response.containsKey("access_token"))){
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
	}

	request->send_P(httpResponseCode, "application/json",resJson.c_str());
	http.end();
}

