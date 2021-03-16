#include "json_utils.hpp"

JsonObject& parseJson(DynamicJsonBuffer& jsonBuffer,String json){
	JsonObject& parsedJson = jsonBuffer.parseObject(json);

	if (parsedJson.success()){
		Serial.println("Parsed json:");
		parsedJson.printTo(Serial);Serial.println();
	}else{
		Serial.println("Could not parse json");
	}

	return parsedJson;
}

bool arrContainsKey(JsonArray& arr,String key){
	for (int i = 0; i < arr.size(); i++){
		if(arr.get<String>(i) == key){
			return true;
		}
	}
	return false;
}

