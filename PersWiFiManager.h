#ifndef PERSWIFIMANAGER_H
#define PERSWIFIMANAGER_H

#include <FS.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>       //https://github.com/me-no-dev/ESPAsyncTCP
#include <ESPAsyncWebServer.h> //https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncDNSServer.h> //https://github.com/devyte/ESPAsyncDNSServer
//                             //https://github.com/me-no-dev/ESPAsyncUDP
#include <SPIFFSEditor.h>
#include <Arduino.h>
#include <Ticker.h>

//#define WIFI_HTM_PROGMEM
//#define WIFI_HTM_GZ_PROGMEM
//#define WIFI_HTM2_PROGMEM
#define WIFI_HTM2_GZ_PROGMEM

#define WIFI_CONNECT_TIMEOUT 30
#define WIFI_RETRY_TIME 60
#define PERSWIFI_DEBUG  1
//#define DEBUG_WIFI_MULTI(fmt, ...) if(Serial) Serial.printf( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
#define DEBUG_WIFI_MULTI(fmt,...)

struct persWifiAPEntry {
    char * ssid;
    char * passphrase;
    unsigned long ID;
};

typedef std::vector<persWifiAPEntry> persWifiAPlist;

class PersWiFiManager
{

public:
  typedef std::function<void(void)> WiFiChangeHandlerFunction;

  PersWiFiManager(AsyncWebServer &s, AsyncDNSServer &d, const fs::FS& fs = SPIFFS);
  ~PersWiFiManager();

  bool attemptConnection(const String &ssid = "", const String &pass = "");

  void setupWiFiHandlers();

  bool begin(const String &ssid = "", const String &pass = "");

  String getApSsid();

  void setApCredentials(const String &apSsid, const String &apPass = "");

  void setFSCredentials(const String &http_user = "admin", const String &http_pass = "password");

  void setConnectNonBlock(bool b);

  void handleWiFi();

  void startApMode();

  void onConnect(WiFiChangeHandlerFunction fn);

  void onDisConnect(WiFiChangeHandlerFunction fn);

  void onAp(WiFiChangeHandlerFunction fn);

  void onStore(WiFiChangeHandlerFunction fn);

  bool addAP(const char* ssid, const char *passphrase = NULL);
  bool removeAP(const char* ssid);
  bool existsAP(const char* ssid, const char *passphrase = NULL);
  persWifiAPlist getAPlist();
  wl_status_t GetStatus();


private:
  persWifiAPlist APlist;
  bool APlistAdd(const char* ssid, const char *passphrase = NULL);
  bool APlistRemove(const char* ssid);
  bool APlistExists(const char* ssid, const char *passphrase = NULL);
  void APlistClean(void);

private:

  Ticker _tkWiFiH;
  fs::FS _fs;
  AsyncWebServer *_aserver;
  AsyncDNSServer *_adnsServer;
  String _apSsid, _apPass;
  String _fsUser = "admin", _fsPass = "password";

  bool _connectNonBlock;
  bool _autoReconnect;
  bool _scanSorted;
  unsigned long _connectStartTime;
  unsigned long _scanTime;


  WiFiChangeHandlerFunction _connectHandler;
  WiFiChangeHandlerFunction _disconnectHandler;
  WiFiChangeHandlerFunction _apHandler;
  WiFiChangeHandlerFunction _storeHandler;

}; //class

#endif
