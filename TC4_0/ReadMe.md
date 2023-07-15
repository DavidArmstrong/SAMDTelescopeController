# SAMDTelescopeController
Arduino SAMD51-based Telescope Controller 4.00.00

#### By David Armstrong, July 2023
 - See MIT LICENSE.md file

=============================<br>

The system may be used just as Digital Setting circles with no motors. But that means manually
moving the 'scope around. Of course, motors can be added later.

Note: DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.


This application requires a SAMD51 based processor.  Suggestions:<br>
Option 1: Adafruit Metro M4 - based on SAMD51 (Used in my tests)<br>
https://www.adafruit.com/product/3382 <br>
Option 2: Sparkfun Micro-Mod system with SAMD51 processor<br>
https://www.sparkfun.com/products/16791 <br>
https://www.sparkfun.com/products/16885 

Note: Any SAMD51 processor Arduino compatible board should work to some degree.

Start by loading the sketch under TC4_0diagnostics, and get that to work first.
That has its' own Readme file.  Its' purpose is to prove that you have all the needed
libraries installed, and that the relevant hardware interfaces exist and can communicate with the
system. If one get a library error, the code gives a URL for where each
library can be downloaded.

Then move on to the main sketch in TC4_0.

For the Metro M4 board with the SAMD51 processor, it is also recommended to select the 'Fastest'
compile speed option on the Arduino IDE.  This is located at: Tools > Optimize > Fastest, and is
available only after selecting the Metro M4 as the target board.  This 
doesn't make the compile time shorter, but it does make the sketch run faster on the processor.
We do this on the SAMD51 because there is not a memory constraint on that system. (We swap 
some memory space for speed of execution increases.)

You may also be required to modify the default values defined at the top of the TC4_0.ino sketch,
based on the hardware setup of your telescope system.

On a Windows computer, one can use the terminal program Tera-Term to access the board via USB.
The Reset button may need to be pressed again after programming via the Arduino IDE to see the 
program start from the beginning.
https://learn.sparkfun.com/tutorials/terminal-basics/tera-term-windows

==========================<br>
## Software Behavior

The primary design focus has been to require as little input as possible from the user to set up. A telescope should be as easy as a toaster to use, and not require the training and agility of an astronaut.

1. Make it wireless.  Battery power is on the telescope itself, not external to it. So no visible wires.
2. No alignment star hunting, if at all possible.
3. No input of parameters, other than what is needed to specify the object to look at.

The system allows three options for user interfaces.  There can be some overlap. These are set in the file TC4_0.ino.

A. Use the USB interface to the processor board.  It's the default, but requires a USB cable. (Serial)
B. Use Xbee hardware to allow a wireless serial connection to the system. (Serial2)
C. Use an LCD display with IR remote for input. (Serial1)

One has to choose either A or B.  Option C can be used by itself, but one of the
other methods has to at least be designated as well.

When the system first powers on, it detects what is connected, and adapts as it can accordingly.

If you know the encoder count range in azimuth and altitude, these values can be hardcoded into
TC4_0.ino, and always used.  Otherwise, the system will attempt to figure out on its' own what 
the ranges are.

To that end, there should be an Azimuth Reference Sensor provided on the telescope mount.  This can be either a switch, light sensor, or magnetic Hall-effect sensor.  Otherwise, the program will ask for manual help in initialization. An inclinometer can also be added to the rocker board to help level the telescope in azimuth, and the system will prompt for that when it is ready.  Note that placement of the Azimuth Reference sensor should be such so that it permits leveling adjustments to be done when the telescope is at that position.

The altitude range needs either sensors for horizon and zenith positions, or an inclinometer. Again, not providing either of these options results in the program prompting for user input to initialize the Altitude axis.

====================================<br>
Software Notes:

You MUST set up the Arduino IDE precisely as listed on the Adafruit guide for the Metro M4:<br>
https://learn.adafruit.com/adafruit-metro-m4-express-featuring-atsamd51

Arduino Libraries:<br>
RTC by Seeed Studio: https://github.com/Seeed-Studio/Seeed_Arduino_RTC (SAMD51) <br>
Adafruit INA219 by Adafruit: https://github.com/adafruit/Adafruit_INA219 <br>
extEEPROM by Jack Christensen: https://github.com/PaoloP74/extEEPROM <br>
IR Library: https://github.com/cyborg5/IRLib2 (SAMD51)

Sparkfun BME280 by Sparkfun Electronics:  https://github.com/sparkfun/SparkFun_BME280_Arduino_Library <br>
Sparkfun Micro OLED Breakout by Sparkfun Electronics:  https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library <br>
Sparkfun Ublox Arduino Library by Sparkfun Electronics:  https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library <br>
Murata SCL3300 Library: https://github.com/DavidArmstrong/Arduino-SCL3300

Other Libraries:<br>
Math: https://www.arduino.cc/en/math/h <br>
Astronomy Routines: https://github.com/DavidArmstrong/SiderealPlanets <br>
Astronomical Object Library: https://github.com/DavidArmstrong/SiderealObjects <br>
Magnetic Variation: https://github.com/DavidArmstrong/WMM_Tinier <br>
VT100/ANSI terminal support: https://github.com/RobTillaart/ANSI <br>
Least Squares Regression Curve Fitting: https://github.com/Rotario/arduinoCurveFitting
