#include <HTTPClient.h>
#include <WiFi.h>

#define SUPPORT_SERVER_URL "http://192.168.0.87:8000"

void connectToWiFi(const char* ssid,const char* password);
String performSupportServerRequest(HTTPClient& http,const char* method,String endpoint,String requestParams,String headers[],int headersCount,String payload);
