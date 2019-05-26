//includes
#include <FS.h>
#include <ArduinoJson.h>
#include <PersWiFiManager.h>      //https://github.com/debsahu/PersWiFiManager
                                //https://github.com/me-no-dev/ESPAsyncTCP
                                //https://github.com/me-no-dev/ESPAsyncWebServer
                                //https://github.com/devyte/ESPAsyncDNSServer
                                //https://github.com/me-no-dev/ESPAsyncUDP
#include <ESP8266WiFi.h>
#include <aeslib.h>

#define DEVICE_NAME "ESP8266 ADEM"
#define MAX_NR_SSID 10
//server objects
AsyncWebServer server(80);
AsyncDNSServer dnsServer;
PersWiFiManager persWM(server, dnsServer);
////// Sample program data
int x;
String y;

AESLib aesLib;

String plaintext = "12345678;";
int loopcount = 0;

char cleartext[256];
char ciphertext[512];

// AES Encryption Key
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// General initialization vector (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Sample strings as generated by node.js server
String server_b64iv = "AAAAAAAAAAAAAAAAAAAAAAAA=="; // same as aes_iv  but in Base-64 form as received from server
String server_b64msg = "ei6NxsBeWk7hj41eia3S0LdkAlm2qxpRbmcsrd23TTc="; // same as aes_iv  but in Base-64 form as received from server

// Generate IV (once)
void aes_init() {
  // workaround for incorrect B64 functionality on first run...
  encrypt((char *) "HELLO WORLD!", aes_iv);

  print_key_iv();

  // reset aes_iv to server-based value
  int ivLen = base64_decode((char*)server_b64iv.c_str(), (char *)aes_iv, server_b64iv.length());
  Serial.print("Decoded IV bytes: ");
  Serial.println(ivLen);
  print_key_iv();
}

String encrypt(char * msg, byte iv[]) {
  int msgLen = strlen(msg);
  char *encrypted = new char [4 * msgLen];
  aesLib.encrypt64(msg, encrypted, aes_key, 128,iv);
  String tmp = String(encrypted);
  delete(encrypted);
  return tmp;
}

String decrypt(char * msg, byte iv[]) {
//  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char *decrypted = new char [msgLen]; // half may be enough
  aesLib.decrypt64(msg, decrypted, aes_key, 128,iv);
  String tmp = String(decrypted);
  delete(decrypted);
  return tmp;
}

void print_key_iv() {

  int i;

  Serial.println("AES IV: ");
  for (i = 0; i < (int) sizeof(aes_iv); i++) {
    Serial.print(aes_iv[i], HEX);
    if ((i + 1) < (int) sizeof(aes_iv)) {
      Serial.print(",");
    }
  }

  Serial.println("");
}


void test_base64(){
String strTest1 = "";
  for(int k = 0; k < 4; k++){
    strTest1 = strTest1 + "U";
      Serial.print(".... [    ]");Serial.println(strTest1);
    
    int i = base64_enc_len(strTest1.length());
    if ((i%4) == 0){
      Serial.print(" ==> [ OK ]");
    }
    else
    {
      Serial.print(" ==> [FAIL]");
    }
    Serial.print("input length :");Serial.print(strTest1.length());
    Serial.print(" is encoded : ");Serial.println(i);
    char out[i];
    int j = base64_encode(out,(char *)strTest1.c_str(),strTest1.length());
    if (j==i){
      Serial.print(" ==> [ OK ]");
    }
    else
    {
      Serial.print(" ==> [FAIL]");
    }
    Serial.print("encoded length  :");Serial.println(j);
    if (String("VQ==VVU=VVVVVVVVVQ==").substring(k*4, k*4 + j).equals(out)){
      Serial.print(" ==> [ OK ]");
    }
    else
    {
      Serial.print(" ==> [FAIL]");
    }
    Serial.print("compare encoded string :");Serial.println(out);
    int m= base64_dec_len(out,j);
    char in[m];
    if (m==(int)strTest1.length()){
      Serial.print(" ==> [ OK ]");
    }
    else
    {
      Serial.print(" ==> [FAIL]");
    }
    Serial.print("decode length :");Serial.println(m);
    base64_decode(in,out,j);
    if (strTest1.equals(in)){
      Serial.print(" ==> [ OK ]");
    }
    else
    {
      Serial.print(" ==> [FAIL]");
    }
    Serial.print("decoded string :");Serial.println(in);
  }
}

/*
// Enable ECB, CTR and CBC mode. Note this can be done before including aes.h or at compile-time.
// E.g. with GCC by using the -D flag: gcc -c aes.c -DCBC=0 -DCTR=1 -DECB=1
#define CBC 1
#define CTR 1
#define ECB 1
#include <AES.hpp>



static void phex(uint8_t* str);
static int test_encrypt_cbc(void);
static int test_decrypt_cbc(void);

// prints string as hex
static void phex(uint8_t* str)
{

#if defined(AES256)
    uint8_t len = 32;
#elif defined(AES192)
    uint8_t len = 24;
#elif defined(AES128)
    uint8_t len = 16;
#endif

    unsigned char i;
    for (i = 0; i < len; ++i){
        Serial.print("0x");
        Serial.print( str[i],HEX);
        Serial.print(", ");
    }
    Serial.println();
    for (i = 0; i < len; ++i)
        Serial.print( (char )str[i]);
    Serial.println();
}



static int test_decrypt_cbc(void)
{

#if defined(AES128)
    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t in[]  = {  0x44, 0x6E, 0xB4, 0xFC, 0x0E, 0xCB, 0x11, 0x14, 0x04, 0x93, 0x05, 0x74, 0xE5, 0x8E, 0x3E, 0xD6};
#endif
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t out[] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', 'A', 'E', 'S', 0x00 };

//  uint8_t buffer[64];
    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, in, 16);
    phex(in);
    Serial.print("\n");
    Serial.print("CBC decrypt: ");

    if (0 == memcmp((char*) out, (char*) in, 16)) {
        Serial.print("SUCCESS!\n");
	return(0);
    } else {
        Serial.print("FAILURE!\n");
	return(1);
    }
}

static int test_encrypt_cbc(void)
{
#if defined(AES128)
    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t out[] = { 0x44, 0x6E, 0xB4, 0xFC, 0x0E, 0xCB, 0x11, 0x14, 0x04, 0x93, 0x05, 0x74, 0xE5, 0x8E, 0x3E, 0xD6};
    
#endif
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t in[]  = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', 'A', 'E', 'S', 0x00 };
    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, key, iv);
    phex(in);
    Serial.print("\n");

    AES_CBC_encrypt_buffer(&ctx, in, 16);
    
    phex(in);
    Serial.print("\n");

    Serial.print("CBC encrypt: ");

    if (0 == memcmp((char*) out, (char*) in, 16)) {
        Serial.print("SUCCESS!\n");
	return(0);
    } else {
        Serial.print("FAILURE!\n");
	return(1);
    }
}
*/



void readAPs(){
  if (SPIFFS.exists("/config.json")) {
    //file exists, reading and loading
    Serial.println("reading config file");
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) {
      Serial.print("....opened config file ");
      Serial.print(ESP.getFreeHeap());
      Serial.print(" --> ");
      // size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      // std::unique_ptr<char[]> buf(new char[size]);
      Serial.print(ESP.getFreeHeap());
      
      // configFile.readBytes(buf.get(), size);
      DynamicJsonDocument jsonDoc(1024);

      DeserializationError error = deserializeJson(jsonDoc, configFile);
      if (!error){
        Serial.println("....parsed json");
        const char *encpwd;
        const char *ssid;
    
        /// Loop over storage size
        JsonArray AP = jsonDoc["AP"];
        Serial.printf("jsonArray of size %d\n", AP.size());
        for(int i=0;i<=10;i++){
          byte dec_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
          Serial.printf("%d : ",i);
          if (!AP[i]["S"].isNull()) {
            ssid = AP[i]["S"];
            Serial.printf("reading : AP[%d] = %s ",i,ssid);
            if (String(ssid).length() > 0) {
              if (!AP[i]["P"].isNull()) {
                encpwd = AP[i]["P"];
                Serial.printf("encrypted password = %s ",encpwd);
                

                String pwd = decrypt( (char*)encpwd, dec_iv);
                Serial.printf("Decrypted password = %s\n",pwd.c_str());
                persWM.addAP(ssid,pwd.c_str());
              }
              else {
                persWM.addAP(ssid);
              }
            }
          }
          else
          {
            break;
          }
        }
        // is this blocking ??? https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiMulti.cpp
        
      } else {
          Serial.println("failed to load json config");
      }
    }
  }
}

void setup() {
  Serial.begin(77400); //for terminal debugging
  Serial.println("Begin Setup");
  dnsServer.start(53,"www.myAdem.local",IPAddress(192, 168, 0, 189));
  //sets network name for AP mode
  persWM.setApCredentials(DEVICE_NAME);
  //persWM.setApCredentials(DEVICE_NAME, "password"); optional password

  persWM.onConnect([]() {
    if (Serial) { 
      Serial.print("Router IP: ");
      Serial.println(WiFi.localIP());
      Serial.print(" On SSID: ");
      Serial.println(WiFi.SSID());
    }
  });
  persWM.onDisConnect([]() {
    if (Serial) { 
      Serial.println("Disconnected ");
    }
    readAPs(); // reread the AP's, they can be changed using edit function
  });

  persWM.onAp([](){
    if (Serial) { 
    Serial.print("AP Mode, IP: ");
    Serial.println(persWM.getApSsid());
    }
  });

  persWM.onStore([]() {
    Serial.print(ESP.getFreeHeap());
    DynamicJsonDocument jsonDoc(1024);
    
    File configFile ;
    if (! SPIFFS.exists("/config.json")) {
      Serial.println("create config file");
    }
    else {
      Serial.println("reading config file");
      Serial.print(ESP.getFreeHeap());
      configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        //size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        //std::unique_ptr<char[]> buf(new char[size]);
        //configFile.readBytes(buf.get(), size);
        DeserializationError error = deserializeJson(jsonDoc, configFile);//buf.get());
        Serial.printf("error %s\n", error.c_str());
        configFile.close();
      }
    }
    Serial.print(ESP.getFreeHeap());
    if (jsonDoc.isNull()){ // create empty document
      jsonDoc.createNestedArray("AP"); 
    }
    serializeJsonPretty(jsonDoc,Serial);
    const char *ssid;
    bool isDirty = false;
    /// Loop over storage size
    /// get count of SSID's
    int count = MAX_NR_SSID;
    JsonArray AP = jsonDoc["AP"];
    if (Serial) { 
      Serial.printf("%s : access point has be entered or update\n",WiFi.SSID().c_str());
    }
    for(auto entry : persWM.getAPlist()) {
      bool isFound = false;
      byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      String encrypted = encrypt(entry.passphrase, enc_iv);
      if (Serial) { 
        Serial.printf("found %s as SSID and password (encrypted) %s\n",entry.ssid,encrypted.c_str());
      }
      for(int i = 0; i < count;i++){
          ssid = AP[i]["S"];
          if (String(entry.ssid).equals(ssid)) {
            isFound = true;
            if (encrypted.equals((const char *)AP[i]["P"])){
              continue;
            }
            else
            {
              AP[i]["P"] = encrypted;
              isDirty = true;
            }
          }
      }
      if(!isFound){
        // new AP --> add to list
        JsonObject newAP = AP.createNestedObject();
        newAP["S"] = entry.ssid;
        newAP["P"] = encrypted;
        isDirty = true;
      }
    }
    serializeJsonPretty(jsonDoc,Serial);
    if (isDirty){
      Serial.println("storing new config file");
      
      configFile = SPIFFS.open("/config.json", "w");
      if (!configFile) {
        Serial.println(F("Failed to open config file"));
      }
      else {
        if (serializeJsonPretty(jsonDoc, configFile) == 0) {
          Serial.println(F("Failed to write to config file"));
        }
        configFile.close();
      }
    }
  });
  
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS opened: " );
    readAPs();
  }
  
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
    StaticJsonDocument<200> jsondoc;
    
    jsondoc["x"] = x;
    jsondoc["y"] = y;

    char jsonchar[200];
    serializeJson(jsondoc, jsonchar,200); //print to char array, takes more memory but sends in one piece
    request->send(200, "application/json", jsonchar);

  }); //server.on api

  server.on("/Adem", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("server.on /Adem");
    request->redirect("http://home.scarlet.be/~la348750/");
    //AsyncWebServerResponse *response = request->beginResponse( 302, "text/plain","");
    //response->addHeader("Location",String("home.scarlet.be/Adem"));
    //request->send(response);
  });

  server.begin();
  Serial.println("----------------------\nsetup complete.");
  
  test_base64();
  aes_init();

  print_key_iv();

  byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
  // first decrypt after init should use aes_iv as given by server to test bare string first
  String decrypted = decrypt((char*)server_b64msg.c_str(), enc_iv); // aes_iv fails here, incorrectly decoded...
  Serial.print("Server Cleartext: ");
  Serial.println(decrypted);

  print_key_iv();

  loopcount = -1;
  Serial.print("size of byte ");Serial.println(sizeof(byte));
  Serial.print("size of int ");Serial.println(sizeof(int));
  Serial.print("size of key ");Serial.println(sizeof(aes_key));
} //void setup

 byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
 byte dec_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
 
void loop() {
  //encrypt();
  //decrypt("ipYk12VCYyD+aJ7KL7lO8L5zOq71XvsLzp650gKBFgQor7GHs98QpQSjQOZdhCwggq2Ehf4nVNwTeK3VjtqMVJRGBw9YViARXCTOGqctjFc=", "+eNzSlRRPi0YZhrp5ctpnA==", 83);
  //delay(8000);

  //in non-blocking mode, handleWiFi must be called in the main loop
  //persWM.handleWiFi(); // no need
  // do stuff with x and y
  loopcount++;
/*
  sprintf(cleartext, ">START; %i<", loopcount);

  print_key_iv();

  // Encrypt
  String encrypted = encrypt(cleartext, enc_iv);
  sprintf(ciphertext, "%s", encrypted.c_str());
  Serial.print("Ciphertext: ");
  Serial.println(encrypted);

  // Decrypt
  String decrypted = decrypt( ciphertext, dec_iv);
  Serial.print("Cleartext: ");
  Serial.println(decrypted);

  if (decrypted.equals(cleartext)){
    Serial.println("SUCCES");
  }
  else
  {
    Serial.println("FAILURE");
    loopcount = 0;
  }
  Serial.print(ESP.getFreeHeap());
  delay(1500);
*/
} //void loop