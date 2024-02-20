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
#include "WifiModule.h"

#include <SPI.h>
#include <SD.h>
#include <HTTPClient.h>

#include <WiFi.h>

#include "webServerModule.h"

#include <Preferences.h>

#include "TFCardModule.h"

// Variable to store the HTTP request
String header;
String webhook_url;
String backup_webhook_url;
String payload_data;
String port;
String sender;
String receiver;

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

#define SMS_TARGET SMS_Number //Change the number you want to send sms message

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
char smsBuffer[250];

String smsStatus,senderNumber,receivedDate,msg,smsIndex;

//void updateSerial();

void parseData(String buff){
  Serial.println("Start parseData");
  Serial.println(buff);
  Serial.println("End of buff");

  unsigned int len, index, startindex, endindex;

  startindex = buff.indexOf("=");
  endindex = buff.indexOf("\r");
  smsIndex = buff.substring(startindex+1, endindex);
  Serial.println("smsindex= " + smsIndex);

  //////////////////////////////////////////////////
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //////////////////////////////////////////////////
  
  //////////////////////////////////////////////////
  if(buff != "OK"){
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
    
    buff.remove(0, index+2);
    
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length()); 
      temp = "AT+CMGR=" + temp + "\r"; 
      //get the message stored at memory location "temp"
      SerialAT.println(temp); 
    }
    else if(cmd == "+CMGR"){
      extractSms(buff);
      SerialAT.print("AT+CMGD=");
      SerialAT.println(smsIndex);
      
      //if(senderNumber == PHONE){
      //  doAction();
      //}
    }
  //////////////////////////////////////////////////
  }
  else{
  //The result of AT Command is "OK"
  }
}

//************************************************************
void extractSms(String buff){
  AppendLog (buff);
  unsigned int index, endindex;;
   
  index = buff.indexOf(",");
  smsStatus = buff.substring(1, index-1); 
  buff.remove(0, index+2);

  index = buff.indexOf(",");
  senderNumber = buff.substring(0, index-1);
  buff.remove(0,index+5);

  index = buff.indexOf("\r");
  receivedDate = buff.substring(0, index-1);
  buff.remove(0,buff.indexOf("\r"));
  buff.trim();
    
  index =buff.indexOf("\n\r");
  buff = buff.substring(0, index);
  buff.trim();
  msg = buff;
  buff = "";
  msg.toLowerCase();
  Serial.println("Extract Msg"); 
  Serial.println(msg);

  payload_data = "?port=" + port + "&sender=" + sender + "&receiver=" + receiver;
  //String httpRequestData = "Sender: " + sender + "&sender=" + senderNumber + "&message=" + msg;   
  String httpRequestData = "Sender: " + sender + "\n\rReceiver: " + receiver + "\n\rSMSC: " + receiver + "\n\rSCTS: " + receiver + "\n\r\n\r" + msg;
  
  HTTPClient http_client;

  Serial.println("Forward SMS to primary webhook.");
  http_client.begin(webhook_url+payload_data);
  http_client.addHeader("Content-Type", "text/plain");
  http_client.addHeader("Sender-Number", senderNumber);
  http_client.addHeader("TimeStamp", receivedDate);  
  http_client.addHeader("Message-Content", msg);
  //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14&sender=" + senderNumber + "&msg=" + msg;           
  //String httpRequestData = "timestamp=" + receivedDate + "&sender=" + senderNumber + "&message=" + msg;       
      // Send HTTP POST request
  int httpResponseCode = http_client.POST(httpRequestData);
  //int httpResponseCode = http_client.POST(httpRequestData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
        
      // Free resources
  http_client.end();

  Serial.println("Forward SMS to backup webhook.");
  http_client.begin(backup_webhook_url+payload_data);
  http_client.addHeader("Content-Type", "text/plain");
  http_client.addHeader("Sender-Number", senderNumber);
  http_client.addHeader("TimeStamp", receivedDate);  
  http_client.addHeader("Message-Content", msg);
  //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14&sender=" + senderNumber + "&msg=" + msg;           
  //httpRequestData = "timestamp=" + receivedDate + "&sender=" + senderNumber + "&message=" + msg;       
      // Send HTTP POST request
  httpResponseCode = http_client.POST(httpRequestData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
        
      // Free resources
  http_client.end();
}

void setup()
{
  Serial.begin(115200);

  // Connect WiFi
  connectWiFi();

  // If Wifi Failed, start AP
  if (!WiFiUp) {
    initWiFiap();
  }

  // Start Web Server
  startWebServer();

  Preferences pref;
  pref.begin("tsim", false); 
  webhook_url= pref.getString("webhook-url");
  backup_webhook_url= pref.getString("b-webhook-url");
  port= pref.getString("port");
  sender= pref.getString("sender");
  receiver= pref.getString("receiver");
  pref.end();

  if (webhook_url == "") {
    webhook_url = default_WebHookURL;
  }

  if (backup_webhook_url == "") {
    backup_webhook_url = default_Backup_WebHookURL;
  }

  //Serial.println("Test SD Card.");
  //SDCardTest ();

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

    Serial.print("Init completed");
    //Serial.print("Init success, start to send message to  ");
    //Serial.println(SMS_TARGET);

    //String imei = modem.getIMEI();
    //bool res = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);

    //Serial.print("Send sms message ");
    //Serial.println(res ? "OK" : "fail");
}


void loop()
{
    //if (SerialAT.available()) {
    //    SerialAT.write(Serial.read());
    //}
    //if (SerialAT.available()) {
    //    SerialAT.write(Serial.read());
    //}
    //delay(1000);

  while(SerialAT.available()){
    parseData(SerialAT.readString());
  }

  while(Serial.available())  {
    SerialAT.println(Serial.readString());
  }

  //Serial.print("looping");
  //delay(1000);
  
/*
  updateSerial();
  static String currentLine = "";
  while (SerialAT.available()) {
    char c = SerialAT.read();
    if (c == '\n') {
      currentLine.trim(); // Remove leading/trailing whitespace
      if (currentLine.startsWith("+CMTI: \"SM\",")) {
        // Extract SMS index from the line
        int smsIndex = currentLine.substring(13).toInt();
        // Read the SMS with the extracted index using AT commands
        SerialAT.print("AT+CMGR=");
        SerialAT.println(smsIndex);
        delay(100); // Allow time for response
        // Process the SMS message
        if (SerialAT.find("+CMGR:")) {
          String receivedMessage = SerialAT.readStringUntil('\n');
          receivedMessage.trim();
          SerialMon.println("Received SMS: " + receivedMessage);
          // Store the received message in the buffer
          receivedMessage.toCharArray(smsBuffer, sizeof(smsBuffer));
          SerialMon.println("Stored in buffer: " + String(smsBuffer));
          // Delete the SMS after processing
          SerialAT.print("AT+CMGD=");
          SerialAT.println(smsIndex);
          delay(100); // Allow time for response
        }
      }
      currentLine = ""; // Clear the line
    } else {
      currentLine += c; // Append character to the line
    }
  }
  //int rdn = random(100); // random number trigger event based sms or conditinal sms trigger
  //SMS_Message = "HELLO FROM ESP32";
  //if (rdn == 10) {
  //  if (modem.sendSMS(SMS_TARGET, SMS_Message)) {
  //    SerialMon.println(SMS_Message);
  //  }
  //  else {
  //    SerialMon.println("SMS failed to send");
  //  }
  //}
  delay(1000);    
*/
}