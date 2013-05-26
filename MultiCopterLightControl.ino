#include <FastSPI_LED2.h>
#include <EEPROM.h>

#include "config.h"
#include "structs.h"

struct CRGB leds[NUM_LEDS];
WS2811Controller800Mhz<LED_PIN> LED;

int mode = MODE_0;
boolean reverse = false;
int DELAY = 100;
int config = 0;
int currentColorIdx = 0;

void setup(){
  LED.init();
  clearLeds();
  readColors();
  readLeds();
  #if defined (INPUT_TERMINAL)
    setupInputTerminal();
  #elif defined (INPUT_RC)
    setupInputRC();
  #elif defined (INPUT_MSP)
    setupInputMSP();
  #endif
}

void clearLeds(){
  memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
}

void allLeds(CRGB color){
  for (int i=0; i<NUM_LEDS; i++) leds[i] = color;
}

void setLed(int iLed, CRGB color){
  leds[iLed] = color;
}

int getLedIdx(int arm, int armLed){
  return arm * LEDS_PER_ARM + armLed;
}

void setArmLed(int iArm, int iLed, CRGB color){
  int i = iArm * LEDS_PER_ARM + iLed;
  leds[i] = color;
}

void setArm(int iArm, CRGB color){
  for (int i=0; i<LEDS_PER_ARM; i++) setArmLed(iArm, i, color);
}

void show(){
  LED.showRGB((byte*)leds, NUM_LEDS);
}

int lastMode = -1;
void loop(){
  #if defined (INPUT_TERMINAL)
    loopInputTerminal();
  #elif defined (INPUT_RC)
    loopInputRC();
  #elif defined (INPUT_MSP)
    loopInputMSP();
  #endif

  switch(mode){
    case MODE_0:{
      if (lastMode != mode) readLeds();
      showCurrentColors(config, DELAY);
      break;
    }
    case MODE_1:{
      runningLed(DELAY, config, NULL, 0, true, LEDS_PER_ARM); 
      break;
    }
    case MODE_2:{
//      runningLed(DELAY, config, &getCRGB(250, 250, 250), 100, false, 1); 
      pulseBrightness(DELAY, config, 50, 250, 50);
      break;
    }
    case MODE_3:{
      police(&getCRGB(250, 0, 0), &getCRGB(0, 0, 250), 500);
      break;
    }
  }
  lastMode = mode;
}

