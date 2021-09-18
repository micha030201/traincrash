#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 8
#define DATA_PIN A2
#define FORWARD 0
#define BACKWARD 1
#define SLOW 250
#define MEDIUM 50
#define FAST 5

CRGB leds[NUM_LEDS];

#pragma pack(1)

typedef struct paket
{
   uint8_t pos;
   uint16_t cur;
   uint16_t volt;
} paket;

union buff {
  struct paket p;
  unsigned char mass[6]; 
};

boolean direction = FORWARD;
uint8_t incomingByte;
union buff tmp;

void setup() { 
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  Serial.begin(9600);
}

void loop() { 
  if (Serial.available() >= 6) {
    Serial.readBytes(tmp.mass, 6);
    
    incomingByte = tmp.p.pos;
    Serial.print(tmp.p.pos);
    Serial.print(' ');
    Serial.print(tmp.p.cur);
    Serial.print(' ');
    Serial.print(tmp.p.volt);
    Serial.print('\n');

    

    if(incomingByte == '1'){
      for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = CRGB::Pink;
        FastLED.show();
      }
    }
    else if (incomingByte == '0'){
      for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = CRGB(0, 0, 0);
        FastLED.show();
      }
    }
  }
  delay(1);
}
