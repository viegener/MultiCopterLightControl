void oneColor(CRGB color, int DELAY){
  if (!checkDelay(DELAY)) return;
  allLeds(color);
  show();
}

void showCurrentColors(int iConfig, int DELAY){
  if (!checkDelay(DELAY)) return;
  clearLeds();
  for (int i=0; i<NUM_LEDS; i++){
    setLed(i, getLedRGB(i, iConfig % MAX_LED_CONFIGS));
  }
  show();
}

boolean policeFirst = true;
void police(CRGB* color1, CRGB* color2, int DELAY){
  if (!checkDelay(DELAY)) return;
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
}

int runningOffset = 0;
int runningForward = true;
void runningLed(int iConfig, CRGB* blinkColor, int blinkDelay, boolean bounce, int length, int DELAY){
  if (!checkDelay(DELAY)) return;
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

  if (flash){
    if (blinkColor!=NULL) oneColor(*blinkColor, 0);
    else showCurrentColors(iConfig, 0);
    delay(blinkDelay);
  }
}

boolean switchArms = false;
void runningLed2(int iConfig, int arms1[], int arm1Size, int arms2[], int arm2Size, int length, int DELAY){
  if (!checkDelay(DELAY)) return;
  clearLeds();
  if (length<1) length=1;
  for (int i=0; i<length; i++){
    int iLed = runningOffset-(length-1) + i;
    if (iLed<0 || iLed>=LEDS_PER_ARM) continue;
    
    int* currentArms = NULL;
    int currentSize = 0;
    if (runningForward == switchArms){
      currentArms = arms1;
      currentSize = arm1Size;
    }
    else{
      currentArms = arms2;
      currentSize = arm2Size;
    }
    
    for (int ca=0; ca<currentSize; ca++){
      int a = currentArms[ca];
      if (a<0 || a>=NUM_ARMS) continue;
      int il = getLedIdx(a, iLed);
      setLed(il, getLedRGB(il, iConfig % MAX_LED_CONFIGS));
    }
  }
  show();
  
  if (arms2!=NULL && arm2Size>0){
    if (runningOffset==0){
      runningForward = true;
    }
    else if (runningOffset==(LEDS_PER_ARM+(length-1)-1)){
      runningForward = false;
      switchArms = !switchArms;
    }
  }
  else runningForward = true;
  
  if (runningForward) runningOffset = (runningOffset+1)%(LEDS_PER_ARM+(length-1));
  else runningOffset = (runningOffset+(LEDS_PER_ARM+(length-1))-1)%(LEDS_PER_ARM+(length-1));
}

boolean pulseDirIn = true;
int pulseStep = 0;
void pulseBrightness(int iConfig, int startBrightness, int endBrightness, int steps, int DELAY){
  if (!checkDelay(DELAY)) return;
  float sv = (float)(startBrightness%256);
  float ev = (float)(endBrightness%256);
  float dv = (ev-sv)/steps;
  int v = startBrightness + (int)((pulseStep%steps)*dv);
  clearLeds();
  for (int iLed=0; iLed<NUM_LEDS; iLed++){
    setLed(iLed, getLedRGB(iLed, iConfig));
  }
  show(v);
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
}

int blendingStep = 0;
int blendingForward = true;
void blendColors(int config1, int config2, int steps, int DELAY){
  if (!checkDelay(DELAY)) return;
  blendingStep %= steps;
  for (int i=0; i<NUM_LEDS; i++){
    CRGB rgb1 = getLedRGB(i, config1);
    CRGB rgb2 = getLedRGB(i, config2);
    int r, g, b;
    r = ((rgb1.r * (steps - blendingStep)) + (rgb2.r * blendingStep)) / steps;
    g = ((rgb1.g * (steps - blendingStep)) + (rgb2.g * blendingStep)) / steps;
    b = ((rgb1.b * (steps - blendingStep)) + (rgb2.b * blendingStep)) / steps;
    CRGB rgb = CRGB(r, g, b);
    setLed(i, rgb);
  }
  show();
  
  if (blendingStep==0) blendingForward = true;
  else if (blendingStep==(steps-1)) blendingForward = false;
  
  if (blendingForward) blendingStep = (blendingStep+1)%steps;
  else blendingStep = (blendingStep+(steps-1))%steps;
}

void runningDot(int iConfig, CRGB* blinkColor, boolean bounce, int DELAY){
  if (!checkDelay(DELAY)) return;

  clearLeds();

  for (int i=0; i<NUM_LEDS; i++){
    setLed(i, getLedRGB(i, iConfig % MAX_LED_CONFIGS));
  }

  int iLed = runningOffset;
  for (int a=0; a<NUM_ARMS; a++){
    setLed(iLed, *blinkColor);
    iLed += LEDS_PER_ARM;
  }
  show();
  
  if (bounce){
    if (runningOffset<=0) runningForward = true;
    else if (runningOffset>=(LEDS_PER_ARM-1)) runningForward = false;
  }
  else runningForward = true;
  
  runningOffset = runningForward ?
    (runningOffset+1)%LEDS_PER_ARM : (runningOffset-1)%LEDS_PER_ARM;

}


// #define MORPH_DEBUG

CRGB aColor = CRGB(0, 0, 0);
CRGB aColorNext = CRGB(0, 255, 0);
int orgBase = 512;

float reduction = .7;
float increase = 1.3;

#ifdef MORPH_DEBUG
long lstep = 0;
#endif

int morphIncrease = false;

void runningMorph(boolean change_r, boolean change_g, boolean change_b, int DELAY){
  CRGB aColorSave;
  if (!checkDelay(DELAY)) return;
  
  clearLeds();
  
  if ( orgBase == 512 ) {
    orgBase = 255;
    aColor = CRGB(0, 0, 255);
  }

  int base = orgBase;
  orgBase = -1;
  
#ifdef MORPH_DEBUG
  lstep++;
  Serial.print("**** Step : ");
  Serial.println( lstep );
  
  Serial.print("    Base : " );
  Serial.println(base );
#endif

  if ( change_r || change_g || change_g ) {
    aColor.r = change_r ? base : 0;
    aColor.g = change_g ? base : 0;
    aColor.b = change_b ? base : 0;
  }

  boolean doIncrease = morphIncrease;

  aColorSave = aColor;

  for (int b=0; b<LEDS_PER_ARM; b++){
    if ( aColor.r > 0 )
      aColor.r = base;
    if ( aColor.g > 0 )
      aColor.g = base;
    if ( aColor.b > 0 )
      aColor.b = base;
    int iLed = b;
    setLed(iLed, aColor);
    iLed += LEDS_PER_ARM;
    setLed(iLed, aColor);
    iLed += LEDS_PER_ARM;
    setLed(iLed, aColor);
    iLed += LEDS_PER_ARM;
    setLed(iLed, aColor);

#ifdef MORPH_DEBUG
    Serial.print("   Led : " );
    Serial.print( b );
    Serial.print("  C ( " ); 
    Serial.print(aColor.r);
    Serial.print("," ); 
    Serial.print(aColor.g);
    Serial.print("," ); 
    Serial.print(aColor.b);
    Serial.println(" )" ); 
    Serial.print("        Base ");
    Serial.print(base );
    Serial.print("    inc ");
    Serial.println( morphIncrease );
#endif
    if ( doIncrease ) {
      base *= increase;
//      base += 10;
      if ( base >= 252 ) {
        base = 255;
        doIncrease = false;
      }
    } else {
      base *= reduction;
//      base -= 10;
      if ( base <=7 ) {
        base = 7;
        doIncrease = true;
        aColor = aColorNext;
      }
    }
    if ( orgBase == -1 ) {
      orgBase = base;
    }
  }
  aColor = aColorSave;

  if ( ! ( change_r || change_g || change_g ) ) {
    if ( orgBase >= 252 ) {
#ifdef MORPH_DEBUG
          Serial.println(" ****** SWITCH DIR" );
#endif
            morphIncrease = ! morphIncrease;
    } else if ( orgBase <= 7 ) {

#ifdef MORPH_DEBUG
      Serial.println(" ****** New Color" );
#endif

      int ns = random( 1, 7 );

      aColor = aColorNext;
      morphIncrease = ! morphIncrease;
      aColorNext = CRGB(0, 0, 0);
      if ( (  ns & 0x04 ) > 0 ) {
        aColorNext.r = 255;
      }
      if ( (  ns & 0x02 ) > 0 ) {
        aColorNext.g = 255;
      }
      if ( (  ns & 0x01 ) > 0 ) {
        aColorNext.b = 255;
      }
#ifdef MORPH_DEBUG
      Serial.print("    is : ");
      Serial.print("  C ( " ); 
      Serial.print(aColorNext.r);
      Serial.print("," ); 
      Serial.print(aColorNext.g);
      Serial.print("," ); 
      Serial.print(aColorNext.b);
      Serial.println(" )" ); 
#endif
    }
  }

  show();
//  delay(2000);
}



