/*
  Wifi secure connection example for ESP32
  Running on TLS 1.2 using mbedTLS
  Suporting the following chipersuites:
  "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384","TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384","TLS_DHE_RSA_WITH_AES_256_GCM_SHA384","TLS_ECDHE_ECDSA_WITH_AES_256_CCM","TLS_DHE_RSA_WITH_AES_256_CCM","TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384","TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384","TLS_DHE_RSA_WITH_AES_256_CBC_SHA256","TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA","TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA","TLS_DHE_RSA_WITH_AES_256_CBC_SHA","TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8","TLS_DHE_RSA_WITH_AES_256_CCM_8","TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384","TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384","TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384","TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384","TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384","TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256","TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA","TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256","TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256","TLS_DHE_RSA_WITH_AES_128_GCM_SHA256","TLS_ECDHE_ECDSA_WITH_AES_128_CCM","TLS_DHE_RSA_WITH_AES_128_CCM","TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256","TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256","TLS_DHE_RSA_WITH_AES_128_CBC_SHA256","TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA","TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA","TLS_DHE_RSA_WITH_AES_128_CBC_SHA","TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8","TLS_DHE_RSA_WITH_AES_128_CCM_8","TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256","TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256","TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256","TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256","TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256","TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256","TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA","TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA","TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA","TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA","TLS_DHE_PSK_WITH_AES_256_GCM_SHA384","TLS_DHE_PSK_WITH_AES_256_CCM","TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384","TLS_DHE_PSK_WITH_AES_256_CBC_SHA384","TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA","TLS_DHE_PSK_WITH_AES_256_CBC_SHA","TLS_DHE_PSK_WITH_CAMELLIA_256_GCM_SHA384","TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384","TLS_DHE_PSK_WITH_CAMELLIA_256_CBC_SHA384","TLS_PSK_DHE_WITH_AES_256_CCM_8","TLS_DHE_PSK_WITH_AES_128_GCM_SHA256","TLS_DHE_PSK_WITH_AES_128_CCM","TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256","TLS_DHE_PSK_WITH_AES_128_CBC_SHA256","TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA","TLS_DHE_PSK_WITH_AES_128_CBC_SHA","TLS_DHE_PSK_WITH_CAMELLIA_128_GCM_SHA256","TLS_DHE_PSK_WITH_CAMELLIA_128_CBC_SHA256","TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256","TLS_PSK_DHE_WITH_AES_128_CCM_8","TLS_ECDHE_PSK_WITH_3DES_EDE_CBC_SHA","TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA","TLS_RSA_WITH_AES_256_GCM_SHA384","TLS_RSA_WITH_AES_256_CCM","TLS_RSA_WITH_AES_256_CBC_SHA256","TLS_RSA_WITH_AES_256_CBC_SHA","TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384","TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384","TLS_ECDH_RSA_WITH_AES_256_CBC_SHA","TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384","TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384","TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA","TLS_RSA_WITH_AES_256_CCM_8","TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384","TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256","TLS_RSA_WITH_CAMELLIA_256_CBC_SHA","TLS_ECDH_RSA_WITH_CAMELLIA_256_GCM_SHA384","TLS_ECDH_RSA_WITH_CAMELLIA_256_CBC_SHA384","TLS_ECDH_ECDSA_WITH_CAMELLIA_256_GCM_SHA384","TLS_ECDH_ECDSA_WITH_CAMELLIA_256_CBC_SHA384","TLS_RSA_WITH_AES_128_GCM_SHA256","TLS_RSA_WITH_AES_128_CCM","TLS_RSA_WITH_AES_128_CBC_SHA256","TLS_RSA_WITH_AES_128_CBC_SHA","TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256","TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256","TLS_ECDH_RSA_WITH_AES_128_CBC_SHA","TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256","TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256","TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA","TLS_RSA_WITH_AES_128_CCM_8","TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256","TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256","TLS_RSA_WITH_CAMELLIA_128_CBC_SHA","TLS_ECDH_RSA_WITH_CAMELLIA_128_GCM_SHA256","TLS_ECDH_RSA_WITH_CAMELLIA_128_CBC_SHA256","TLS_ECDH_ECDSA_WITH_CAMELLIA_128_GCM_SHA256","TLS_ECDH_ECDSA_WITH_CAMELLIA_128_CBC_SHA256","TLS_RSA_WITH_3DES_EDE_CBC_SHA","TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA","TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA","TLS_RSA_PSK_WITH_AES_256_GCM_SHA384","TLS_RSA_PSK_WITH_AES_256_CBC_SHA384","TLS_RSA_PSK_WITH_AES_256_CBC_SHA","TLS_RSA_PSK_WITH_CAMELLIA_256_GCM_SHA384","TLS_RSA_PSK_WITH_CAMELLIA_256_CBC_SHA384","TLS_RSA_PSK_WITH_AES_128_GCM_SHA256","TLS_RSA_PSK_WITH_AES_128_CBC_SHA256","TLS_RSA_PSK_WITH_AES_128_CBC_SHA","TLS_RSA_PSK_WITH_CAMELLIA_128_GCM_SHA256","TLS_RSA_PSK_WITH_CAMELLIA_128_CBC_SHA256","TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA","TLS_PSK_WITH_AES_256_GCM_SHA384","TLS_PSK_WITH_AES_256_CCM","TLS_PSK_WITH_AES_256_CBC_SHA384","TLS_PSK_WITH_AES_256_CBC_SHA","TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384","TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384","TLS_PSK_WITH_AES_256_CCM_8","TLS_PSK_WITH_AES_128_GCM_SHA256","TLS_PSK_WITH_AES_128_CCM","TLS_PSK_WITH_AES_128_CBC_SHA256","TLS_PSK_WITH_AES_128_CBC_SHA","TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256","TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256","TLS_PSK_WITH_AES_128_CCM_8","TLS_PSK_WITH_3DES_EDE_CBC_SHA","TLS_EMPTY_RENEGOTIATION_INFO_SCSV"]
  2017 - Evandro Copercini - Apache 2.0 License.
*/

#include <WiFiClientSecure.h>

const char* ssid     = "UPC6530397";     // your network SSID (name of wifi network)
const char* password = "4uh2dnyjbpGh"; // your network password

const char*  server = "www.strava.com";  // Server URL

// www.howsmyssl.com root certificate authority, to verify the server
// change it to your server root CA
// SHA1 fingerprint is broken now!

const char* test_root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx\n" \
"EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT\n" \
"EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp\n" \
"ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz\n" \
"NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH\n" \
"EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE\n" \
"AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw\n" \
"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD\n" \
"E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH\n" \
"/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy\n" \
"DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh\n" \
"GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR\n" \
"tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA\n" \
"AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE\n" \
"FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX\n" \
"WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu\n" \
"9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr\n" \
"gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo\n" \
"2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO\n" \
"LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI\n" \
"4uJEvlz36hz1\n" \
"-----END CERTIFICATE-----\n";

// You can use x.509 client certificates if you want
//const char* test_client_key = "";   //to verify the client
//const char* test_client_cert = "";  //to verify the client


WiFiClientSecure client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  delay(100);

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  client.setCACert(test_root_ca);
  //client.setCertificate(test_client_key); // for client verification
  //client.setPrivateKey(test_client_cert);	// for client verification

  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    client.println("POST /api/v3/uploads HTTP/1.1");
    client.println("Host: www.strava.com");
    client.println("Connection: keep-alive");
    client.println("Accept: */*");
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

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }

    client.stop();
  }
}

void loop() {
  // do nothing
}