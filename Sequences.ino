
int cycleLed = 0;
void cycleThroughAllLeds(CRGB color, int DELAY, boolean reversed){
  clearLeds();
  leds[cycleLed] = color;
  show();
  if (reversed) cycleLed = (cycleLed + (NUM_LEDS-1)) % NUM_LEDS;
  else cycleLed = (cycleLed + 1) % NUM_LEDS;
  delay(DELAY);
}

int scannerLed = 0;
void scanner(CRGB color, int DELAY) {
  CHSV hsv = rgb2hsv(color);
  hsv.v = hsv.v/8;
  CRGB color2 = hsv2rgb(hsv);
  
  clearLeds();
  leds[scannerLed] = color2;
  leds[(scannerLed+1)%NUM_LEDS] = color;
  leds[(scannerLed+2)%NUM_LEDS] = color2;
  show();  
  scannerLed = (scannerLed+1)%NUM_LEDS;
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

int rainbowOffset = 0;
void rainbowAllLeds(int DELAY, int colorStep, boolean reversed){
  for (int i=0; i<NUM_LEDS; i++){
    leds[i] = hsv2rgb(getCHSV((((int)(360.0f/NUM_LEDS*i))+rainbowOffset)%360 , 255, brightness));
  }
  show();
  if (reversed) rainbowOffset = (rainbowOffset+(360-colorStep)) % 360;
  else rainbowOffset = (rainbowOffset+colorStep) % 360;
  delay(DELAY);
}

void rainbowArms(int DELAY, int colorStep, boolean reversed){
  for (int a=0; a<NUM_ARMS; a++){
    for (int i=0; i<LEDS_PER_ARM; i++){
      setArmLed(a, i, hsv2rgb(getCHSV((((int)(360.0f/LEDS_PER_ARM*i))+rainbowOffset)%360 , 255, brightness)));
    }
  }
  show();
  if (reversed) rainbowOffset = (rainbowOffset+(360-colorStep)) % 360;
  else rainbowOffset = (rainbowOffset+colorStep) % 360;
  delay(DELAY);
}

void rainbowCycle(int DELAY, int colorStep, boolean reversed){
  for (int a=0; a<NUM_ARMS; a++){
    setArm(a, hsv2rgb(getCHSV((((int)(360.0f/NUM_ARMS*a))+rainbowOffset)%360 , 255, brightness)));
  }
  show();
  if (reversed) rainbowOffset = (rainbowOffset+(360-colorStep)) % 360;
  else rainbowOffset = (rainbowOffset+colorStep) % 360;
  delay(DELAY);
}

int cyclingDotArm = 0;
int cyclingDotLed = 0;
void cyclingDot(CRGB color, int DELAY, boolean reversed){
  clearLeds();
  setArmLed(cyclingDotArm, cyclingDotLed, color);
  show();
  if (reversed) cyclingDotArm = (cyclingDotArm+(NUM_ARMS-1)) % NUM_ARMS;
  else cyclingDotArm = (cyclingDotArm+1) % NUM_ARMS;
  if (cyclingDotArm==0){
    if (reversed) cyclingDotLed = (cyclingDotLed+(LEDS_PER_ARM-1))%LEDS_PER_ARM;
    else cyclingDotLed = (cyclingDotLed+1)%LEDS_PER_ARM;
  }
  delay(DELAY);
}

