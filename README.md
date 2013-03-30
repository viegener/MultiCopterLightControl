MultiCopterLightControl
=======================

First Version of MultiCopterLightControl (MCLC)

MCLC is an open source project to control a WS2811 RGB LED strip with an arduino. It's main goal is to support diffent lightning modes for multicopters. Even though it can be used for other lightning projects like MoodLamps etc. with some (small) modifications. At the moment, the project only support some lightning effects and the control of the program is limited to serial commands. 

I decided to use FastSPI_LED2, even it's in beta phase, because it's very lightweight compared to the first version of FastSPI_LED. My respect and thanks to the developer of FastSPI_LED, which is an very easy to use piece of software.
https://code.google.com/p/fastspi/

Now I will explain in short, the current structure of the arduino code, followed by available effects and settings.


Code Structure
==============
The code is divided into 5 files.

MultiCopterLightControl.ino
Main file with setup() and loop() method with controll structures to switch between lightning modes.
Some very basic methods to modify leds are implemented also in this file, like methods to set a given color to a complete arm, clear all leds, update the current leds, save led colors to eeprom and thing like that. It also brings all the other files together.

structs.h
Holds all definitions of data structures we use in the project

config.h
This file can be modified to meet your own needs. See below for the diffent available settings.

SerialCommands.ino
This file defines/executes all serial commands. Later on, this file can be replaced or another file added to provide other control interfaces. See below for a list of available commands.

Sequences.ino
In this file, all lightning effects are implemented. See below for a list of available effects.


Settings
========
NUM_ARMS        = Number of arms of the multicopter
LEDS_PER_ARM    = LEDs per arm
LED_PIN         = digital pin on the arduino to connect the data pin of the LED strip

Uncomment "#define DEBUG" to disable debug output on serial.
Some modes are also defined and you can set an default mode. This mode will be used, if no valid mode selected.


Serial Commands
===============
Color settings
setR N           : value for the red portion of the current color, 0 <= N < 256
setG N           : value for the green portion of the current color, 0 <= N < 256
setB N           : value for the blue portion of the current color, 0 <= N < 256
setArm N         : apply the current color given arm, 0 <= N < NUM_ARMS
save             : save the current color of the LEDs to EEPROM
clear            : set all LEDs to black (0, 0, 0)

Effect settings
setV             : set the brightness of some effects
d N              : set the delay of some effects, 0 < N < 10000
rev              : reverse some effects

Mode settings
+                : select next mode
-                : select previous mode
m N              : set mode to N, 0 <= m < NUM_MODES


Effects
=======
Modename, description and available settings of the effect.
R = REVERSE, D = DELAY, B = BRIGHTNESS, C = COLOR
MODE_SAVED_COLORS       : show in EEPROM stored colors [C]
MODE_RAINBOW            : cycle through all colors all leds (one led on color) [R, D, B]
MODE_RAINBOW_ARMS       : cycle through all colors per arm (all arms identical, one arm_led one color) [R, D, B]
MODE_RAINBOW_CYCLE      : cycle through all colors arm-wise (one arm one color) [R, D, B]
MODE_CYCLING_DOT        : one dot cycle through the arms (inside-out or reversed) [R, D, C]
MODE_POLICE             : blinking arms in two different color, currentColor + color shift [R, D, C]
MODE_SCANNER_ALL        : scanner in given color [D, C]
MODE_RUNNING_DOT        : one dot running through all leds [R, D, C]


