import processing.serial.*;
import java.util.*;
import controlP5.*;

private static final int WINDOW_X = 800;
private static final int WINDOW_Y = 600;
private static final int GUI_BAUD_RATE = 115200;
private static final String FONT_NAME = "Times New Roman"; 
private static final int BUFFER_SIZE = 1024;

boolean setupDone = false;
boolean initCom = false;
int setupStatus = NEED_SETUP;
String selectedPort = null;

int BLACK = color(0, 0, 0);
int WHITE = color(255, 255, 255);
int RED = color(200, 0, 0);
int GREEN = color(0, 200, 0);
int BLUE = color(0, 0, 200);
int PURPLE = color(200, 0, 200);
int GRAY = color(128, 128, 128);

ControlFont ft = new ControlFont(createFont(FONT_NAME, 24), 12);

private int numArms = -1;
private int ledsPerArm = -1;
private int maxColors = -1;
private int maxConfigs = -1;
private int numLeds = -1;

private static final String MSP_HEADER = "$M<";

private static final int
  MSP_SETUP                =150,
  MSP_COLORS               =151,
  MSP_CONFIGS              =152,
  MSP_SET_COLORS           =230,
  MSP_SET_CONFIGS          =231,
  MSP_SAVE_COLORS          =232,
  MSP_SAVE_CONFIGS         =233,
  MSP_NEXT_MODE            =234,
  MSP_PREV_MODE            =235,
  MSP_NEXT_CONFIG          =236,
  MSP_PREV_CONFIG          =237;
  
public static final int
  IDLE = 0,
  HEADER_START = 1,
  HEADER_M = 2,
  HEADER_ARROW = 3,
  HEADER_SIZE1 = 4,
  HEADER_SIZE2 = 5,
  HEADER_CMD = 6,
  HEADER_ERR = 7  
;

public static final int
  NEED_SETUP = 0,
  NEED_COLORS = 1,
  NEED_CONFIGS = 2,
  DONE = 3
;

int c_state = IDLE;
boolean err_rcvd = false;

byte checksum=0;
byte cmd;
int offset=0, dataSize=0;
byte[] inBuf = new byte[BUFFER_SIZE];
List<List<Integer>> ledConfigs = null;

int activeButton = 0;
int activeConfig = 0;

ControlP5 cp5;
Button[] bColor, bLed;
Button bSaveColors, bGetColors, bSetColors, bSetConfigs, bGetConfigs, bSaveConfigs, bNextMode, bPrevMode, bNextConfig, bPrevConfig;
Textlabel tlPorts;
DropdownList dlPorts;
ColorPicker cpColor;
Serial g_serial;

void setup(){
  size(WINDOW_X, WINDOW_Y);
  frame.setTitle("MultiCopterLightControl Configuration");
  frameRate(30);
  noStroke();
  
  cp5 = new ControlP5(this);
  
  // Label for port selection
  tlPorts = cp5.addTextlabel("labelPorts");
  tlPorts.setText("Select Port:");
  tlPorts.setPosition(10, 10);
  tlPorts.setColor(WHITE);
  tlPorts.setFont(new ControlFont(createFont(FONT_NAME, 24), 14));

  // port selection  
  dlPorts = cp5.addDropdownList("ports");
  dlPorts.setSize(150, 100);
  dlPorts.setPosition(85, 27);
  dlPorts.setBackgroundColor(BLACK);
  dlPorts.setItemHeight(20);
  dlPorts.setBarHeight(15);
  dlPorts.captionLabel().set("Available ports");
  dlPorts.captionLabel().style().marginTop = 3;
  dlPorts.captionLabel().style().marginLeft = 3;
  dlPorts.valueLabel().style().marginTop = 3;
  for (int i=0; i<Serial.list().length; i++){
    String org = Serial.list()[i];
    String pn = shortifyPortName(org, 13);
    dlPorts.addItem(pn, i);
  }
  dlPorts.setColorBackground(color(75));
  dlPorts.setColorActive(GREEN);

  setupDone = true;
}

void draw(){
  background(BLACK);
  readSerialMessages();
}

void controlEvent(ControlEvent theEvent) {
  if (theEvent.isGroup()) {
    // check if the Event was triggered from a ControlGroup
//    println("event from group : "+theEvent.getGroup().getValue()+" from "+theEvent.getGroup());
    if (theEvent.getGroup() == dlPorts){
      int idx = (int)theEvent.getGroup().getValue();
      InitSerial(idx);
    }
    else if (theEvent.getGroup() == cpColor){
      setColorToButton(bColor[activeButton], cpColor.getColorValue());
      activateConfig(activeConfig);
    }
  } 
  else if (theEvent.isController()) {
//    println("event from controller : "+theEvent.getController().getValue()+" from "+theEvent.getController());
    if (theEvent.getController() instanceof Button && setupStatus==DONE){
      {
        String buttonName = theEvent.getController().getName();
        if (buttonName.startsWith("Color")){
          activateButton((int)theEvent.getController().getValue());
        }
        else if (buttonName.startsWith("LED")){
          int selectedLed = (int)theEvent.getController().getValue();
          ledConfigs.get(activeConfig).set(selectedLed, activeButton);
          activateConfig(activeConfig);
        }
      }
    }
  }
}

void activateButton(int idx){
  idx = max(0, idx);
  idx = idx%maxColors;
  activeButton = idx;
  if (cpColor!=null && bColor!=null) cpColor.setColorValue(bColor[activeButton].getColor().getBackground());
}

void createColorButtons(List<Integer> colors, int startX, int startY, int buttonX, int buttonY){
  if (colors==null || colors.size()!=maxColors) return;
  bColor = new Button[maxColors];
  int maxPerRow = (WINDOW_X - 2*startX) / buttonX;
  int rows = ceil(((float)colors.size())/maxPerRow);
  for (int i=0; i<colors.size(); i++){
    int col = color(0);
    if (colors.get(i)!=null) col = colors.get(i).intValue();
    String bName = "Color" + i;
    int currentRow = ceil(((float)(i+1))/maxPerRow) - 1;
    int currentColumn = i%maxPerRow;
    
    bColor[i] = cp5.addButton(bName);
    bColor[i].setValue(i);
    bColor[i].setSize(buttonX, buttonY);
    
    int currentY = startY + currentRow*buttonY;
    int currentX = startX + currentColumn*buttonX;

    bColor[i].setPosition(currentX, currentY);
    setColorToButton(bColor[i], col);
    bColor[i].getCaptionLabel().alignX(CENTER);
    bColor[i].getCaptionLabel().setFont(ft);
    bColor[i].setLabel("Color  "+i);
  }
  
  cpColor = cp5.addColorPicker("picker");
  cpColor.setPosition(startX, 150);
  
  bSetColors = cp5.addButton("setColors");
  bSetColors.setSize(90, 25);
  bSetColors.setPosition(startX + 270, 150);
  setColorToButton(bSetColors, GRAY);
  bSetColors.setColorActive(GREEN);
  bSetColors.getCaptionLabel().alignX(CENTER);
  bSetColors.getCaptionLabel().setFont(ft);
  bSetColors.setLabel("Set Colors");

  bGetColors = cp5.addButton("getColors");
  bGetColors.setSize(90, 25);
  bGetColors.setPosition(startX + 370, 150);
  setColorToButton(bGetColors, GRAY);
  bGetColors.setColorActive(GREEN);
  bGetColors.getCaptionLabel().alignX(CENTER);
  bGetColors.getCaptionLabel().setFont(ft);
  bGetColors.setLabel("Get Colors");

  bSaveColors = cp5.addButton("saveColors");
  bSaveColors.setSize(90, 25);
  bSaveColors.setPosition(startX + 470, 150);
  setColorToButton(bSaveColors, GRAY);
  bSaveColors.getCaptionLabel().alignX(CENTER);
  bSaveColors.getCaptionLabel().setFont(ft);
  bSaveColors.setLabel("Save Colors");
}

void createLedButtons(List<Integer> leds, int startX, int startY, int buttonX, int buttonY, int offsetX, int offsetY){
  if (leds==null || leds.size()!=numLeds) return;
  bLed = new Button[numLeds];
  int maxPerRow = min(ledsPerArm, (WINDOW_X - 2*startX + offsetX) / (buttonX+offsetX));
  int currentRow = 0, currentColumn = 0;
  for (int i=0; i<leds.size(); i++){
    int col = color(0);
    if (leds.get(i)!=null) col = bColor[leds.get(i)%maxColors].getColor().getBackground();
    String bName = "LED" + i;
    
    bLed[i] = cp5.addButton(bName);
    bLed[i].setValue(i);
    bLed[i].setSize(buttonX, buttonY);
    
    int currentY = startY + currentRow*(buttonY+offsetY);
    int currentX = startX + currentColumn*(buttonX+offsetX);

    bLed[i].setPosition(currentX, currentY);
    setColorToButton(bLed[i], col);
    bLed[i].getCaptionLabel().alignX(CENTER);
    bLed[i].setLabel("");
    
    currentColumn++;
    if (currentColumn>=maxPerRow || (i+1)%ledsPerArm==0){
      currentColumn = 0;
      currentRow++;
    }
  }
  
  bSetConfigs = cp5.addButton("setConfigs");
  bSetConfigs.setSize(90, 25);
  bSetConfigs.setPosition(startX + 270, 180);
  setColorToButton(bSetConfigs, GRAY);
  bSetConfigs.setColorActive(GREEN);
  bSetConfigs.getCaptionLabel().alignX(CENTER);
  bSetConfigs.getCaptionLabel().setFont(ft);
  bSetConfigs.setLabel("Set Configs");

  bGetConfigs = cp5.addButton("getConfigs");
  bGetConfigs.setSize(90, 25);
  bGetConfigs.setPosition(startX + 370, 180);
  setColorToButton(bGetConfigs, GRAY);
  bGetConfigs.setColorActive(GREEN);
  bGetConfigs.getCaptionLabel().alignX(CENTER);
  bGetConfigs.getCaptionLabel().setFont(ft);
  bGetConfigs.setLabel("Get Configs");

  bSaveConfigs = cp5.addButton("saveConfigs");
  bSaveConfigs.setSize(90, 25);
  bSaveConfigs.setPosition(startX + 470, 180);
  setColorToButton(bSaveConfigs, GRAY);
  bSaveConfigs.setColorActive(GREEN);
  bSaveConfigs.getCaptionLabel().alignX(CENTER);
  bSaveConfigs.getCaptionLabel().setFont(ft);
  bSaveConfigs.setLabel("Save Configs");

  bNextMode = cp5.addButton("nextMode");
  bNextMode.setSize(90, 25);
  bNextMode.setPosition(startX + 570, 150);
  setColorToButton(bNextMode, GRAY);
  bNextMode.setColorActive(GREEN);
  bNextMode.getCaptionLabel().alignX(CENTER);
  bNextMode.getCaptionLabel().setFont(ft);
  bNextMode.setLabel("Next Mode");

  bPrevMode = cp5.addButton("prevMode");
  bPrevMode.setSize(90, 25);
  bPrevMode.setPosition(startX + 570, 180);
  setColorToButton(bPrevMode, GRAY);
  bPrevMode.setColorActive(GREEN);
  bPrevMode.getCaptionLabel().alignX(CENTER);
  bPrevMode.getCaptionLabel().setFont(ft);
  bPrevMode.setLabel("Prev Mode");

  bNextConfig = cp5.addButton("nextConfig");
  bNextConfig.setSize(90, 25);
  bNextConfig.setPosition(startX + 670, 150);
  setColorToButton(bNextConfig, GRAY);
  bNextConfig.setColorActive(GREEN);
  bNextConfig.getCaptionLabel().alignX(CENTER);
  bNextConfig.getCaptionLabel().setFont(ft);
  bNextConfig.setLabel("Next Config");

  bPrevConfig = cp5.addButton("prevConfig");
  bPrevConfig.setSize(90, 25);
  bPrevConfig.setPosition(startX + 670, 180);
  setColorToButton(bPrevConfig, GRAY);
  bPrevConfig.setColorActive(GREEN);
  bPrevConfig.getCaptionLabel().alignX(CENTER);
  bPrevConfig.getCaptionLabel().setFont(ft);
  bPrevConfig.setLabel("Prev Config");
}

void activateConfig(int config){
  activeConfig = config%maxConfigs;
  if (bColor==null || ledConfigs==null) return;
  for (int i=0; i<numLeds; i++){
    int col = bColor[ledConfigs.get(activeConfig).get(i)%maxColors].getColor().getBackground();
    setColorToButton(bLed[i], col);
  }
}

void setColorToButton(Button bb, int c){
  bb.setColorActive(c);
  bb.setColorBackground(c);
  bb.setColorForeground(c);
  int r = (int)red(c);
  int g = (int)green(c);
  int b = (int)blue(c);
  int r2 = 255 - r;
  int g2 = 255 - g;
  int b2 = 255 - b;
  if (abs(r-r2)<30) r2 = 0;
  if (abs(g-g2)<30) g2 = 0;
  if (abs(b-b2)<30) b2 = 0;
  int ic = color(r2, g2, b2);
  bb.setColorCaptionLabel(ic);
  bb.setColorValueLabel(ic);
}

int p;
int read32() {return (inBuf[p++]&0xff) + ((inBuf[p++]&0xff)<<8) + ((inBuf[p++]&0xff)<<16) + ((inBuf[p++]&0xff)<<24); }
int read16() {return (inBuf[p++]&0xff) + ((inBuf[p++])<<8); }
int read8()  {return inBuf[p++]&0xff;}

String shortifyPortName(String portName, int maxlen)  {
  String shortName = portName;
  if(shortName.startsWith("/dev/")) shortName = shortName.substring(5);  
  if(shortName.startsWith("tty.")) shortName = shortName.substring(4); // get rid of leading tty. part of device name
  if(portName.length()>maxlen) shortName = shortName.substring(0,(maxlen-1)/2) + "~" +shortName.substring(shortName.length()-(maxlen-(maxlen-1)/2));
  if(shortName.startsWith("cu.")) shortName = "";// only collect the corresponding tty. devices
  return shortName;
}
  
void InitSerial(int portValue) {
  if (initCom){
    g_serial.clear();
    g_serial.stop();
  }
  String portPos = Serial.list()[portValue];
  selectedPort = shortifyPortName(portPos, 8);
  String shortPortName = "COM = " + selectedPort;
  g_serial = new Serial(this, portPos, GUI_BAUD_RATE);
  g_serial.buffer(BUFFER_SIZE);
  initCom = true;
}
  
void sendRequestMSP(List<Byte> msp) {
  byte[] arr = new byte[msp.size()];
  int i = 0;
  for (byte b: msp) {
    arr[i++] = b;
  }
  g_serial.write(arr); // send the complete byte sequence in one go
}

private List<Byte> requestMSP(int msp) {
  return  requestMSP( msp, null);
}

private List<Byte> requestMSP(int msp, Character[] payload) {
  if(msp < 0) {
   return null;
  }
  List<Byte> bf = new LinkedList<Byte>();
  for (byte c : MSP_HEADER.getBytes()) {
    bf.add( c );
  }
  byte checksum=0;
  byte pl_size = (byte)((payload != null ? int(payload.length) : 0)&0xFF);
  bf.add(pl_size);
  checksum ^= (pl_size&0xFF);
  bf.add((byte)(msp & 0xFF));
  checksum ^= (msp&0xFF);
  if (payload != null) {
    for (char c :payload){
      bf.add((byte)(c&0xFF));
      checksum ^= (c&0xFF);
    }
  }
  bf.add(checksum);
  return (bf);
}

public void evaluateCommand(byte cmd, int dataSize) {
  int icmd = (int)(cmd&0xFF);
  switch(icmd) {
    case MSP_SETUP:
      if (setupStatus == NEED_SETUP){
        numArms = read8();
        ledsPerArm = read8();
        maxColors = read8();
        maxConfigs = read8();
        numLeds = numArms * ledsPerArm;
        dlPorts.setVisible(false);
        tlPorts.setText("Port: " + selectedPort);
        setupStatus = NEED_COLORS;
      }
      break;
    case MSP_COLORS:
      List<Integer> colors = new ArrayList<Integer>(maxColors);
      for (int i=0; i<maxColors; i++){
        int r = read8();
        int g = read8();
        int b = read8();
        int col = color(r, g, b);
        if (setupStatus == NEED_COLORS) colors.add(col);
        else if (setupStatus>NEED_COLORS) setColorToButton(bColor[i], col);
      }
      if (setupStatus == NEED_COLORS) createColorButtons(colors, 20, 50, 90, 45);
      setupStatus = max(setupStatus, NEED_CONFIGS);
      activateButton(activeButton);
      break;
    case MSP_CONFIGS:
      boolean update = ledConfigs!=null;
      if (!update) ledConfigs = new ArrayList<List<Integer>>(maxConfigs);
      for (int i=0; i<maxConfigs; i++){
        int iLed = 0;
        if (!update) ledConfigs.add(new ArrayList<Integer>(numLeds));         
        for (int j=0; j<((numLeds/2)+1); j++){
          int c = read8();
          if (iLed >= numLeds) continue;
          int c1 = (c >> 4);
          if (update) ledConfigs.get(i).set(iLed, c1);
          else ledConfigs.get(i).add(c1);
          iLed++;
          if (iLed >= numLeds) continue;
          int c2 = (c & 0xF);
          if (update) ledConfigs.get(i).set(iLed, c2);
          else ledConfigs.get(i).add(c2);
          iLed++;
        }
      }
      if (setupStatus == NEED_CONFIGS) createLedButtons(ledConfigs.get(activeConfig), 20, 220, 45, 45, 2, 2);
      setupStatus = max(setupStatus, DONE);
      activateConfig(activeConfig);
      break;
    default:
        //println("Don't know how to handle reply "+icmd);
  }
}

public void setColors(){
  List<Character> payload = new ArrayList<Character>();
  for (int i=0; i<maxColors; i++){
    int col = bColor[i].getColor().getBackground();
    int r = (int)red(col);
    int g = (int)green(col);
    int b = (int)blue(col);
    payload.add(char(r));
    payload.add(char(g));
    payload.add(char(b));
  }
  sendRequestMSP(requestMSP(MSP_SET_COLORS, payload.toArray( new Character[payload.size()]) )); 
}

public void getColors(){
  sendRequestMSP(requestMSP(MSP_COLORS));
}

public void saveColors(){
  setColors();
  sendRequestMSP(requestMSP(MSP_SAVE_COLORS));
}

public void getConfigs(){
  sendRequestMSP(requestMSP(MSP_CONFIGS));
  activateConfig(activeConfig);
}

public void setConfigs(){
  List<Character> payload = new ArrayList<Character>();
  int value = 0;
  payload.add(char(activeConfig));
  for (int i=0; i<numLeds; i++){
    if (i%2==0){
      value = (ledConfigs.get(activeConfig).get(i).intValue() << 4);
    }
    else{
      value += ledConfigs.get(activeConfig).get(i).intValue();
      payload.add(char(value));
      value = 0;
    }
  }
  if (numLeds%2==0) payload.add(char(0));
  else payload.add(char(value));
  sendRequestMSP(requestMSP(MSP_SET_CONFIGS, payload.toArray( new Character[payload.size()]) )); 
}

public void saveConfigs(){
  setConfigs();
  sendRequestMSP(requestMSP(MSP_SAVE_CONFIGS));
}

public void nextMode(){
  sendRequestMSP(requestMSP(MSP_NEXT_MODE));
}

public void prevMode(){
  sendRequestMSP(requestMSP(MSP_PREV_MODE));
}

public void nextConfig(){
  sendRequestMSP(requestMSP(MSP_NEXT_CONFIG));
  activateConfig((activeConfig+1) % maxConfigs);
}

public void prevConfig(){
  sendRequestMSP(requestMSP(MSP_PREV_CONFIG));
  activateConfig((activeConfig+(maxConfigs-1)) % maxConfigs);
}

private long time = millis();
private void readSerialMessages(){
  int temp;
  
  if (!initCom) return;
  if (setupStatus<DONE){
    long newTime = millis();
    if ((newTime - time) > 500){
      time = newTime;
      switch(setupStatus){
        case NEED_SETUP:{
          sendRequestMSP(requestMSP(MSP_SETUP));
        }
        break;
        case NEED_COLORS:{
          getColors();
        }
        break;
        case NEED_CONFIGS:{
          sendRequestMSP(requestMSP(MSP_CONFIGS));
        }
        break;
      }
    }
  }
  int c, i;
  while (g_serial.available()>0) {
    c = (g_serial.read());
    if (c_state == IDLE) {
      c_state = (c=='$') ? HEADER_START : IDLE;
    } else if (c_state == HEADER_START) {
      c_state = (c=='M') ? HEADER_M : IDLE;
    } else if (c_state == HEADER_M) {
      if (c == '>') {
        c_state = HEADER_ARROW;
      } else if (c == '!') {
        c_state = HEADER_ERR;
      } else {
        c_state = IDLE;
      }
    } else if (c_state == HEADER_ARROW || c_state == HEADER_ERR) {
      /* is this an error message? */
      err_rcvd = (c_state == HEADER_ERR);        /* now we are expecting the payload size */
      dataSize = (c&0xFF);
      /* reset index variables */
      p = 0;
      offset = 0;
      checksum = 0;
      checksum ^= (c&0xFF);
      c_state = HEADER_SIZE1;
    } else if (c_state == HEADER_SIZE1) {
      temp = (c&0xFF) *256;      
      dataSize += temp;
      checksum ^= (c&0xFF);
      c_state = HEADER_SIZE2;
    } else if (c_state == HEADER_SIZE2) {
      cmd = (byte)(c&0xFF);
      checksum ^= (c&0xFF);
      c_state = HEADER_CMD;
    } else if (c_state == HEADER_CMD && offset < dataSize) {
        checksum ^= (c&0xFF);
        inBuf[offset++] = (byte)(c&0xFF);
    } else if (c_state == HEADER_CMD && offset >= dataSize) {
      /* compare calculated and transferred checksum */
      if ((checksum&0xFF) == (c&0xFF)) {
/*V: uncomment for debug purposes only */
/*
        System.out.println("OK "+((int)(cmd&0xFF))+": "+(checksum&0xFF)+" expected, got "+(int)(c&0xFF));
        System.out.print("<"+(cmd&0xFF)+" "+(dataSize)+"> {");
        for (i=0; i<dataSize; i++) {
          if (i!=0) { System.err.print(' '); }
          System.out.print((inBuf[i] & 0xFF));
          System.out.print(" ");
        }
        System.out.println("} ["+c+"]");
        System.out.println(new String(inBuf, 0, dataSize));
/* */
        if (err_rcvd) {
          //System.err.println("Copter did not understand request type "+c);
        } else {
          /* we got a valid response packet, evaluate it */
          evaluateCommand(cmd, (int)dataSize);
        }
      } else {
        System.out.println("invalid checksum for command "+((int)(cmd&0xFF))+": "+(checksum&0xFF)+" expected, got "+(int)(c&0xFF));
        System.out.print("<"+(cmd&0xFF)+" "+(dataSize)+"> {");
        for (i=0; i<dataSize; i++) {
          if (i!=0) { System.err.print(' '); }
          System.out.print((inBuf[i] & 0xFF));
          System.out.print(" ");
        }
        System.out.println("} ["+c+"]");
        System.out.println(new String(inBuf, 0, dataSize));
      }
      c_state = IDLE;
    }
  }
}
