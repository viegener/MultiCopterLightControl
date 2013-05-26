
void oneColor(CRGB color, int DELAY){
  allLeds(color);
  show();
  if (DELAY>0) delay(DELAY);
}

void showCurrentColors(int iConfig, int DELAY){
  clearLeds();
  for (int i=0; i<NUM_LEDS; i++){
    setLed(i, getLedRGB(i, iConfig % MAX_LED_CONFIGS));
  }
  show();
  if (DELAY>0) delay(DELAY);
}

boolean policeFirst = true;
void police(CRGB* color1, CRGB* color2, int DELAY){
  int p1 = LEDS_PER_ARM / 2;
  int p2 = LEDS_PER_ARM;
  
  int s = p1;
  int e = p2;
  CRGB c = *color2;
  if (policeFirst){
    s = 0;
    e = p1;
    c = *color1;
  }
  clearLeds();
  for (int a=0; a<NUM_ARMS; a++){
    for (int i=s; i<e; i++){
      setArmLed(a, i, c);
    }
  }
  show();
  policeFirst = !policeFirst;
  if (DELAY>0) delay(DELAY);
}

int runningOffset = 0;
int runningForward = true;
void runningLed(int DELAY, int iConfig, CRGB* blinkColor, int blinkDelay, boolean bounce, int length){
  clearLeds();
  if (length<1) length=1;
  for (int i=0; i<length; i++){
    int iLed = runningOffset-(length-1) + i;
    if (iLed<0 || iLed>=LEDS_PER_ARM) continue;
    for (int a=0; a<NUM_ARMS; a++){
      setLed(iLed, getLedRGB(iLed, iConfig % MAX_LED_CONFIGS));
      iLed += LEDS_PER_ARM;
    }
  }
  show();
  
  if (bounce){
    if (runningOffset==0) runningForward = true;
    else if (runningOffset==(LEDS_PER_ARM+(length-1)-1)) runningForward = false;
  }
  else runningForward = true;
  
  boolean flash = (blinkDelay>0 && runningOffset == (LEDS_PER_ARM+(length-1)-1));
  
  if (runningForward) runningOffset = (runningOffset+1)%(LEDS_PER_ARM+(length-1));
  else runningOffset = (runningOffset+(LEDS_PER_ARM+(length-1))-1)%(LEDS_PER_ARM+(length-1));

  if (DELAY>0) delay(DELAY);

  if (flash){
    if (blinkColor!=NULL) oneColor(*blinkColor, blinkDelay);
    else showCurrentColors(iConfig, blinkDelay);
  }  
}

void showBrightness(int DELAY, int iConfig, int brightness){
  int v = brightness%256;
  clearLeds();
  for (int iLed=0; iLed<NUM_LEDS; iLed++){
    CHSV hsv = getLedHSV(iLed, iConfig);
    setLed(iLed, hsv2rgb(getCHSV(hsv.h, hsv.s, v)));
  }
  show();
  if (DELAY>0) delay(DELAY);
}

boolean pulseDirIn = true;
int pulseStep = 0;
void pulseBrightness(int DELAY, int iConfig, int startBrightness, int endBrightness, int steps){
  float sv = (float)(startBrightness%256);
  float ev = (float)(endBrightness%256);
  float dv = (ev-sv)/steps;
  int v = startBrightness + (int)((pulseStep%steps)*dv);
  showBrightness(0, iConfig, v);
  if (pulseDirIn) pulseStep = (pulseStep+1)%steps;
  else pulseStep = (pulseStep+(steps-1))%steps;
  if (pulseStep<=0){
    pulseStep = 0;
    pulseDirIn = true;
  }
  else if ((pulseStep+1)>=steps){
    pulseStep = steps-1;
    pulseDirIn = false;
  }
  if (DELAY>0) delay(DELAY);
}

