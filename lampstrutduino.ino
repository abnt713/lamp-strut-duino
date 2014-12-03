#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"
#include <string.h>

#include <JsonGenerator.h>
using namespace ArduinoJson::Generator;

static uint8_t mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0xD1, 0x04 };

int pin = 9;
int lightStatus = 0;
int bright = 0;
long consumption;

static uint8_t ip[] = { 192, 168, 0, 30 };

/* This creates an instance of the webserver.  By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);

void getConfigs(JsonObject<1> &json){
  json["type"] = "lamp";
}


void getBrightParameter(JsonArray<1> json){
  json.add("brilho");
}

void getStatus(JsonObject<2> &json){
  json["ligada"] = lightStatus;
  json["brilho"] = bright;
}

void indexCmd(WebServer &server, WebServer::ConnectionType type, char *, bool){
  server.httpSuccess();
  
  JsonObject<3> root;
  JsonObject<1> configs;
  JsonObject<3> services;
  JsonObject<2> jsonstatus;
  
  getConfigs(configs);
  
  JsonArray<0> onjson;
  services["ligar"] = onjson;
  
  JsonArray<0> offjson;
  services["desligar"] = offjson;

  JsonArray<1> brilhojson;
  brilhojson.add("brilho");
  services["definir_brilho"] = brilhojson;
  
  getStatus(jsonstatus);
  
  root["configs"] = configs;
  root["services"] = services;
  root["status"] = jsonstatus;
  
  root.printTo(server);
}

/* commands are functions that get called by the webserver framework
 * they can read any posted data from client, and they output to the
 * server to send data back to the web browser. */
void onCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  server.httpSuccess();
  
  bright = 100;
  analogWrite(pin, getBrightValue(bright));
  lightStatus = 1;
  
  JsonObject<3> root;
  JsonObject<1> configs;
  JsonObject<3> services;
  JsonObject<2> jsonstatus;
  
  getConfigs(configs);
  
  JsonArray<0> onjson;
  services["ligar"] = onjson;
  
  JsonArray<0> offjson;
  services["desligar"] = offjson;

  JsonArray<1> brilhojson;
  brilhojson.add("brilho");
  services["definir_brilho"] = brilhojson;
  
  getStatus(jsonstatus);
  
  root["configs"] = configs;
  root["services"] = services;
  root["status"] = jsonstatus;
  
  root.printTo(server);
  
  /* this is a special form of print that outputs from PROGMEM */
//  server.printP(helloMsg);
}

void offCmd(WebServer &server, WebServer::ConnectionType type, char *, bool){
  server.httpSuccess();
  
  analogWrite(pin, 0);
  lightStatus = 0;
  JsonObject<3> root;
  JsonObject<1> configs;
  JsonObject<3> services;
  JsonObject<2> jsonstatus;
  
  getConfigs(configs);
  
  JsonArray<0> onjson;
  services["ligar"] = onjson;
  JsonArray<0> offjson;
  services["desligar"] = offjson;
  JsonArray<1> brilhojson;
  brilhojson.add("brilho");
  services["definir_brilho"] = brilhojson;
  
  getStatus(jsonstatus);
  
  root["configs"] = configs;
  root["services"] = services;
  root["status"] = jsonstatus;
  
  root.printTo(server);
}

void brightCmd(WebServer &server, WebServer::ConnectionType type, char * tailUrl, bool tailComplete){
  server.httpSuccess();
  
  if(strlen(tailUrl)){
    char* token = strtok(tailUrl, "=");
    token = strtok(NULL, "=");
    
    int possiblebright = atoi(token);
    if (possiblebright >= 0 && possiblebright <= 100){
      bright = possiblebright;
    }
      
  }
  
  if (lightStatus == 1){
    analogWrite(pin, getBrightValue(bright));
  }
  JsonObject<3> root;
  JsonObject<1> configs;
  JsonObject<3> services;
  JsonObject<2> jsonstatus;
  
  getConfigs(configs);
  
  JsonArray<0> onjson;
  services["ligar"] = onjson;
  JsonArray<0> offjson;
  services["desligar"] = offjson;
  JsonArray<1> brilhojson;
  brilhojson.add("brilho");
  services["definir_brilho"] = brilhojson;
  
  getStatus(jsonstatus);
  
  root["configs"] = configs;
  root["services"] = services;
  root["status"] = jsonstatus;
  
  root.printTo(server);
}

void extrasCmd(WebServer &server, WebServer::ConnectionType type, char *, bool){
  JsonObject<1> root;
  JsonArray<1> extras;
  root["extras"] = extras;
  root.printTo(server);
}
  

void setup()
{
  /* initialize the Ethernet adapter */
  Ethernet.begin(mac, ip);
  pinMode(pin, OUTPUT);
  /* setup our default command that will be run when the user accesses
   * the root page on the server */
  webserver.setDefaultCommand(&indexCmd);

  /* run the same command if you try to load /index.html, a common
   * default page name */
  webserver.addCommand("services/ligar", &onCmd);
  webserver.addCommand("services/desligar", &offCmd);
  webserver.addCommand("services/definir_brilho", &brightCmd);
  webserver.addCommand("get_extras", &extrasCmd);

  /* start the webserver */
  webserver.begin();
}

void loop()
{
  char buff[64];
  int len = 64;

  /* process incoming connections one at a time forever */
  webserver.processConnection(buff, &len);
  /*
  loops += 1;
  if (loops > MAX_LOOPS){
    if (lightStatus == 1){
      consumption += (bright * POWER) / 10000;
    }
    loops = 0;
  }
  */
}

int getBrightValue(int percentValue){
  int maxValue = 127;
  int minValue = 0;
  
  return (percentValue * maxValue) / 100;
}
