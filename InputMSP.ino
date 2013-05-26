#if defined (INPUT_MSP)

  #define MSP_IDENT                100   //out message         multitype + multiwii version + protocol version + capability variable
  #define MSP_STATUS               101   //out message         cycletime & errors_count & sensor present & box activation & current setting number
  #define MSP_RAW_IMU              102   //out message         9 DOF
  #define MSP_SERVO                103   //out message         8 servos
  #define MSP_MOTOR                104   //out message         8 motors
  #define MSP_RC                   105   //out message         8 rc chan and more
  #define MSP_RAW_GPS              106   //out message         fix, numsat, lat, lon, alt, speed, ground course
  #define MSP_COMP_GPS             107   //out message         distance home, direction home
  #define MSP_ATTITUDE             108   //out message         2 angles 1 heading
  #define MSP_ALTITUDE             109   //out message         altitude, variometer
  #define MSP_ANALOG               110   //out message         vbat, powermetersum, rssi if available on RX
  #define MSP_RC_TUNING            111   //out message         rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
  #define MSP_PID                  112   //out message         P I D coeff (9 are used currently)
  #define MSP_BOX                  113   //out message         BOX setup (number is dependant of your setup)
  #define MSP_MISC                 114   //out message         powermeter trig
  #define MSP_MOTOR_PINS           115   //out message         which pins are in use for motors & servos, for GUI 
  #define MSP_BOXNAMES             116   //out message         the aux switch names
  #define MSP_PIDNAMES             117   //out message         the PID names
  #define MSP_WP                   118   //out message         get a WP, WP# is in the payload, returns (WP#, lat, lon, alt, flags) WP#0-home, WP#16-poshold
  #define MSP_BOXIDS               119   //out message         get the permanent IDs associated to BOXes
 
  #define HEADER_IDLE   0
  #define HEADER_START  1
  #define HEADER_M      2
  #define HEADER_ARROW  3
  #define HEADER_SIZE   4
  #define HEADER_CMD    5
  #define HEADER_ERR    6
  
  struct flags_struct {
    uint8_t OK_TO_ARM :1 ;
    uint8_t ARMED :1 ;
    uint8_t I2C_INIT_DONE :1 ;
    uint8_t ACC_CALIBRATED :1 ;
    uint8_t NUNCHUKDATA :1 ;
    uint8_t ANGLE_MODE :1 ;
    uint8_t HORIZON_MODE :1 ;
    uint8_t MAG_MODE :1 ;
    uint8_t BARO_MODE :1 ;
    uint8_t GPS_HOME_MODE :1 ;
    uint8_t GPS_HOLD_MODE :1 ;
    uint8_t HEADFREE_MODE :1 ;
    uint8_t PASSTHRU_MODE :1 ;
    uint8_t GPS_FIX :1 ;
    uint8_t GPS_FIX_HOME :1 ;
    uint8_t SMALL_ANGLES_25 :1 ;
    uint8_t CALIBRATE_MAG :1 ;
    uint8_t VARIO_MODE :1;

    uint8_t AVAIL_ACC :1;
    uint8_t AVAIL_BARO :1;
    uint8_t AVAIL_MAG :1;
    uint8_t AVAIL_GPS :1;
    uint8_t AVAIL_SONAR :1;
  } f;

  int
    mw_version = 0,
    multiType = 0,
    multiCapability = 0,
    present = 0, 
    mw_mode = 0,
    msp_version = 0,
    ax = 0,
    ay = 0,
    az = 0,
    gx = 0,
    gy = 0,
    gz = 0,
    magx = 0,
    magy = 0,
    magz = 0,
    rcRoll = 0,
    rcPitch = 0,
    rcYaw = 0,
    rcThrottle = 0,
    rcAUX1 = 0,
    rcAUX2 = 0,
    rcAUX3 = 0,
    rcAUX4 = 0
  ;

  int
    mot[8] = {0, 0, 0, 0, 0, 0, 0, 0}
  ;
  
  int
    idxArm = -1,
    idxAngle = -1,
    idxHorizon = -1,
    idxBaro = -1,
    idxVario = -1,
    idxMag = -1,
    idxHeadFree = -1,
    idxHEadAdj = -1,
    idxCamStab = -1,
    idxCamTrig = -1,
    idxGpsHome = -1,
    idxGpsHold = -1,
    idxPassthru = -1,
    idxBeeper = -1,
    idxLedMax = -1,
    idxLedLow = -1,
    idxLLights = -1,
    idxCalib = -1,
    idxGovenor = -1,
    idxOsdSw = -1
  ;

  int c_state = HEADER_IDLE;
  boolean err_rcvd = false;
  int offset=0, dataSize=0;
  int p;
  byte checksum=0;
  byte cmd;
  byte inBuf[256];
  int iStatus = 0;
  boolean bStatus[20] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
  
  boolean initDone = false;
  long time = millis();
  
  int requestDelay = 100;
  
  void setupInputMSP(){
    blinkLed(3, 250);
    delay(9000);
    Serial.begin(115200);
    sendRequestMSP(MSP_BOXNAMES);
  }
  
  void loopInputMSP(){
    while(Serial.available()>0){
      byte c = (Serial.read());
      if (c_state == HEADER_IDLE) c_state = (c=='$') ? HEADER_START : HEADER_IDLE;
      else if (c_state == HEADER_START) c_state = (c=='M') ? HEADER_M : HEADER_IDLE;
      else if (c_state == HEADER_M){
        if (c == '>') c_state = HEADER_ARROW;
        else{
          if (c == '!') c_state = HEADER_ERR;
          else c_state = HEADER_IDLE;
        }
      }
      else if (c_state == HEADER_ARROW || c_state == HEADER_ERR){
        /* is this an error message? */
        err_rcvd = (c_state == HEADER_ERR);        /* now we are expecting the payload size */
        dataSize = (c&0xFF);
        /* reset index variables */
        p = 0;
        offset = 0;
        checksum = 0;
        checksum ^= (c&0xFF);
        /* the command is to follow */
        c_state = HEADER_SIZE;
      }
      else if (c_state == HEADER_SIZE) {
        cmd = (byte)(c&0xFF);
        checksum ^= (c&0xFF);
        c_state = HEADER_CMD;
      }
      else if (c_state == HEADER_CMD && offset < dataSize){
        checksum ^= (c&0xFF);
        inBuf[offset++] = (byte)(c&0xFF);
      }
      else if (c_state == HEADER_CMD && offset >= dataSize){
        /* compare calculated and transferred checksum */
        if ((checksum&0xFF) == (c&0xFF) && !err_rcvd) evaluateCommand(cmd, (int)dataSize);
        c_state = HEADER_IDLE;
      }
    }
    if (!initDone) return;
    if ((millis() - time) > REQUEST_DELAY){
      sendRequestMSP(MSP_STATUS);
#if defined (REQUEST_MOTORS)
      sendRequestMSP(MSP_MOTOR);
#endif
      time = millis();
    }
    if (idxArm<0 || !bStatus[idxArm]) mode = MODE_UNARMED;
    else{
      // copter is armed
      if (idxAngle>0 && bStatus[idxAngle]) mode = MODE_ANGLE;
      else if (idxHorizon>0 && bStatus[idxHorizon]) mode = MODE_HORIZON;
      else mode = MODE_ACRO;
    }
  }
  
  void skip8(){
    p++;
  }
  void skip16(){
    p+=2;
  }
  void skip32(){
    p+=4;
  }
  
  uint8_t read8()  {
    return inBuf[p++]&0xff;
  }
  uint16_t read16() {
    uint16_t t = read8();
    t+= (uint16_t)read8()<<8;
    return t;
  }
  uint32_t read32() {
    uint32_t t = read16();
    t+= (uint32_t)read16()<<16;
    return t;
  }
  
  void sendRequestMSP(int msp) {
    if(msp < 0) return;
    byte bf[6];
    int counter = 0;
    bf[counter++] = (byte)'$';
    bf[counter++] = (byte)'M';
    bf[counter++] = (byte)'<';
    byte checksum=0;
    byte pl_size = (byte)(0&0xFF);
    bf[counter++] = pl_size;
    checksum ^= (pl_size&0xFF);
    bf[counter++] = (byte)(msp & 0xFF);
    checksum ^= (msp&0xFF);
    bf[counter++] = checksum;
    Serial.write(bf, 6);
  }
  
  void updateStatus(){
//    if ((present&1) >0) f.AVAIL_ACC = 1; else f.AVAIL_ACC = 0;
//    if ((present&2) >0) f.AVAIL_BARO = 1; else f.AVAIL_BARO = 0;
//    if ((present&4) >0) f.AVAIL_MAG = 1; else f.AVAIL_MAG = 0;
//    if ((present&8) >0) f.AVAIL_GPS = 1; else f.AVAIL_GPS = 0;
//    if ((present&16)>0) f.AVAIL_SONAR = 1; else f.AVAIL_SONAR = 0;
    for(int i=0; i<iStatus; i++) {
      if ((mw_mode&(1<<i))>0) bStatus[i]=true;
      else bStatus[i]=false;
    }
  }
  
  void initBoxNames(){
    char str[dataSize];
    for (int i=0; i<dataSize; i++) str[i] = (char)inBuf[i];
    String text = str;
    char splitChar = ';';
    while (text.startsWith(String(splitChar)) && text.length()>0) text = text.substring(1, text.length());
    while (text.length()>0 && text.charAt(text.length()-1)==splitChar) text = text.substring(0, text.length()-1);
    iStatus = countSplitCharacters(text, splitChar);
    int index = -1;
    int lastIndex = -1;
    int i=0;
    do{
      lastIndex = index + 1;
      index = text.indexOf(splitChar, lastIndex);
      int idx = index;
      if (idx==-1) idx=text.length();
      String bName = text.substring(lastIndex, idx);
      if (bName == "ARM") idxArm = i;
      else if (bName == "ANGLE") idxAngle = i;
      else if (bName == "HORIZON") idxHorizon = i;
      else if (bName == "BARO") idxBaro = i;
      else if (bName == "VARIO") idxVario = i;
      else if (bName == "MAG") idxMag = i;
      else if (bName == "HEADFREE") idxHeadFree = i;
      else if (bName == "HEADADJ") idxHEadAdj = i;
      else if (bName == "CAMSTAB") idxCamStab = i;
      else if (bName == "CAMTRIG") idxCamTrig = i;
      else if (bName == "GPS HOME") idxGpsHome = i;
      else if (bName == "GPS HOLD") idxGpsHold = i;
      else if (bName == "PASSTHRU") idxPassthru = i;
      else if (bName == "BEEPER") idxBeeper = i;
      else if (bName == "LEDMAX") idxLedMax = i;
      else if (bName == "LEDLOW") idxLedLow = i;
      else if (bName == "LLIGHTS") idxLLights = i;
      else if (bName == "CALIB") idxCalib = i;
      else if (bName == "GOVERNOR") idxGovenor = i;
      else if (bName == "OSD SW") idxOsdSw = i;
      i++;
    } while (index > -1);
    if (iStatus>0){
      initDone = true;
    }
    else sendRequestMSP(MSP_BOXNAMES);
  }

  int countSplitCharacters(String text, char splitChar) {
    if (text==NULL) return 0;
    int returnValue = 1;
    int index = -1;
    do{
      index = text.indexOf(splitChar, index + 1);
      if (index > -1) returnValue += 1;
    } while (index > -1);
    return returnValue;
  }

  void evaluateCommand(byte cmd, int dataSize) {
    int i;
    int icmd = (int)(cmd&0xFF);
    if (icmd == MSP_IDENT){
      mw_version = read8();
      multiType = read8();
      msp_version = read8(); // MSP version
      multiCapability = read32();// capability
    }
    else if (icmd == MSP_STATUS){
      skip16();
      skip16();
      skip16();
//      present = read16();
      mw_mode = read32();
      updateStatus();
    }
    else if (icmd == MSP_RAW_IMU){
      ax = read16();
      ay = read16();
      az = read16();
      gx = read16()/8;
      gy = read16()/8;
      gz = read16()/8;
      magx = read16()/3;
      magy = read16()/3;
      magz = read16()/3;
    }
    else if (icmd == MSP_MOTOR){
      for(i=0;i<8;i++){
        mot[i] = read16();
      }
    }
    else if (icmd == MSP_RC){
      rcRoll = read16();
      rcPitch = read16();
      rcYaw = read16();
      rcThrottle = read16();    
      rcAUX1 = read16();
      rcAUX2 = read16();
      rcAUX3 = read16();
      rcAUX4 = read16();
    }
    else if (icmd == MSP_BOXNAMES){
      initBoxNames();
    }
  }
 
#endif
