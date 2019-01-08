#define DEBUG_SERIAL //uncomment for Serial debugging statements

#ifdef DEBUG_SERIAL
#define DEBUG_BEGIN Serial.begin(115200)
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x) 
#define DEBUG_BEGIN
#endif

//includes
#include <ArduinoJson.h>
#include <PersWiFiManager.h>      //https://github.com/debsahu/PersWiFiManager
//                                //https://github.com/me-no-dev/ESPAsyncTCP
//                                //https://github.com/me-no-dev/ESPAsyncWebServer
//                                //https://github.com/devyte/ESPAsyncDNSServer
//                                //https://github.com/me-no-dev/ESPAsyncUDP
#include <ESP8266WiFi.h>

#define DEVICE_NAME "ESP8266 DEVICE"

//server objects
AsyncWebServer server(80);
AsyncDNSServer dnsServer;
PersWiFiManager persWM(server, dnsServer);

////// Sample program data
int x;
String y;

void setup() {
  DEBUG_BEGIN; //for terminal debugging
  DEBUG_PRINT();

  //allows serving of files from SPIFFS
  SPIFFS.begin();
  //sets network name for AP mode
  persWM.setApCredentials(DEVICE_NAME);
  //persWM.setApCredentials(DEVICE_NAME, "password"); optional password
  
  persWM.setFSCredentials("admin","password"); //SPIFFs: http://<IP>/edit
  persWM.begin();

  //handles commands from webpage, sends live data in JSON format
  server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request) {
    DEBUG_PRINT("server.on /api");
    if (request->hasArg("x")) {
      x = request->arg("x").toInt();
      DEBUG_PRINT(String("x: ")+x);
    } //if
    if (request->hasArg("y")) {
      y = request->arg("y");
      DEBUG_PRINT("y: "+y);
    } //if

    //build json object of program data
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["x"] = x;
    json["y"] = y;

    char jsonchar[200];
    json.printTo(jsonchar); //print to char array, takes more memory but sends in one piece
    request->send(200, "application/json", jsonchar);

  }); //server.on api

  server.begin();
  DEBUG_PRINT("setup complete.");
} //void setup

void loop() {

  // do stuff with x and y

} //void loop