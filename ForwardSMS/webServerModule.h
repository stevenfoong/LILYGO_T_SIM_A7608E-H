#include <ESPAsyncWebSrv.h>
#include "web_index.h"
#include <Preferences.h>

#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>


AsyncWebServer webserver(80);

Preferences pref;
String sim_number;


const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void startWebServer() {

    //TinyGsm modem(SerialAT);
    //modem.sendAT("+CNUM");
    //modem.waitResponse(1000L, sim_number);
    SerialAT.print("AT+CNUM\r\n");
    sim_number = SerialAT.readString();

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

    webserver.on("/savebackupwebhook", HTTP_POST, [](AsyncWebServerRequest *request){
      String message;
      String orig_backup_webhook_url;
      String post_backup_webhook_url;

      pref.begin("tsim", false); 

      bool error_found = false;

      if (request->hasParam("backup-webhook-url", true)) {
        post_backup_webhook_url = request->getParam("backup-webhook-url", true)->value();
      } else {
        message = "URL not found";
        error_found = true;
      }

      if (post_backup_webhook_url == "") {
        message = "webhook URL is empty";
        error_found = true;
      }       

      if (error_found) {
        request->send(200, "text/plain", "Error : " + message + ". Press back button to previous page.");
      } else {
        orig_backup_webhook_url = pref.getString("b-webhook-url");
        pref.putString("b-webhook-url", post_backup_webhook_url);
        request->send(200, "text/plain", "Backup Webhook URL : " + orig_backup_webhook_url + " -> " + post_backup_webhook_url + ". Press back button to previous page.");
      }
      pref.end();
    });

    webserver.on("/saveport", HTTP_POST, [](AsyncWebServerRequest *request){
      String message;
      String orig_port;
      String post_port;

      pref.begin("tsim", false); 

      bool error_found = false;

      if (request->hasParam("port", true)) {
        post_port = request->getParam("port", true)->value();
      } else {
        message = "PORT not found";
        error_found = true;
      }

      if (post_port == "") {
        message = "PORT is empty";
        error_found = true;
      }       

      if (error_found) {
        request->send(200, "text/plain", "Error : " + message + ". Press back button to previous page.");
      } else {
        orig_port = pref.getString("port");
        pref.putString("port", post_port);
        request->send(200, "text/plain", "Port : " + orig_port + " -> " + post_port + ". Press back button to previous page.");
      }
      pref.end();
    });

    webserver.on("/savesender", HTTP_POST, [](AsyncWebServerRequest *request){
      String message;
      String orig_sender;
      String post_sender;

      pref.begin("tsim", false); 

      bool error_found = false;

      if (request->hasParam("sender", true)) {
        post_sender = request->getParam("sender", true)->value();
      } else {
        message = "SENDER not found";
        error_found = true;
      }

      if (post_sender == "") {
        message = "SENDER is empty";
        error_found = true;
      }       

      if (error_found) {
        request->send(200, "text/plain", "Error : " + message + ". Press back button to previous page.");
      } else {
        orig_sender = pref.getString("sender");
        pref.putString("sender", post_sender);
        request->send(200, "text/plain", "Sender : " + orig_sender + " -> " + post_sender + ". Press back button to previous page.");
      }
      pref.end();
    });

    webserver.on("/savereceiver", HTTP_POST, [](AsyncWebServerRequest *request){
      String message;
      String orig_receiver;
      String post_receiver;

      pref.begin("tsim", false); 

      bool error_found = false;

      if (request->hasParam("receiver", true)) {
        post_receiver = request->getParam("receiver", true)->value();
      } else {
        message = "RECEIVER not found";
        error_found = true;
      }

      if (post_receiver == "") {
        message = "RECEIVER is empty";
        error_found = true;
      }       

      if (error_found) {
        request->send(200, "text/plain", "Error : " + message + ". Press back button to previous page.");
      } else {
        orig_receiver = pref.getString("receiver");
        pref.putString("receiver", post_receiver);
        request->send(200, "text/plain", "Receiver : " + orig_receiver + " -> " + post_receiver + ". Press back button to previous page.");
      }
      pref.end();
    });

    webserver.on("/smstest", HTTP_POST, [](AsyncWebServerRequest *request){
      TinyGsm modem(SerialAT);
      String message;
      //String orig_receiver;
      //String post_receiver;
      String sms_number;

      //pref.begin("tsim", false); 

      bool error_found = false;

      if (request->hasParam("smsnumber", true)) {
        sms_number = request->getParam("smsnumber", true)->value();
      } else {
        message = "SMS NUMBER not found";
        error_found = true;
      }

      if (sms_number == "") {
        message = "SMS NUMBER is empty";
        error_found = true;
      }       

      if (error_found) {
        request->send(200, "text/plain", "Error : " + message + ". Press back button to previous page.");
      } else {
        String imei = modem.getIMEI();
        //String sim_number = modem.getIMSI();
        //String sim_number = modem.sendAT("AT+CNUM");
        //String sim_number;
        //modem.sendAT("+CNUM");
        //modem.waitResponse(1000L, sim_number);
        //bool res = modem.sendSMS(sms_number, String("Hello from ") + imei + String(" ") + sms_number);
        bool res = modem.sendSMS(sms_number, String("Hello from ") + sim_number);
        Serial.print("Send sms message ");
        Serial.println(res ? "OK" : "fail");

        //orig_receiver = pref.getString("receiver");
        //pref.putString("receiver", post_receiver);
        request->send(200, "text/plain", "SMS sent. Press back button to previous page.");
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
      String orig_backup_webhook_url;
      String orig_port;
      String orig_sender;
      String orig_receiver;

      orig_post_wifi_ssid = pref.getString("wifi-ssid");
      orig_post_wifi_password = pref.getString("wifi-password");
      orig_webhook_url = pref.getString("webhook-url");
      orig_backup_webhook_url = pref.getString("b-webhook-url");
      orig_port = pref.getString("port");
      orig_sender = pref.getString("sender");
      orig_receiver = pref.getString("receiver");
      
      request->send(200, "text/html", "wifi SSID : " + orig_post_wifi_ssid + " . <br> wifi PASSWORD : " + orig_post_wifi_password + " . <br> Webhook URL : " + orig_webhook_url + " . <br> Backup Webhook URL : " + orig_backup_webhook_url + " . <br> Sender : " + orig_sender + " . <br> Receiver : " + orig_receiver +" . <br> Port : " + orig_port);

      pref.end();
    });


  webserver.onNotFound(notFound);

  webserver.begin();
}