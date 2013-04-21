void oneColor(CRGB color, int DELAY){
  allLeds(color);
  show();
  delay(DELAY);
}

void savedColors(int DELAY){
  for (int i=0; i<NUM_LEDS; i++) setLed(i, leds_rom[i]);
  show();
  delay(DELAY);
}

boolean policeFirst = true;
void police(CRGB color1, CRGB color2, int DELAY){
  int p1 = LEDS_PER_ARM / 2;
  int p2 = LEDS_PER_ARM;
  
  int s = p1;
  int e = p2;
  CRGB c = color2;
  if (policeFirst){
    s = 0;
    e = p1;
    c = color1;
  }
  clearLeds();
  for (int a=0; a<NUM_ARMS; a++){
    for (int i=s; i<e; i++){
      setArmLed(a, i, c);
    }
  }
  show();
  policeFirst = !policeFirst;
  delay(DELAY);
}

int runningOffset = 0;
int runningForward = true;
void runningLed(int DELAY, CRGB* color, int blinkDelay, boolean bounce){
  clearLeds();
  int iLed = runningOffset;
  for (int a=0; a<NUM_ARMS; a++){
    setLed(iLed, leds_rom[iLed]);
    iLed += LEDS_PER_ARM;
  }
  show();
  
  if (bounce){
    if (runningOffset==0) runningForward = true;
    else if (runningOffset==LEDS_PER_ARM-1) runningForward = false;
  }
  else runningForward = true;
  
  boolean flash = (blinkDelay>=0 && runningOffset == LEDS_PER_ARM-1);
  
  if (runningForward) runningOffset = (runningOffset+1)%LEDS_PER_ARM;
  else runningOffset = (runningOffset+LEDS_PER_ARM-1)%LEDS_PER_ARM;
  
  delay(DELAY);

  if (flash){
    if (color!=NULL) oneColor(*color, blinkDelay);
    else savedColors(blinkDelay);
  }  
}

