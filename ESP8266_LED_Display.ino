//**************************************************************
//  Name    : 16x16 LED Display Test                                
//  Author  : Matt Levine
//  Date    : 27 Nov, 2013    
//  Version : 1.0                                             
//  Notes   :                          
//****************************************************************
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <pgmspace.h>
#include "font.h" // ascii offset of 32
#include "config.h"

MDNSResponder mdns;

ESP8266WebServer server(80);

int outputEn = 0;
int latchPin = 2;
int clockPin = 5;
int dataPinA = 4;
int dataPinB = 16;

uint8_t buffer[128];
String message = "#HIGHFEST brought to you by: 36 N. High St. #THEZOO               ";
int length = message.length();

void setup(void){
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(outputEn, OUTPUT);
  pinMode(dataPinA, OUTPUT);
  pinMode(dataPinB, OUTPUT);
  analogWrite(outputEn, 1000);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/msg", handleMsg);

  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  //int length = message.length();
  for(uint8_t letter = 0; letter < length; letter++){// Scroll through entire font  
    for(uint8_t i = 0; i < fontInfo[message[letter]-32][0]*2 + 1; i+=2){
      server.handleClient();
      if(fontInfo[message[letter]-32][0]*2 == i){// Place one pixel spaces between each character

        for(uint8_t j = 0; j < 128; j+=2){
          buffer[j] = buffer[j+2];
          buffer[j+1] = buffer[j+3];
        }

        buffer[127]=0x00;
        buffer[126]=0x00;
      }else{    

        for(uint8_t j = 0; j < 128; j+=2){// Scroll buffer one pixel at a time
          buffer[j] = buffer[j+2];
          buffer[j+1] = buffer[j+3];
        }

        buffer[127] = pgm_read_byte_near(font + fontInfo[message[letter]-32][1] + i + 1);
        buffer[126] = pgm_read_byte_near(font + fontInfo[message[letter]-32][1] + i);
      }
      writeDisplay();// Write buffer to display
      delay(40);
    }
  }
} 

//
// User functions
//

void allOn(void){
  
}

void halt(void){
   while(true){} 
}

void writeDisplay(void){
  digitalWrite(latchPin, LOW);
  //PORTB = PORTB | (1<<2);
  for(uint8_t i = 0; i < 128; i+=2){
    shiftOut(dataPinA, dataPinB, clockPin, MSBFIRST, buffer[i], buffer[i+1]);
  }
  digitalWrite(latchPin, HIGH);
}

void shiftOut(uint8_t dataPinA, uint8_t dataPinB, uint8_t clockPin, uint8_t bitOrder, uint8_t valA, uint8_t valB){
  uint8_t i;
  for (i = 0; i < 8; i++)  {
    if(bitOrder == LSBFIRST){
      digitalWrite(dataPinA, !!(valA & (1 << i)));
      digitalWrite(dataPinB, !!(valB & (1 << i)));
    }else{      
      digitalWrite(dataPinA, !!(valA & (1 << (7 - i))));
      digitalWrite(dataPinB, !!(valB & (1 << (7 - i))));
    }      
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);            
  }
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleMsg(){
  message = server.arg(0);
  message.replace("%20"," ");
  message.replace("%23","#");
  length = message.length();
  Serial.print("Message: ");
  Serial.println(message);
  server.send(200, "text/plain", "Success");
}

