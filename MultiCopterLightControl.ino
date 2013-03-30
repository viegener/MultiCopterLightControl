#include <FastSPI_LED2.h>
#include <EEPROM.h>
#include <SerialCommand.h>

#include "config.h"
#include "structs.h"

struct CRGB leds[NUM_LEDS];
WS2811Controller800Mhz<LED_PIN> LED;
SerialCommand sCmd;

int mode = 0;
int lastMode = -1;
CRGB currentRGB = {0, 0, 0};
boolean reverse = false;
int brightness = 50;
int DELAY = 50;

void setup(){
  Serial.begin(115200);
  LED.init();
  clearLeds();
  initSerialCommands();
  DEBUG_PRINTLN("--- SETUP COMPLETE ---");
}

void clearLeds(){
  memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
}

void setLed(int iLed, CRGB color){
  leds[iLed] = color;
}

void setArmLed(int iArm, int iLed, CRGB color){
  int i = iArm * LEDS_PER_ARM + iLed;
  leds[i] = color;
}

void setArm(int iArm, CRGB color){
  for (int i=0; i<LEDS_PER_ARM; i++) setArmLed(iArm, i, color);
}

CRGB getCRGB(byte r, byte g, byte b){
  CRGB c = {g, r, b};
  return c;
}

CHSV getCHSV(int h, byte s, byte v){
  CHSV c = {h, s, v};
  return c;
}

inline CRGB hsv2rgb(const CHSV c) {
  // hue: 0-359, sat: 0-255, val (brightness): 0-255
  CRGB rgb;
  int base;
  if (c.s == 0) { // Achromatic color (gray).
    rgb.r = c.v;
    rgb.g = c.v;
    rgb.b = c.v;
  } 
  else  {
    base = ((255 - c.s) * c.v)>>8;
    switch(c.h/60) {
    case 0:
      rgb.r = c.v;
      rgb.g = (((c.v-base)*c.h)/60)+base;
      rgb.b = base;
      break;
    case 1:
      rgb.r = (((c.v-base)*(60-(c.h%60)))/60)+base;
      rgb.g = c.v;
      rgb.b = base;
      break;
    case 2:
      rgb.r = base;
      rgb.g = c.v;
      rgb.b = (((c.v-base)*(c.h%60))/60)+base;
      break;
    case 3:
      rgb.r = base;
      rgb.g = (((c.v-base)*(60-(c.h%60)))/60)+base;
      rgb.b = c.v;
      break;
    case 4:
      rgb.r = (((c.v-base)*(c.h%60))/60)+base;
      rgb.g = base;
      rgb.b = c.v;
      break;
    case 5:
      rgb.r = c.v;
      rgb.g = base;
      rgb.b = (((c.v-base)*(60-(c.h%60)))/60)+base;
      break;
    }
  }
  return rgb;
}

inline CHSV rgb2hsv(const CRGB c) {
  // r: 0-255, g: 0-255, b: 0-255
  float fr = c.r / 255.0;
  float fg = c.g / 255.0;
  float fb = c.b / 255.0;
  float h, s, v, imax, imin;
  imax = max(max(fr, fg), fb);
  imin = min(min(fr, fg), fb);
  if (imin == imax){
    h = 0;
    s = 0;
    v = imax;
  }
  else{
    float d = (fr==imin) ? fg-fb : ((fb==imin) ? fr-fg : fb-fr);
    float j = (fr==imin) ? 3 : ((fb==imin) ? 1 : 5);
    h = 60.0 * (j - d/(imax - imin));
    s = (float)(imax - imin) / imax;
    v = imax;
  }
  CHSV hsv;
  hsv.h = (uint16_t)h;
  hsv.s = (uint16_t)(s * 255);
  hsv.v = (uint16_t)(v * 255);
  return hsv;
}

void show(){
  LED.showRGB((byte*)leds, NUM_LEDS);
}

void writeLeds(){
  for (int i=0; i<NUM_LEDS; i++){
    EEPROM.write(i * 3 + 0, (byte)(255 - leds[i].r));
    EEPROM.write(i * 3 + 1, (byte)(255 - leds[i].g));
    EEPROM.write(i * 3 + 2, (byte)(255 - leds[i].b));
  }
}

void readLeds(){
  for (int i=0; i<NUM_LEDS; i++){
    leds[i].r = (byte)(255 - EEPROM.read(i * 3 + 0));
    leds[i].g = (byte)(255 - EEPROM.read(i * 3 + 1));
    leds[i].b = (byte)(255 - EEPROM.read(i * 3 + 2));
  }
}

void handleMode(int m){
  switch(m){
    case MODE_SAVED_COLORS:{
      if (lastMode!=mode) readLeds();
      show();
      delay(50);
      break;
    }
    case MODE_RAINBOW:{
      rainbowAllLeds(DELAY, 2, reverse);
      break;
    }
    case MODE_RAINBOW_ARMS:{
      rainbowArms(DELAY, 5, reverse);
      break;
    }
    case MODE_RAINBOW_CYCLE:{
      rainbowCycle(DELAY, 5, reverse);
      break;
    }
    case MODE_CYCLING_DOT:{
      cyclingDot(currentRGB, DELAY, reverse);
      break;
    }
    case MODE_POLICE:{
      CHSV hsv = rgb2hsv(currentRGB);
      if (reverse) hsv.h = (hsv.h + 120)%360;
      else hsv.h = (hsv.h + 240)%360;
      CRGB color2 = hsv2rgb(hsv);
      police(currentRGB, color2, DELAY);
      break;
    }
    case MODE_SCANNER_ALL:{
      scanner(currentRGB, DELAY);
      break;
    }
    case MODE_RUNNING_DOT:{
      cycleThroughAllLeds(currentRGB, DELAY, reverse);
      break;
    }
  }
}

void loop(){
  sCmd.readSerial();
  switch(mode){
    case MODE_SAVED_COLORS:
    case MODE_RAINBOW:
    case MODE_RAINBOW_ARMS:
    case MODE_RAINBOW_CYCLE:
    case MODE_CYCLING_DOT:
    case MODE_POLICE:
    case MODE_RUNNING_DOT:
    case MODE_SCANNER_ALL:
      handleMode(mode);
      break;
    default:
      handleMode(MODE_DEFAULT);
      break;
  }
  lastMode = mode;
}



