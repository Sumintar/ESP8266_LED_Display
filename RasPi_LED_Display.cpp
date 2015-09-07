//
// g++ -o led RasPi_LED_Display.cpp -lwiringPi
//
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <wiringPi.h>
#include <signal.h>
#include <string>
#include <cstring>
#include "font.h"

#define uint8_t char
#define MSBFIST 1
#define LSBFIST 0

using namespace std;

// Constants
int outputEn = 1;
int latchPin = 9;
int clockPin = 7;
int dataPinA = 0;
int dataPinB = 2;

int brightness = 960; // range is 0-1024

uint8_t buffer[128];
std::string message = "wubba lubba dub dub      ";
int length = message.length();

// Functions
void writeDisplay(void);
void shiftOut(uint8_t dataPinA, uint8_t dataPinB, uint8_t clockPin, uint8_t valA, uint8_t valB);
void pinSetup(void);
void sigint(int a);
void clearBuffer(void);

// BEGIN Logic
// Main
int main (void)
{
  std::cout << length << std::endl;
  signal(SIGINT,sigint);
  wiringPiSetup () ;
  pinSetup();
  for (;;)
  {
    const char* strdata = message.c_str();

    for(uint8_t letter = 0; letter < message.length(); letter++){// Scroll through entire font
      for(uint8_t i = 0; i < fontInfo[strdata[letter]-32][0]*2 + 1; i+=2){
        if(fontInfo[strdata[letter]-32][0]*2 == i){// Place one pixel spaces between each character

          for(uint8_t j = 0; j < 128; j+=2){
            buffer[j] = buffer[j+2];
            buffer[j+1] = buffer[j+3];
          }

          buffer[127]=0x00;
          buffer[126]=0x00;
        } else {
          ::cout << strdata[letter] << std::endl;
          for(uint8_t j = 0; j < 128; j+=2){// Scroll buffer one pixel at a time
            buffer[j] = buffer[j+2];
            buffer[j+1] = buffer[j+3];
          }

          buffer[127] = font[fontInfo[strdata[letter]-32][1] + i + 1];
          buffer[126] = font[fontInfo[strdata[letter]-32][1] + i];
        }
        writeDisplay();// Write buffer to display
        delay(40);
      }
    }
  }
  return 0 ;
}

void writeDisplay(void){
  digitalWrite(latchPin, LOW);
  //PORTB = PORTB | (1<<2);
  for(uint8_t i = 0; i < 128; i+=2){
    shiftOut(dataPinA, dataPinB, clockPin, buffer[i], buffer[i+1]);
  }
  digitalWrite(latchPin, HIGH);
}

void shiftOut(uint8_t dataPinA, uint8_t dataPinB, uint8_t clockPin, uint8_t valA, uint8_t valB){
  uint8_t i;
  for (i = 0; i < 8; i++)  {
    /*if(bitOrder == LSBFIRST){
      digitalWrite(dataPinA, !!(valA & (1 << i)));
      digitalWrite(dataPinB, !!(valB & (1 << i)));
    }else{*/
      digitalWrite(dataPinA, !!(valA & (1 << (7 - i))));
      digitalWrite(dataPinB, !!(valB & (1 << (7 - i))));
    //}
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }
}

// capture ctrl-c sigint
void sigint(int a) {
  clearBuffer();
  printf("Shutting down...\n");
  exit(a);
}

// writes a blank buffer to display
void clearBuffer(void) {
  memset(buffer, 0, sizeof(buffer));
  writeDisplay();
}

// Pin setup
void pinSetup(void){
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(outputEn, PWM_OUTPUT);
  pinMode(dataPinA, OUTPUT);
  pinMode(dataPinB, OUTPUT);
  pwmWrite(outputEn, brightness);
}
