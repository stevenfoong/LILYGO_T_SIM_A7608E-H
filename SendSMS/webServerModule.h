#include <ESPAsyncWebSrv.h>
#include "web_index.h"
#include <Preferences.h>

AsyncWebServer webserver(80);

Preferences pref;


const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void startWebServer() {
  

    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String html = HTML_CONTENT_HOME;
        request->send(200, "text/html", html);
    });

        // Send a GET request to <IP>/get?message=<message>
    webserver.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/html", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    webserver.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam("webhook-url", true)) {
            message = request->getParam("webhook-url", true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    webserver.on("/savewifi", HTTP_POST, [](AsyncWebServerRequest *request){
      String message;
      String orig_post_wifi_ssid;
      String orig_post_wifi_password;
      String post_wifi_ssid;
      String post_wifi_password;

      pref.begin("tsim", false); 

      bool error_found = false;
        if (request->hasParam("wifi-ssid", true)) {
            post_wifi_ssid = request->getParam("wifi-ssid", true)->value();
        } else {
            message = "SSID not found";
            error_found = true;
        }
        if (post_wifi_ssid == "") {
            message = "SSID is empty";
            error_found = true;
        }

        if (request->hasParam("wifi-password", true)) {
            post_wifi_password = request->getParam("wifi-password", true)->value();
        } else {
            message = "wifi pasword not found";
            error_found = true;
        }
        if (post_wifi_password == "") {
            message = "wifi pasword is empty";
            error_found = true;
        }

        if (error_found) {
            request->send(200, "text/plain", "Error : " + message + ". Press back button to previous page.");
        } else {
          orig_post_wifi_ssid = pref.getString("wifi-ssid");
          orig_post_wifi_password = pref.getString("wifi-password");
          pref.putString("wifi-ssid", post_wifi_ssid);
          pref.putString("wifi-password", post_wifi_password);
          request->send(200, "text/plain", "wifi SSID : " + orig_post_wifi_ssid + " -> " + post_wifi_ssid + " , wifi PASSWORD : " + orig_post_wifi_password + " -> " + post_wifi_password + ". Press back button to previous page.");
        }
        //request->send(200, "text/plain", "Hello, POST: " + message);
      pref.end();
    });

    webserver.on("/savewebhook", HTTP_POST, [](AsyncWebServerRequest *request){
      String message;
      String orig_webhook_url;
      String post_webhook_url;

      pref.begin("tsim", false); 

      bool error_found = false;

      if (request->hasParam("webhook-url", true)) {
        post_webhook_url = request->getParam("webhook-url", true)->value();
      } else {
        message = "URL not found";
        error_found = true;
      }

      if (post_webhook_url == "") {
        message = "webhook URL is empty";
        error_found = true;
      }       

      if (error_found) {
        request->send(200, "text/plain", "Error : " + message + ". Press back button to previous page.");
      } else {
        orig_webhook_url = pref.getString("webhook-url");
        pref.putString("webhook-url", post_webhook_url);
        request->send(200, "text/plain", "Webhook URL : " + orig_webhook_url + " -> " + post_webhook_url + ". Press back button to previous page.");
      }
      pref.end();
    });

    webserver.on("/reboot", HTTP_GET, [] (AsyncWebServerRequest *request) {
      Serial.println("Rebooting Unit");
      ESP.restart();
    });

    webserver.on("/viewconfig", HTTP_GET, [] (AsyncWebServerRequest *request) {
      pref.begin("tsim", false); 

      String orig_post_wifi_ssid;
      String orig_post_wifi_password;
      String orig_webhook_url;

      orig_post_wifi_ssid = pref.getString("wifi-ssid");
      orig_post_wifi_password = pref.getString("wifi-password");
      orig_webhook_url = pref.getString("webhook-url");
      
      request->send(200, "text/plain", "wifi SSID : " + orig_post_wifi_ssid + " , wifi PASSWORD : " + orig_post_wifi_password + ", Webhook URL : " + orig_webhook_url);

      pref.end();
    });


  webserver.onNotFound(notFound);

  webserver.begin();
}