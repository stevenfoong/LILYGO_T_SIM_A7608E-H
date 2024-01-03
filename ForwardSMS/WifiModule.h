#include <WiFi.h>

#include <Preferences.h>

boolean WiFiUp = false;
unsigned long connectedMillis;

IPAddress local_ip(192,168,168,168);
IPAddress gateway(192,168,168,168);
IPAddress subnet(255,255,255,0);

void connectWiFi() {

  Preferences pref;
  String wifi_ssid;
  String wifi_password;

  pref.begin("tsim", false); 

  wifi_ssid = pref.getString("wifi-ssid");
  wifi_password = pref.getString("wifi-password");

  pref.end();

  if (wifi_ssid == "") {
    wifi_ssid = default_ssid;
  }

  if (wifi_password == "") {
    wifi_password = default_password;
  }

  WiFi.setAutoConnect(false);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(wifi_ssid, wifi_password);
  Serial.println("Connecting to WiFi ..");

  int i = 0;

  while (WiFi.status() != WL_CONNECTED && i++ < 15) {
    Serial.print('.');
    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to WiFi with IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
    WiFiUp = true;
  } else {
    Serial.println("Failed to connect Wifi.");
  }

  //Serial.println(WiFi.localIP());
  //Serial.println("Connecting to Internet ..");
  //HTTPClient http;
  //String url = WebHookURL + "?localip=" + WiFi.localIP();
  //http.begin(serverPath.c_str());
  //http.begin("https://www.yahoo.com");
  //http.begin("https://webhook.site/f3046b1d-5ee2-4734-8171-bde61a9dc567?localip=" + WiFi.localIP());
  //int httpResponseCode = http.GET();
  //if (httpResponseCode == 200) {
  //  Serial.println("Connect to Internet Successfully.");
    //String payload = http.getString();
    // Serial.println(payload);
  //} else if (httpResponseCode>0) {
  //  Serial.print("Error code: ");
  //  Serial.println(httpResponseCode);
  //} else {
  //  Serial.print("Error code: ");
  //  Serial.println(httpResponseCode);
  //}
      // Free resources
  //http.end();
}


void initWiFiap() {
//  if (WiFiUp) {
//    byte w8 = 0;
//    while (WiFi.status() != WL_CONNECTED && w8++ < 15) {
//      delay(333); // try for 5 seconds
//      Serial.print(">");
//    }
//    Serial.printf("\r\n");
//  }

//  if (WiFi.status() == WL_CONNECTED) {
//    Serial.printf("\tConnected to %s IP address %s strength %d%%\r\n", WiFi_SSID, WiFi.localIP().toString().c_str(), 2 * (WiFi.RSSI() + 100));
//    WiFi.setAutoReconnect(false);
//    retryCounter = 0; // reset counter when connected
//  } else {
  WiFi.mode(WIFI_AP); // drop station mode if LAN/WiFi is down
  WiFi.softAPConfig(local_ip, gateway, subnet);
  if (!WiFi.softAP(ap_ssid, ap_password)){
    Serial.printf("Soft AP creation failed.");
    while(1);
  }
  Serial.printf("Try %s AP with IP address %s\r\n", ap_ssid, WiFi.softAPIP().toString().c_str());


//  }
//  if (mdns.begin(mDNSname)) {
//    Serial.printf("mDNS responder started\r\n\tName: %s.local\r\n", mDNSname);
//  } else {
//    Serial.println("*** Error setting up mDNS responder\r\n");
//  }
//  if (UDP.begin(localPort)) {
//    Serial.printf("Broadcasting UDP on %s AP with IP address %s port %d\r\n", ap_ssid, WiFi.softAPIP().toString().c_str(), localPort);
//  } else {
//    Serial.println("*** Error setting up UDP\r\n");
//  }
}

void reconnectWifi() {
  connectedMillis = millis(); // update
  //retryCounter ++; // update connection retries
  WiFi.mode(WIFI_AP_STA); // LAN and AP and UDP clients
  WiFi.begin(); // connect to LAN
  //Serial.printf("Trying to reconnect to %s, attempt %d ", ssid, retryCounter);
  Serial.printf("Trying to reconnect to %s . ", default_ssid);
  connectWiFi();
}