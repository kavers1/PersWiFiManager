# PersWiFiManager Async
Persistent WiFiManager Arduino library for ESP8266-based microcontrollers

- Same idea, but Async
- Uses tickers, upgrade ardiono-esp8266 core to v **2.5.0-beta2 or greater**
- SPIFFS editor http://<IP_Address>/edit
- `persWM.setFSCredentials("admin","passsword");` // used for SPIFFS editor credentials
- `persWM.setConnectNonBlock(true);` is obsolete, non-blocking is done in this code

## Libraries needed
|Library               |Link                                           |Purpose      |
|----------------------|-----------------------------------------------|-------------|
|ESPAsyncUDP           |https://github.com/me-no-dev/ESPAsyncUDP       |comm         |
|ESPAsyncTCP           |https://github.com/me-no-dev/ESPAsyncTCP       |comm         |
|ESPAsyncWebServer     |https://github.com/me-no-dev/ESPAsyncWebServer |web          |
|ESPAsyncDNSServer     |https://github.com/devyte/ESPAsyncDNSServer    |dns          |
|ArduinoJson           |https://github.com/bblanchon/ArduinoJson       |example      |

## Info from r-downing/PersWiFiManager
This Persistent WiFi Manager provides a WiFi Settings web interface for ESP8266-based microcontrollers. It can autoconnect to the last used network, and then if unsuccessful, switch to AP mode with a captive DNS server. This action is non-blocking; the program will proceed to the main loop after, regardless of WiFi connection status. In AP mode, any device (phone, computer, etc) can connect to this network to configure the wifi connection, or access the device as a webserver. 

This library and UI was inspired by tzapu's [WiFiManager library](https://github.com/tzapu/WiFiManager). The main difference is that it allows the program to continue functioning normally, even in AP mode. It is also more memory efficient, as it does not have to dynamically build the page, and can serve it from SPIFFS rather than PROGMEM. 

[Full Documentation](http://ryandowning.net/PersWiFiManager)
