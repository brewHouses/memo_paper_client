#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include "epd.h"        // e-Paper driver
#include "config.h"

const char* ssid = "abc";
const char* password = "12345678";

int registed_flag = 0;
String update_time("_");

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
  Serial.println(update_time);
  HTTPClient http;
  http.begin(IP, PORT, "/api/paper_hb_handler");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // send itslef's id and last update_time
  int httpCode = http.POST(String("paper_id=")+ID+String("&update_time=")+update_time);                                                                
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
    http.end();
    if(update_time == "_"  && payload.length() > 1){
      update_time = "0";
      //return 1;
    }
    if (payload[0] == 'Y')
      return 1;
    return 0;        
  }
}

void get_last_time_from_server(const char *api){
  HTTPClient http;
  http.begin(IP, PORT, api);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // send itslef's id and last update_time
  int httpCode = http.POST(String("paper_id=")+ID);                                                                
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
    update_time = payload;
    http.end();        
  }
}

void paper_show(){
    EPD_dispInit();
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
    EPD_dispInit();
    EPD_loadA_init("1");
    EPD_loadA_init("2");
    EPD_showB();
}

void paper_show_memo(){
    EPD_dispInit();
    EPD_loadA_memo("1");
    EPD_loadA_memo("2");
    EPD_showB();
    get_last_time_from_server("/api/paper_hb_querry/get_last_time");
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
  heart_beat();
  delay(HB_INTERVAL);
  heart_beat();
}

void loop(void) {
  // if heart_beat return True, then we can get update from server
  // if false, no update in server
  if(heart_beat()){
      paper_show_memo();
      delay(HB_INTERVAL);
  }
  else
      delay(HB_INTERVAL);
}
