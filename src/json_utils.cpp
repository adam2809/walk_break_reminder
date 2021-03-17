#include "json_utils.hpp"

JsonObject& parseJson(DynamicJsonBuffer& jsonBuffer,String json){
	JsonObject& parsedJson = jsonBuffer.parseObject(json);

	if (parsedJson.success()){
		String parsed;
		parsedJson.printTo(parsed);
		log_v("Parsed json: %s",parsed);
	}else{
		log_w("Could not parse json: %s", json);
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

