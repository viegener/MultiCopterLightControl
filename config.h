#define DEBUG // uncomment to remove debug output on serial

#define NUM_ARMS 4       // number of arms on the multicopter
#define LEDS_PER_ARM 6   // leds per arm
#define LED_PIN 12       // led pin => data of led strip

// defines for modes, change if you don't some modes
#define NUM_MODES 8              // number of different mode
#define MODE_SAVED_COLORS 0      // show stored colors
#define MODE_RAINBOW_ARMS 1      // cycle through all colors per arm (all arms identical, one arm_led one color)
#define MODE_RAINBOW 2           // cycle through all colors all leds (one led on color)
#define MODE_RAINBOW_CYCLE 3     // cycle through all colors arm-wise (one arm one color)
#define MODE_CYCLING_DOT 4       // one dot cycle through the arm (inside-out or reversed)
#define MODE_POLICE 5            // police light in given colors
#define MODE_SCANNER_ALL 6       // scanner in given color
#define MODE_RUNNING_DOT 7       // one dot running through all leds

#define MODE_DEFAULT MODE_SAVED_COLORS

// do not modify anything below this line

#define NUM_LEDS (NUM_ARMS * LEDS_PER_ARM) // calculate total number of leds

// defines for debug output
#ifdef DEBUG
  #define DEBUG_PRINT(x)  Serial.print (x)
  #define DEBUG_PRINTLN(x)  Serial.println (x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

