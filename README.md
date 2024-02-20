# LILYGO_T_SIM_A7608E-H

This is the project that I use  
LILYGO® T-SIM A7608E-H No GPS ESP32 LTE Cat 4 High Speed 4G Network GPS Antenna Wireless WiFi Bluetooth Development Board  
to forward incoming sms to web server and store the sms in SD card as backup.  

This code will contain the following functions:
- web console to change configuration
- retrieve the configuration store in the flash memory from the web console
- connect to wifi during startup
- fallback to AP mode if wifi failed to connect
- forward the sms to 2 webhook URLs
- store the sms into the SD card

This is the pin diagram of the board.
 
 ![alt text](https://github.com/stevenfoong/LILYGO_T_SIM_A7608E-H/blob/main/image/T-A7608E-H.jpg)

This is the web console.

 ![alt text](https://github.com/stevenfoong/LILYGO_T_SIM_A7608E-H/blob/main/image/web-console.png)

 I develop it using Arduino IDE 2.2.1  
 - board : ESP32 Dev Module
 - PSRAM : Enabled
 - Upload Speed : 115200

Library Needed:
 - ESP32 Board Manager by Espressif Systems
 - ESPAsyncWebSrv by dvarrel

References:
 - https://github.com/Xinyuan-LilyGO/T-A7608X/tree/main (archived and read-only)
 - https://github.com/Xinyuan-LilyGO/LilyGO-T-A76XX/tree/main (latest and compatible code)
