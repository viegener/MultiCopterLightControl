#define DEBUG // uncomment to remove debug output on serial

#define NUM_ARMS 4       // number of arms on the multicopter
#define LEDS_PER_ARM 6   // leds per arm
#define LED_PIN 12       // led pin => data of led strip

#define MODE_DEFAULT MODE_SAVED_COLORS

// defines for modes, change if you want to youse more/other modes
#define NUM_MODES 5              // number of different mode
#define MODE_SAVED_COLORS 0      // show stored colors
#define MODE_RUNNING_LED 1      // show stored colors

#define MODE_RUNNING_LED_TEST1 2      // show stored colors
#define MODE_RUNNING_LED_TEST2 3      // show stored colors
#define MODE_RUNNING_LED_TEST3 4      // show stored colors



////////////////////////////////////////////////////
// !!! Do not modify anything below this line !!! //
////////////////////////////////////////////////////

#define NUM_LEDS (NUM_ARMS * LEDS_PER_ARM) // calculate total number of leds

// defines for debug output
#ifdef DEBUG
  #define DEBUG_PRINT(x)  Serial.print (x)
  #define DEBUG_PRINTLN(x)  Serial.println (x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

