#include <ArduinoJson.h>
#include <Arduino.h>

JsonObject& parseJson(DynamicJsonBuffer& jsonBuffer,String json);
bool arrContainsKey(JsonArray& arr,String key);