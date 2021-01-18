#include <HTTPClient.h>
#include <WiFi.h>

#define SUPPORT_SERVER_URL "http://192.168.0.87:3000"



void connectToWiFi(const char* ssid,const char* password);
void performSupportServerRequest(HTTPClient &http,const char* method,String endpoint,String payload,String headers[],int headersCount);
