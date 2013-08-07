#include <FastSPI_LED2.h>
#include <EEPROM.h>

#include "config.h"

struct CRGB leds[NUM_LEDS];

int mode = MODE_0;
boolean reverse = false;
int DELAY = 100;
int config = 0;
int currentColorIdx = 0;

void setup(){
  LEDS.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  clearLeds();
  readColors();
  readLeds();
  #if defined (INPUT_TERMINAL)
    setupInputTerminal();
  #elif defined (INPUT_GUI)
    setupInputGUI();
  #elif defined (INPUT_RC)
    setupInputRC();
  #elif defined (INPUT_MSP)
    setupInputMSP();
  #endif
  delay(2000);
}

unsigned long delayTime = millis();
boolean checkDelay(int DELAY){
  unsigned long newMillis = millis();
  if (newMillis - delayTime >= DELAY){
    delayTime = newMillis;
    return true;
  }
  if (newMillis < delayTime) delayTime = 0;
  return false;
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
  LEDS.show();
}

void show(int brightness){
  LEDS.show(brightness);
}

int lastMode = -1;
void loop(){
  #if defined (INPUT_TERMINAL)
    loopInputTerminal();
  #elif defined (INPUT_GUI)
    loopInputGUI();
  #elif defined (INPUT_RC)
    loopInputRC();
  #elif defined (INPUT_MSP)
    loopInputMSP();
  #endif

  if (lastMode != mode) delayTime = 0;
  switch(mode){
    case MODE_0:{
      runningMorph(false, false, false, DELAY/3);
//      runningMorph(false, true, true, DELAY);
//      runningDot(config, &CRGB(0, 0, 0), true, DELAY);        
//      showCurrentColors(config, DELAY);
      break;
    }
    case MODE_1:{
//      runningLed(config, NULL, 0, true, LEDS_PER_ARM, DELAY); 
      int arms1[] = {0, 3};
      int arms2[] = {1, 2};
      runningLed2(config, arms1, sizeof(arms1)/sizeof(int), arms2, sizeof(arms2)/sizeof(int), 2, DELAY);
      break;
    }
    case MODE_2:{
      pulseBrightness(config, 50, 250, 50, 20);
      break;
    }
    case MODE_3:{
//      runningLed(config, &CRGB(250, 250, 250), 100, false, 1, DELAY);
//      police(&CRGB(250, 0, 0), &CRGB(0, 0, 250), 500);
      blendColors(0, 1, 30, 50);
      break;
    }
  }
  lastMode = mode;
}

