MultiCopterLightControl
=======================

First Version of MultiCopterLightControl (MCLC)

MCLC is an open source project to control a WS2811 RGB LED strip with an Arduino. It's main goal is to support different lightning modes for multicopters. Even though it can be used for other lightning projects like MoodLamps etc. with some (small) modifications. At the moment, the project only supports some lightning effects and the control of the program is limited to serial commands. 

I decided to use FastSPI_LED2, even it's in the beta phase, because it's very lightweight compared to the first version of FastSPI_LED. My respect and thanks to the developer of FastSPI_LED, which is an very easy to use piece of software.
https://code.google.com/p/fastspi/

Now I will explain in short, the requirements, the current structure of the Arduino code, followed by the available effects and settings.

Requirements
============
To compile the sketch and upload it to your arduino, you need the actual arduino IDE. Addionally you must have installed the following two libraries into your arduino directory:<br/>
<b>SerialCommand</b>: <a>https://github.com/kroimon/Arduino-SerialCommand</a><br/>
<b>FastSPI_LED2</b>: <a>http://code.google.com/p/fastspi/</a><br/>

And of course you need at least one WS2811 RGB-LED strip and an Arduino ;-)

Code Structure
==============
The code is divided into 5 files.

<b>MultiCopterLightControl.ino</b><br/>
Main file with setup() and loop() method with control structures to switch between lightning modes.
Some very basic methods to modify LEDs are also implemented in this file, like methods to set a given color to a complete arm, clear all LEDs, update the current LEDs, save LED colors to EEPROM and so on. It also brings all the other parts together.

<b>structs.h</b><br/>
Holds all definitions of data structures we use in the project.

<b>config.h</b><br/>
This file can be modified to meet your own needs. See below for the different available settings.

<b>SerialCommands.ino</b><br/>
This file defines/executes all serial commands. See below for a list of available commands.

<b>Sequences.ino</b><br/>
In this file, all lightning effects are implemented. See below for a list of available effects.


Settings
========
<b>NUM_ARMS</b>        = Number of arms of the multicopter<br/>
<b>LEDS_PER_ARM</b>    = LEDs per arm<br/>
<b>LED_PIN</b>         = digital pin on the arduino to connect the data pin of the LED strip<br/>

Uncomment "#define DEBUG" to disable debug output on serial.
Some modes are also defined and you can set an default mode. This mode will be used, if no valid mode selected.


Serial Commands
===============
<b>Color settings</b><br/>
<b>setR N</b>           : value for the red portion of the current color, 0 <= N < 256<br/>
<b>setG N</b>           : value for the green portion of the current color, 0 <= N < 256<br/>
<b>setB N</b>           : value for the blue portion of the current color, 0 <= N < 256<br/>
<b>setArm N</b>         : apply the current color to the given arm, 0 <= N < NUM_ARMS<br/>
<b>save</b>             : save the current color of the LEDs to EEPROM<br/>
<b>clear</b>            : set all LEDs to black (0, 0, 0)<br/>

<b>Effect settings</b><br/>
<b>setV</b>             : set the brightness of some effects<br/>
<b>d N</b>              : set the delay of some effects, 0 < N < 10000<br/>
<b>rev</b>              : reverse some effects<br/>

<b>Mode settings</b><br/>
<b>+</b>                : select next mode<br/>
<b>-</b>                : select previous mode<br/>
<b>m N</b>              : set mode to N, 0 <= N < NUM_MODES<br/>


Effects
=======
Modename, description and available settings of the effect.<br/>
R = REVERSE, D = DELAY, B = BRIGHTNESS, C = COLOR<br/><br/>
<b>MODE_SAVED_COLORS</b>       : show in EEPROM stored colors [C]<br/>
<b>MODE_RUNNING_LED</b>        : synchronous running led on every arm + flash if blinkDelay >= 0 [C, D]<br/>
