
void initSerialCommands(){
  sCmd.addCommand("save", serialSaveCurrentLeds);
  sCmd.addCommand("setR", serialSetR);
  sCmd.addCommand("setG", serialSetG);
  sCmd.addCommand("setB", serialSetB);
  sCmd.addCommand("setArm", serialSetArm);
  sCmd.addCommand("setV", serialSetV);
  sCmd.addCommand("clear", clearLeds);
  sCmd.addCommand("+", serialNextMode);
  sCmd.addCommand("-", serialPreviousMode);
  sCmd.addCommand("m", serialSetMode);
  sCmd.addCommand("d", serialSetDelay);
  sCmd.addCommand("rev", serialReversed);
}

void serialSaveCurrentLeds(){
  writeLeds();
  DEBUG_PRINTLN("Led colors saved to EEPROM.");
}

int readIntArg(){
  char *arg;
  arg = sCmd.next();
  if (arg != NULL) return atoi(arg);
  return -1;
}

void printCurrentColor(){
  DEBUG_PRINT("current RGB color = (");
  DEBUG_PRINT(currentRGB.r);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(currentRGB.g);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(currentRGB.b);
  DEBUG_PRINTLN(")");
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

void serialSetArm(){
  int arm = readIntArg();
  if (arm>=0 && arm<NUM_ARMS){
    setArm(arm, currentRGB);
    DEBUG_PRINT("Updated arm ");
    DEBUG_PRINTLN(arm);
  }
}

void printNewMode(){
  DEBUG_PRINT("new mode = ");
  DEBUG_PRINTLN(mode);
}

void serialNextMode(){
  mode = (mode+1) % NUM_MODES;
  printNewMode();
}

void serialPreviousMode(){
  mode = (mode+(NUM_MODES-1)) % NUM_MODES;
  printNewMode();
}

void serialSetMode(){
  int newMode = readIntArg();
  if (newMode>=0 && newMode<NUM_MODES){
    mode = newMode;
    printNewMode();
  }
}

void serialSetDelay(){
  int newDelay = readIntArg();
  if (newDelay>0 && newDelay<10000){
    DELAY = newDelay;
    DEBUG_PRINT("new delay = ");
    DEBUG_PRINTLN(DELAY);
  }
}

void serialSetV(){
  int newV = readIntArg();
  if (newV>=0 && newV<256){
    brightness = newV;
    DEBUG_PRINT("new brightness = ");
    DEBUG_PRINTLN(brightness);
  }
}

void serialReversed(){
  reverse = !reverse;
  DEBUG_PRINTLN("reversed");
}


