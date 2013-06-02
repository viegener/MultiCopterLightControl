#if defined (INPUT_GUI)

  #define INBUF_SIZE 512
  #define TX_BUFFER_SIZE 512
  #define RX_BUFFER_SIZE 512
  
  #define MSP_SETUP        150
  #define MSP_COLORS       151
  #define MSP_CONFIGS      152
  #define MSP_SET_COLORS   230
  #define MSP_SET_CONFIGS  231
  #define MSP_SAVE_COLORS  232
  #define MSP_SAVE_CONFIGS 233
  #define MSP_NEXT_MODE    234
  #define MSP_PREV_MODE    235
  #define MSP_NEXT_CONFIG  236
  #define MSP_PREV_CONFIG  237
  
  #define HEADER_IDLE   0
  #define HEADER_START  1
  #define HEADER_M      2
  #define HEADER_ARROW  3
  #define HEADER_SIZE   4
  #define HEADER_CMD    5
  #define HEADER_ERR    6

  uint8_t checksum, indRX, cmdMSP;
  uint8_t serialBufferRX[RX_BUFFER_SIZE];
  volatile uint8_t serialHeadRX, serialTailRX;
  volatile uint8_t serialHeadTX, serialTailTX;
  uint8_t serialBufferTX[TX_BUFFER_SIZE];
  uint8_t inBuf[INBUF_SIZE];

  void setupInputGUI(){
    Serial.begin(115200);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
  }
  
  void loopInputGUI(){
    serialCom();
  }
  
  void skip8(){
    indRX++;
  }
  void skip16(){
    indRX+=2;
  }
  void skip32(){
    indRX+=4;
  }
  
  uint32_t read32() {
    uint32_t t = read16();
    t+= (uint32_t)read16()<<16;
    return t;
  }
  uint16_t read16() {
    uint16_t t = read8();
    t+= (uint16_t)read8()<<8;
    return t;
  }
  uint8_t read8()  {
    return inBuf[indRX++]&0xff;
  }
  
  void headSerialReply(uint8_t s) {
    headSerialResponse(0, s);
  }
  
  void tailSerialReply() {
    serialize8(checksum);
    UartSendData();
  }
  
  void UartSendData() {
    while(serialHeadTX != serialTailTX) {
      if (++serialTailTX >= TX_BUFFER_SIZE) serialTailTX = 0;
      Serial.write(serialBufferTX[serialTailTX]);
    }
  }

  void headSerialResponse(uint8_t err, uint8_t s) {
    serialize8('$');
    serialize8('M');
    serialize8(err ? '!' : '>');
    checksum = 0; // start calculating a new checksum
    serialize8(s);
    serialize8(cmdMSP);
  }
  
  void serialize32(uint32_t a) {
    serialize8((a    ) & 0xFF);
    serialize8((a>> 8) & 0xFF);
    serialize8((a>>16) & 0xFF);
    serialize8((a>>24) & 0xFF);
  }

  void serialize16(int16_t a) {
    serialize8((a   ) & 0xFF);
    serialize8((a>>8) & 0xFF);
  }

  void serialize8(uint8_t a) {
    uint8_t t = serialHeadTX;
    if (++t >= TX_BUFFER_SIZE) t = 0;
    serialBufferTX[t] = a;
    checksum ^= a;
    serialHeadTX = t;
  }
  
  void evaluateCommand() {
    switch(cmdMSP) {
      case MSP_SET_COLORS:
        headSerialReply(0);
        for (int i=0; i<MAX_EEPROM_COLORS; i++){
          uint8_t r = read8();
          uint8_t g = read8();
          uint8_t b = read8();
          CRGB rgb = getCRGB(r, g, b);
          storedColorsRGB[i] = rgb;
          storedColorsHSV[i] = rgb2hsv(storedColorsRGB[i]);
        }
      break;
      case MSP_SAVE_COLORS:
        headSerialReply(0);
        writeColors();
      break;
      case MSP_SAVE_CONFIGS:
        headSerialReply(0);
        writeLeds();
      break;
      case MSP_SET_CONFIGS:
        headSerialReply(0);
        config = read8();
        for (int i=0; i<((NUM_LEDS/2)+1); i++){
          storedLedColors[config][i] = read8();
        }
      break;
      case MSP_CONFIGS:
        readLeds();
        headSerialReply(MAX_LED_CONFIGS * ((NUM_LEDS/2)+1));
        for (int i=0; i<MAX_LED_CONFIGS; i++){
          for (int j=0; j<((NUM_LEDS/2)+1); j++){
            serialize8(storedLedColors[i][j]);
          }
        }
      break;
      case MSP_SETUP:
        headSerialReply(4);
        serialize8(NUM_ARMS);
        serialize8(LEDS_PER_ARM);
        serialize8(MAX_EEPROM_COLORS);
        serialize8(MAX_LED_CONFIGS);
      break;
      case MSP_COLORS:
        readColors();
        headSerialReply(3*MAX_EEPROM_COLORS);
        for (int i=0; i<MAX_EEPROM_COLORS; i++){
          serialize8(storedColorsRGB[i].r);
          serialize8(storedColorsRGB[i].g);
          serialize8(storedColorsRGB[i].b);
        }
      break;
      case MSP_NEXT_MODE:
        headSerialReply(0);
        mode = (mode+1) % NUM_MODES;
      break;
      case MSP_PREV_MODE:
        headSerialReply(0);
        mode = (mode+(NUM_MODES-1)) % NUM_MODES;
      break;
      case MSP_NEXT_CONFIG:
        headSerialReply(0);
        config = (config+1) % MAX_LED_CONFIGS;
      break;
      case MSP_PREV_CONFIG:
        headSerialReply(0);
        config = (config+(MAX_LED_CONFIGS-1)) % MAX_LED_CONFIGS;
      break;
    }
    tailSerialReply();
  }

void serialCom() {
  uint8_t c,n;  
  static uint8_t offset;
  static uint8_t dataSize;
  uint8_t c_state = HEADER_IDLE;
  
  while (Serial.available()>0) {
      digitalWrite(13, HIGH);
      uint8_t bytesTXBuff = ((uint8_t)(serialHeadTX-serialTailTX))%TX_BUFFER_SIZE; // indicates the number of occupied bytes in TX buffer
      if (bytesTXBuff > TX_BUFFER_SIZE - 50 ) return; // ensure there is enough free TX buffer to go further (50 bytes margin)
      c = Serial.read();
      // regular data handling to detect and handle MSP and other data
        if (c_state == HEADER_IDLE) {
          c_state = (c=='$') ? HEADER_START : HEADER_IDLE;
        } else if (c_state == HEADER_START) {
          c_state = (c=='M') ? HEADER_M : HEADER_IDLE;
        } else if (c_state == HEADER_M) {
          c_state = (c=='<') ? HEADER_ARROW : HEADER_IDLE;
        } else if (c_state == HEADER_ARROW) {
          if (c > INBUF_SIZE) {  // now we are expecting the payload size
            c_state = HEADER_IDLE;
            continue;
          }
          dataSize = c;
          offset = 0;
          checksum = 0;
          indRX = 0;
          checksum ^= c;
          c_state = HEADER_SIZE;  // the command is to follow
        } else if (c_state == HEADER_SIZE) {
          cmdMSP = c;
          checksum ^= c;
          c_state = HEADER_CMD;
        } else if (c_state == HEADER_CMD && offset < dataSize) {
          checksum ^= c;
          inBuf[offset++] = c;
        } else if (c_state == HEADER_CMD && offset >= dataSize) {
          if (checksum == c) {  // compare calculated and transferred checksum
            evaluateCommand();  // we got a valid packet, evaluate it
          }
          c_state = HEADER_IDLE;
       }
       digitalWrite(13, LOW);
    }
  }

#endif
