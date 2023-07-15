# SAMDTelescopeController
Arduino SAMD51-based Telescope Controller 4.00.00 Diagnostics Sketch

#### By David Armstrong, July 2023
 - See MIT LICENSE.md file
 
==================================================================

This Sketch is designed to be used as a precursor sketch to TC4_0, the SAMD51 processor based Telescope Controller 4.00.00 sketch.

The purpose is to help make sure all the needed software libraries are installed, and that the associated required hardware is present and working.

If the sketch compiles and loads, you have the needed libraries.  However,
if you get an error, the code should at least give you a URL to go to
download the missing library.

Note: DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.

This application requires a SAMD51 based processor:<br>
Option 1: Adafruit Metro M4 - based on SAMD51 (Used in my tests)<br>
https://www.adafruit.com/product/3382 <br>
Option 2: Sparkfun Micro-Mod system with SAMD51 processor<br>
https://www.sparkfun.com/products/16791 <br>
https://www.sparkfun.com/products/16885 

Note: Any SAMD51 processor Arduino compatible board should work to some degree.

On a Windows computer, you can use the terminal program Tera-Term to access the board via USB.
You may need to press the Reset button again after programming via the Arduino IDE to see the 
program start from the beginning.<br>
https://learn.sparkfun.com/tutorials/terminal-basics/tera-term-windows

At the beginning of the file is a section to allow setting various options and known values.
If any of these are changed, identical changes will need to be made in the TC4_0.ino sketch.

## Installing the Required Arduino Libraries

As might be expected in an application this large and complex, there are a number of Arduino libraries
that have to be installed.  Most of these can be installed via the Arduino IDE Library Manager, but
not all.

Here is a list of the libraries that the Arduino IDE Library Manager knows about that must be installed:

```
Adafruit INA219  https://github.com/adafruit/Adafruit_INA219
ANSI  https://github.com/RobTillaart/ANSI
CurveFitting  https://github.com/Rotario/arduinoCurveFitting
Encoder  https://github.com/PaulStoffregen/Encoder
extEEPROM  https://github.com/PaoloP74/extEEPROM
SCL3300  https://github.com/DavidArmstrong/SCL3300
Seeed Arduino RTC  https://github.com/Seeed-Studio/Seeed_Arduino_RTC
Serial Controlled Motor Driver  https://github.com/sparkfun/SparkFun_Serial_Controlled_Motor_Driver_Arduino_Library
SiderealPlanets  https://github.com/DavidArmstrong/SiderealPlanets
Sparkfun BME280  https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
Sparkfun Micro OLED Breakout  https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library
Sparkfun MMC5983MA Magnetometer Arduino Library  https://github.com/sparkfun/SparkFun_MMC5983MA_Magnetometer_Arduino_Library
Sparkfun SerLCD Arduino Library  https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library
Sparkfun u-blox GNSS Arduino Library  https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library
WMM_Tinier  https://github.com/DavidArmstrong/WMM_Tinier
```

There are also a few libraries that must be loaded directly from their on-line repositories:

```
SiderealObjects  https://github.com/DavidArmstrong/SiderealObjects
AMD51 Interrupt Timer library  https://github.com/Dennis-van-Gils/SAMD51_InterruptTimer
HMC6352-Arduino-Library  https://github.com/funflin/HMC6352-Arduino-Library
IRLib2  https://github.com/cyborg5/IRLib2
```

NOTE: That last library, 'IRLib2', has one file that *MUST* have two lines modified so that
the sketch will compile successfully. In the library file at IRLibProtocols\IRLibSAMD51.h<br>
You must change lines 18-19 as follows:

```
//#define IR_TCn 3
#define IR_TCn 4
```

Otherwise, the application will NOT compile.
