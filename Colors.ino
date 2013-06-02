// DON'T CHANGE THESE VALUES, OTHERWISE THE COMPLETE MANAGEMENT OF COLORS ISN'T WORKING
#define MAX_EEPROM_COLORS      16
#define MAX_LED_CONFIGS        8
#define EEPROM_COLORS_OFFSET   MAX_EEPROM_COLORS*3

CRGB storedColorsRGB[MAX_EEPROM_COLORS];
CHSV storedColorsHSV[MAX_EEPROM_COLORS];
CRGB currentRGB = {0, 0, 0};

int num_led_bytes = ceil(NUM_LEDS/2.0f);
uint8_t storedLedColors[MAX_LED_CONFIGS][(NUM_LEDS/2)+1];

void setColor(CRGB c, int idx){
  idx = idx % MAX_EEPROM_COLORS;
  storedColorsRGB[idx] = c;
  storedColorsHSV[idx] = rgb2hsv(storedColorsRGB[idx]);
  printColor(c);
  SERIAL_PRINT(" stored as color ");
  SERIAL_PRINTLN(idx);
}

void clearEeprom(){
  for (int i = 0; i < 512; i++) EEPROM.write(i, 0);
}

void writeColors(){
  for (int i=0; i<MAX_EEPROM_COLORS; i++){
    EEPROM.write(i * 3 + 0, (byte)(255 - storedColorsRGB[i].r));
    EEPROM.write(i * 3 + 1, (byte)(255 - storedColorsRGB[i].g));
    EEPROM.write(i * 3 + 2, (byte)(255 - storedColorsRGB[i].b));
  }
}

void readColors(){
  for (int i=0; i<MAX_EEPROM_COLORS; i++){
    uint8_t r, g, b;
    r = (uint8_t) (255 - EEPROM.read(i*3 + 0));
    g = (uint8_t) (255 - EEPROM.read(i*3 + 1));
    b = (uint8_t) (255 - EEPROM.read(i*3 + 2));
    storedColorsRGB[i] = getCRGB(r, g, b);
    storedColorsHSV[i] = rgb2hsv(storedColorsRGB[i]);
  }
}

void writeLeds(){
  for (int i=0; i<MAX_LED_CONFIGS; i++){
    for (int j=0; j<num_led_bytes; j++){
      EEPROM.write(EEPROM_COLORS_OFFSET + (i * num_led_bytes) + j, (byte) storedLedColors[i][j]);
    }
  }
}

void readLeds(){
  for (int i=0; i<MAX_LED_CONFIGS; i++){
    for (int j=0; j<num_led_bytes; j++){
      storedLedColors[i][j] = (uint8_t)EEPROM.read(EEPROM_COLORS_OFFSET + (i * num_led_bytes) + j);
    }
  }
}

CRGB getCRGB(uint8_t r, uint8_t g, uint8_t b){
  CRGB c = {g, r, b};
  return c;
}

CHSV getCHSV(uint16_t h, uint8_t s, uint8_t v){
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

void setLedColor(int colorIdx, int iLed, int iConfig){
  setColorIndex(iLed, iConfig, colorIdx);
}

void setArmColor(int colorIdx, int iArm, int iConfig){
  for (int i=0; i<LEDS_PER_ARM; i++){
    setLedColor(colorIdx, (iArm%NUM_ARMS) * LEDS_PER_ARM + i, iConfig);
  }
}

void setColorIndex(int iLed, int iConfig, int iColor){
  iLed %= NUM_LEDS;
  iConfig %= MAX_LED_CONFIGS;
  int ledByte = floor(iLed / 2);
  uint8_t value = storedLedColors[iConfig][ledByte];
  if (iLed%2==0) value = ((iColor % MAX_EEPROM_COLORS)<<4) + (value & 0xF);
  else value = ((value>>4)<<4) + (iColor);
  storedLedColors[iConfig][ledByte]= value;
}

uint8_t getColorIndex(int iLed, int iConfig){
  iLed %= NUM_LEDS;
  iConfig %= MAX_LED_CONFIGS;
  int ledByte = floor(iLed / 2);
  uint8_t value = storedLedColors[iConfig][ledByte];
  if (iLed%2==1) return (uint8_t)(value & 0xF);
  else return (uint8_t)(value>>4);
}

CRGB getLedRGB(int iLed, int iConfig){
  return storedColorsRGB[getColorIndex(iLed, iConfig)];
}

CHSV getLedHSV(int iLed, int iConfig){
  return storedColorsHSV[getColorIndex(iLed, iConfig)];
}

void printColor(CRGB c){
  SERIAL_PRINT("(r=");
  SERIAL_PRINT(c.r);
  SERIAL_PRINT(", g=");
  SERIAL_PRINT(c.g);
  SERIAL_PRINT(", b=");
  SERIAL_PRINT(c.b);
  SERIAL_PRINT(")");
}


