# SAMDTelescopeController

Arduino SAMD51-based Telescope Controller 4.00.00

By David Armstrong, July 2022<br>
See MIT LICENSE file<br>
========================================================================


This repository implements one of many possible design scenarios for a Telescope Controller, 
using the Arduino IDE as a development environment.  It's purpose is to allow me, personally,
to use my telescope to look at stuff, instead of spending a lot of time setting it up.

I'm making this available for others to use as a reference in designing their own systems.  As
such, feel free to clone it and change as your needs dictate.

## Installing the Telescope Controller Application

Download this repository, and place the contained sub-directories as instructed here:

The files in the "Arduino Telescope Interface" directory contain an Eagle schematic of how to 
connect various interfaces.  If you have the Eagle PCB software installed, this directory 
should be copied to be a subdirectory under the EAGLE/projects/ directory.

There is also a drawio based diagram showing at a high level how all the parts communicate 
with each other. This includes both the draw.io diagram source, and a 
'TelescopeController4_0.drawio.png' image file created by draw.io.

Both the TC4_0/ and TC4_0diagnostics/ directories should be copied to the Arduino/ directory, where 
Arduino IDE sketch projects are stored.

## Installing the Required Arduino Libraries

As might be expected in an application this large and complex, there are a number of Arduino libraries
that have to be installed.  Most of these can be installed via the Arduino IDE Library Manager, but
not all.

Here is a list of the libraries that the Arduino IDE Library Manager knows about that must be installed:

'''
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
'''

There are also a few libraries that must be loaded directly from their on-line repositories:

'''
SiderealObjects  https://github.com/DavidArmstrong/SiderealObjects
AMD51 Interrupt Timer library  https://github.com/Dennis-van-Gils/SAMD51_InterruptTimer
HMC6352-Arduino-Library  https://github.com/funflin/HMC6352-Arduino-Library
IRLib2  https://github.com/cyborg5/IRLib2
'''

NOTE: That last library, 'IRLib2', has one file that *MUST* have two lines modified so that
the sketch will compile successfully. In the library file at IRLibProtocols\IRLibSAMD51.h<br>
You must change lines 18-19 as follows:

'''
//#define IR_TCn 3
#define IR_TCn 4
'''

Otherwise, the application will NOT compile.

## Initial Testing

Use the TC4_0diagnostics.ino sketch to test the installed libraries and hardware interfaces
before going to the TC4_0.ino sketch.  This test sketch goes through each piece of hardware,
taking one at a time, to make sure each can be reached and responds as expected when it is
initialized.

Note that while two different Magnetic Compass Libraries are loaded, only one will actually
be used by the application. It depends on which one is physically present.

Note: DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.


This is still very much a work in progress.  I'm putting the code here to make it easier to manage, and to avoid a possible future catastrophe.  (Murphy knows me.)

So, not ready for Prime Time, and not ready for a real Telescope either yet. (But getting close.)

====================================<br>
## History:  (Why is this Version 4?)

Version 1.0 began on that wide computer paper in 1982.  The initial
hardware was built in 1983, using a 6809 CPU at 1 MHz and 2K RAM with
64K of EPROM.  (Only 61K was available though.)
The software really didn't get to the point of being usable
until 1995 or so.  The original code was in assembly and FIG-Forth.
Version 2.0 was built in 1998 with the 68B09, so it could run at 2 MHz, with
the same 2K RAM and a larger 128K EPROM.  GAL chips were used to cut down on
chip count and allow full encoder resolution.

Version 3.0 was planned around 2003, but was never completed.  It
was based on using a Freescale HCS12XE512 MCU, running at 50 MHz.  However, when time
finally became available for software development, it was EOL (2018).  
That, and the fact that my development tools would not now work under 
Windows 10, meant I could either try rewriting my tools, or go to 
'something else'.  Several attempts at updating my Visual C++ IDE ended 
in extreme frustration.  (Thank you Microsoft!)  It was time for 'something else'.

Version 4.0 was decided on when I got my hands on a Sparkfun Redboard Turbo.
I'd always thought the Arduino line of stuff wasn't up to the task, but
this board proved me wrong.  It had real dev tools.  And it had a lot of
libraries that were tested, extensive, and readily available.  Software tool availability
often ends up determining what hardware will be used for a system.

As the software got closer to completion, it became evident that the SAMD21 memory space
was going to reach its' limit.  CPU usage was getting higher as well.  The decision was made
to migrate to the SAMD51, which had twice the flash space, and nearly three times the 
raw speed. Hence the use of the Adafruit Metro M4 board. And I could still use the same
Arduino form factor protoboards I constructed that plug into the headers on those boards.

You will notice some use of 'caps' in the current source.  That's because the
Arduino code is a translation of the original FIG-Forth code, which had a 
proven record in the field, literally.  However, that code was written in a
previous millenium.  Such ancient coding is considered painful to look at by
today's standards.  You are welcome to rewrite it, but I don't have the time.
There was enough rewriting done for the new system.

