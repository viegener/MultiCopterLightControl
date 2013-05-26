#if defined (INPUT_TERMINAL)

#include <SerialCommand.h>

SerialCommand sCmd;

void setupInputTerminal(){
  Serial.begin(115200);
  initSerialCommands();
  SERIAL_PRINTLN("*** SETUP COMPLETE ***");
}

void loopInputTerminal(){
  sCmd.readSerial();
}

void initSerialCommands(){
  sCmd.addCommand("setR", serialSetR);
  sCmd.addCommand("setG", serialSetG);
  sCmd.addCommand("setB", serialSetB);
  sCmd.addCommand("setColor", serialSetColor);
  sCmd.addCommand("setConfig", serialSetConfig);
  sCmd.addCommand("getColor", serialGetColor);
  sCmd.addCommand("printColor", printCurrentColor);

  sCmd.addCommand("saveColors", serialSaveColors);
  sCmd.addCommand("saveLeds", serialSaveLeds);
  sCmd.addCommand("clearEeprom", clearEeprom);
  
  sCmd.addCommand("setArm", serialSetArm);
  sCmd.addCommand("setLed", serialSetLed);
  sCmd.addCommand("m+", serialNextMode);
  sCmd.addCommand("m-", serialPreviousMode);
  sCmd.addCommand("d", serialSetDelay);
}

int readIntArg(){
  char *arg;
  arg = sCmd.next();
  if (arg != NULL) return atoi(arg);
  return -1;
}

void printCurrentColor(){
  SERIAL_PRINT("current RGB color = (");
  SERIAL_PRINT(currentRGB.r);
  SERIAL_PRINT(", ");
  SERIAL_PRINT(currentRGB.g);
  SERIAL_PRINT(", ");
  SERIAL_PRINT(currentRGB.b);
  SERIAL_PRINTLN(")");
}

void serialSetR(){
  int newColor = readIntArg();
  if (newColor>=0 && newColor<256){
    currentRGB.r = newColor;
    printCurrentColor();
  }
}

void serialSetG(){
  int newColor = readIntArg();
  if (newColor>=0 && newColor<256){
    currentRGB.g = newColor;
    printCurrentColor();
  }
}

void serialSetB(){
  int newColor = readIntArg();
  if (newColor>=0 && newColor<256){
    currentRGB.b = newColor;
    printCurrentColor();
  }
}

void serialSetColor(){
  int idx = readIntArg()%MAX_EEPROM_COLORS;
  if (idx>=0 && idx<MAX_EEPROM_COLORS){
    SERIAL_PRINT("Set color ");
    SERIAL_PRINT(idx);
    SERIAL_PRINTLN(" to current color.");
    storedColorsRGB[idx] = currentRGB;
    storedColorsHSV[idx] = rgb2hsv(storedColorsRGB[idx]);
    currentColorIdx = idx;
  }
}

void serialSetConfig(){
  config = readIntArg()%MAX_LED_CONFIGS;
  SERIAL_PRINT("new config = ");
  SERIAL_PRINTLN(config);
}

void serialGetColor(){
  int idx = readIntArg()%MAX_EEPROM_COLORS;
  currentRGB = storedColorsRGB[idx];
  currentColorIdx = idx;
}

void serialSaveColors(){
  writeColors();
  SERIAL_PRINTLN("Colors saved to EEPROM.");
}

void serialSaveLeds(){
  writeLeds();
  SERIAL_PRINTLN("Led colors saved to EEPROM.");
}

void serialSetArm(){
  int arm = readIntArg() % NUM_ARMS;
  if (arm>=0 && arm<NUM_ARMS){
    setArmColor(currentColorIdx, arm, config);
    SERIAL_PRINT("Updated arm ");
    SERIAL_PRINTLN(arm);
  }
}

void serialSetLed(){
  int led = readIntArg() % NUM_LEDS;
  if (led>=0 && led<NUM_LEDS){
    setLedColor(currentColorIdx, led, config);
  }
}

void printNewMode(){
  SERIAL_PRINT("new mode = ");
  SERIAL_PRINTLN(mode);
}

void serialNextMode(){
  mode = (mode+1) % NUM_MODES;
  printNewMode();
}

void serialPreviousMode(){
  mode = (mode+(NUM_MODES-1)) % NUM_MODES;
  printNewMode();
}

void serialSetDelay(){
  int newDelay = readIntArg();
  if (newDelay>0 && newDelay<10000){
    DELAY = newDelay;
    SERIAL_PRINT("new delay = ");
    SERIAL_PRINTLN(DELAY);
  }
}

void serialReversed(){
  reverse = !reverse;
  SERIAL_PRINTLN("reversed");
}

#endif

