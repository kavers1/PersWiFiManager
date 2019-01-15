//includes
#include <FS.h>
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
  Serial.begin(115200); //for terminal debugging
  Serial.println();

  //sets network name for AP mode
  persWM.setApCredentials(DEVICE_NAME);
  //persWM.setApCredentials(DEVICE_NAME, "password"); optional password

  persWM.onConnect([]() {
    if (Serial) { 
      Serial.print("Router IP: ");
      Serial.println(WiFi.localIP());
    }
  });
  persWM.onAp([](){
    if (Serial) { 
    Serial.print("AP Mode, IP: ");
    Serial.println(persWM.getApSsid());
    }
  });

  //make connecting/disconnecting non-blocking
  //persWM.setConnectNonBlock(true); //non blocking mode is enabled by default

  persWM.setFSCredentials("admin","password"); //SPIFFs: http://<IP>/edit
  persWM.begin();
  //SPIFFS.format();

  //handles commands from webpage, sends live data in JSON format
  server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("server.on /api");
    if (request->hasArg("x")) {
      x = request->arg("x").toInt();
      Serial.println(String("x: ")+x);
    } //if
    if (request->hasArg("y")) {
      y = request->arg("y");
      Serial.println("y: "+y);
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
  Serial.println("----------------------\nsetup complete.");
} //void setup

void loop() {
  //in non-blocking mode, handleWiFi must be called in the main loop
  //persWM.handleWiFi(); // no need
  // do stuff with x and y

} //void loop