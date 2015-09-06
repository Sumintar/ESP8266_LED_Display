#include <wiringPi.h>
//#include <string>
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
//string message = "This is a test       ";
//int length = message.length();

void writeDisplay(void);
void shiftOut(uint8_t dataPinA, uint8_t dataPinB, uint8_t clockPin, uint8_t valA, uint8_t valB);
void pinSetup(void);


// Main
int main (void)
{
  wiringPiSetup () ;
  pinSetup();
  for (;;)
  {
    for(uint8_t letter = 0; letter < 96; letter++){// Scroll through entire font
      for(uint8_t i = 0; i < fontInfo[letter][0]*2 + 1; i+=2){
        if(fontInfo[letter][0]*2 == i){// Place one pixel spaces between each character

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

          //buffer[127] = pgm_read_byte_near(font + fontInfo[message[letter]-32][1] + i + 1);
          //buffer[126] = pgm_read_byte_near(font + fontInfo[message[letter]-32][1] + i);

          buffer[127] = font[fontInfo[letter][1] + i + 1];
          buffer[126] = font[fontInfo[letter][1] + i];
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

// Pin setup
void pinSetup(void){
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(outputEn, PWM_OUTPUT);
  pinMode(dataPinA, OUTPUT);
  pinMode(dataPinB, OUTPUT);
  pwmWrite(outputEn, brightness);
}
