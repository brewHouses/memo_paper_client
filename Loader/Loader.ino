#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "scripts.h"    // JavaScript code
#include "css.h"        // Cascading Style Sheets
#include "html.h"       // HTML page of the tool
#include "epd.h"        // e-Paper driver

const char* ssid = "abc";
const char* password = "12345678";

ESP8266WebServer server(80);
IPAddress myIP;       // IP address in your local wifi net

void setup(void) {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // Connect to WiFi network
  Serial.println("");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  // SPI initialization
  pinMode(CS_PIN  , OUTPUT);
  pinMode(RST_PIN , OUTPUT);
  pinMode(DC_PIN  , OUTPUT);
  pinMode(BUSY_PIN,  INPUT);
  SPI.begin();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\r\nIP address: ");
  Serial.println(myIP = WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
  EPD_Init();
  EPD_loadA();
  EPD_loadA();
  EPD_showB();
  //req_service("192.168.2.194:3000");
}

String req_service(const char* url){
  HTTPClient http;

  http.begin("192.168.2.194", 3000, "/");
  int httpCode = http.GET();                                                                 
  if (httpCode > 0) {
    String payload = http.getString();
    http.end(); 
    return payload;
    }
    http.end(); 
    String x;
    return x;
  
}

void heart_beat(){
    req_service("some url");
}

void loop(void) {
  server.handleClient();
}

void EPD_Init()
{

  EPD_dispInit();

}

void EPD_Load()
{
  //server.arg(0) = data+data.length+'LOAD'
  String p = server.arg(0);
  if (p.endsWith("LOAD")) {
    int index = p.length() - 8;
    int L = ((int)p[index] - 'a') + (((int)p[index + 1] - 'a') << 4) + (((int)p[index + 2] - 'a') << 8) + (((int)p[index + 3] - 'a') << 12);
    if (L == (p.length() - 8)) {
      Serial.println("LOAD");
      // if there is loading function for current channel (black or red)
      // Load data into the e-Paper
      if (EPD_dispLoad != 0) EPD_dispLoad();
    }
  }
  server.send(200, "text/plain", "Load ok\r\n");
}

void EPD_Next()
{
  Serial.println("NEXT");

  // Instruction code for for writting data into
  // e-Paper's memory
  int code = EPD_dispMass[EPD_dispIndex].next;

  // If the instruction code isn't '-1', then...
  if (code != -1)
  {
    // Do the selection of the next data channel
    EPD_SendCommand(code);
    delay(2);
  }
  // Setup the function for loading choosen channel's data
  EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chRd;

  server.send(200, "text/plain", "Next ok\r\n");
}

void EPD_Show()
{
  Serial.println("SHOW");
  // Show results and Sleep
  EPD_dispMass[EPD_dispIndex].show();
  server.send(200, "text/plain", "Show ok\r\n");
}
