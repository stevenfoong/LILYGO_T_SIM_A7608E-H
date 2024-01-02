/**
 * @file      SendSMS.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-11-16
 * @note      Not support T-SIM7672
 * *  Although the manual of SIM7672G states that it has the functions of making voice calls and sending text messages, 
 * *  the current firmware does not support it.
 */

#include "utilities.h"
#include "config.h"
#include <SPI.h>
#include <SD.h>
#include <HTTPClient.h>

#include <WiFi.h>



// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;



void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println("Connecting to Internet ..");
  HTTPClient http;
  String url = WebHookURL + "?localip=" + WiFi.localIP();
  //http.begin(serverPath.c_str());
  http.begin("https://www.yahoo.com");
  //http.begin("https://webhook.site/f3046b1d-5ee2-4734-8171-bde61a9dc567?localip=" + WiFi.localIP());
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    Serial.println("Connect to Internet Successfully.");
    //String payload = http.getString();
    // Serial.println(payload);
  } else if (httpResponseCode>0) {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
      // Free resources
  http.end();
}

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

#define SMS_TARGET  SMS_Number //Change the number you want to send sms message

#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

char SeialInBuffer[64];
char SMSbuffer[32];

void setup()
{
  Serial.begin(115200);

  // Connect WiFi
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());



  // Turn on DC boost to power on the modem
  #ifdef BOARD_POWERON_PIN
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);
  #endif



    // Set modem reset pin ,reset modem
    Serial.println("Reset Modem");
    pinMode(MODEM_RESET_PIN, OUTPUT);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL); delay(100);
    digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL); delay(2600);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);

    // Turn on modem
    Serial.println("Turn On Modem");
    pinMode(BOARD_PWRKEY_PIN, OUTPUT);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    delay(100);
    digitalWrite(BOARD_PWRKEY_PIN, HIGH);
    delay(1000);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);

    // Set ring pin input
    pinMode(MODEM_RING_PIN, INPUT_PULLUP);

    // Set modem baud
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

    Serial.println("Start modem...");
    delay(3000);

    while (!modem.testAT()) {
        delay(10);
    }
    Serial.println("\nModem is online");
    // Wait PB DONE
    delay(10000);

    Serial.print("Init success");
    //Serial.print("Init success, start to send message to  ");
    //Serial.println(SMS_TARGET);

    //String imei = modem.getIMEI();
    //bool res = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);

    //Serial.print("Send sms message ");
    //Serial.println(res ? "OK" : "fail");
}

void loop()
{
    if (Serial.available()) {
        SerialAT.write(Serial.read());
    }
    if (Serial.available()) {
        SerialAT.write(Serial.read());
    }
    delay(1);
}
