#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "epd.h"        // e-Paper driver

const char* ssid = "abc";
const char* password = "12345678";
#define ID "y4e213"
#define PARA(a,b,c) a##b##c
#define IP "10.2.5.49"
#define PORT 8000

int registed_flag = 0;
String update_time;

//const char[] init_page = [];

ESP8266WebServer server(80);
IPAddress myIP;       // IP address in your local wifi net

void server_connection_test(const char* api){
  HTTPClient http;
  http.begin(IP, PORT, api);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(String("user_id=")+ID);                                                                 
  if (httpCode > 0) {
    String payload = http.getString();
    http.end(); 
    if(payload == "UPDATE"){
        Serial.println("OK");
    }
        Serial.println(payload);
    }
}

void get_init_code(const char* api){
  HTTPClient http;
  http.begin(IP, PORT, api);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(String("paper_id=")+ID);                                                                 
  if (httpCode > 0) {
    String payload = http.getString();
    http.end(); 
    paper_show();
  }
}



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
  
  //server_connection_test("/api/paper_hb_handler");
  paper_show_init();
}

String req_service(const char* url){
  HTTPClient http;

  http.begin("10.2.5.49", 3000, "/");
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

// if the server have some update, then return True
// else return False
char heart_beat(){
  HTTPClient http;

  http.begin(IP, PORT, "/paper_hb_handler");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // send itslef's id and last update_time
  int httpCode = http.POST(String("paper_id=")+ID+String("&update_time=")+update_time);                                                                
  if (httpCode > 0) {
    String payload = http.getString();
    http.end(); 
    if (payload[0] == 'Y')
      return 1;
    return 0;        
  }
}

void paper_show(){
    EPD_Init();
    EPD_loadA();
    EPD_loadA();
    EPD_showB();
}

void paper_show_init(){
    int flag = EPD_loadA_init("0");
    if(flag){
      registed_flag = 1;
      return;
    }
    EPD_Init();
    EPD_loadA_init("1");
    EPD_loadA_init("2");
    EPD_showB();
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
