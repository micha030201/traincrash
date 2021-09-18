#include <FastLED.h>
#include <EEPROM.h>

#include "process.c"

#define NUM_LEDS 8
#define DATA_PIN A2
#define FORWARD 0
#define BACKWARD 1
#define SLOW 250
#define MEDIUM 50
#define FAST 5

static CRGB leds[NUM_LEDS];

#pragma pack(1)

typedef struct paket
{
   uint8_t pos;
   uint16_t cur;
   uint16_t volt;
   uint8_t check;
} paket;

union buff {
  struct paket p;
  unsigned char mass[7]; 
};

union buff tmp;
uint8_t tmp_pos = 0;

void draw() {
  for (int i = 8, j = 0; i < POSITIONS; ++i, ++j) {
    switch (info[i].state) {
      case GREEN:
        leds[j] = CRGB(0, 255, 0);         
        break;
      case RED:
        leds[j] = CRGB(255, 0, 0);
        break;
      case YELLOW:
        leds[j] = CRGB(255, 255, 0);
        break;
      case WHITE:
        leds[j] = CRGB(255, 255, 255);
        break;
      default:
        leds[j] = CRGB(0, 0, 0);
        break;
    }
    //Serial.print(info[i].state);
    //Serial.print(' ');
  }
  //Serial.print('\n');

  FastLED.show();
}

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(15);
  
  for (int i = 0; i < POSITIONS; ++i) {
    //EEPROM.write(i, 0);
    info[i].state = (enum state) EEPROM.read(i);
  }
  draw();
  
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() >= 7) {
    Serial.readBytes(tmp.mass, 7);
    if (tmp.mass[6] != '\n') {
      Serial.readBytes(tmp.mass, 1);
      return;
    }
    if ((tmp.p.pos + tmp.p.cur + tmp.p.volt) % 256 == tmp.p.check) {
      tmp_pos = tmp.p.pos;
    } else {
      tmp.p.pos = tmp_pos;
      tmp.p.cur = 0;
      tmp.p.volt = 0;
    }

    process(tmp.p.pos, tmp.p.cur, tmp.p.volt);

    draw();

    if (info_changed) {
      for (int i = 0; i < POSITIONS; ++i) {
        EEPROM.write(i, info[i].state);
      }
      info_changed = 0;
    }
  }
}
