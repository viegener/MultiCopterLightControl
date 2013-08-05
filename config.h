// select (uncomment) one of the following input modes
//#define INPUT_TERMINAL      // use serial terminal to control lightning mode and configure led colors
#define INPUT_GUI           // use GUI to configure led colors
//#define INPUT_RC            // use rc inputs to control lightning mode
//#define INPUT_MSP           // use MultiWii Serial Protcol to control lightning mode

#define NUM_ARMS 4             // number of arms on the multicopter
#define LEDS_PER_ARM 20        // leds per arm
#define LED_PIN 12             // led pin => data of led strip

// input mode depending configs
#if defined (INPUT_TERMINAL) || defined (INPUT_GUI)
  #define NUM_MODES               4
#elif defined (INPUT_RC)
  #define RC_PIN_1                10            // PIN for RC input channel 1

  #define MODE_RC_LOW             MODE_0
  #define MODE_RC_MID             MODE_1
  #define MODE_RC_HIGH            MODE_2
  #define MODE_RC_NO_CONNECTION   MODE_3
#elif defined (INPUT_MSP)
//  #define REQUEST_MOTORS                    // send motor request message to MultiWii-FC, only needed if motor output is needed
  #define REQUEST_DELAY           250         // request delay, smaller delay = faster update of MultiWii values = more cpu power
  
  #define MODE_UNARMED            MODE_3
  #define MODE_ACRO               MODE_0
  #define MODE_ANGLE              MODE_1
  #define MODE_HORIZON            MODE_2
#endif

#if defined (INPUT_MSP)
  #if defined (REQUEST_MOTORS)
    int motorArmMapping[8] = {2, 1, 3, 0, -1, -1, -1, -1};  // 8 motor outputs from MultiWii-FC, mapping to the corresponding arm index of the leds
  #endif
#endif



/////////////////////////////////////////////////////////////////////////////////
// !!! Do not modify anything below this line, except you know what you do !!! //
/////////////////////////////////////////////////////////////////////////////////

#define MODE_0 0
#define MODE_1 1
#define MODE_2 2
#define MODE_3 3

#define NUM_LEDS (NUM_ARMS * LEDS_PER_ARM) // calculate total number of leds

#if defined (INPUT_TERMINAL)
  #define SERIAL_PRINT(x) Serial.print(x)
  #define SERIAL_PRINTLN(x) Serial.println(x)
#else
  #define SERIAL_PRINT(x)
  #define SERIAL_PRINTLN(x)
#endif
