# SAMDTelescopeController

## Arduino SAMD51-based Telescope Controller 4.00.00 Documentation

### Docs directory Readme File

#### By David Armstrong, December 2025
 - See MIT LICENSE file

========================================================================

This repository implements a SAMD51 processor based Telescope Controller, 
using the Arduino IDE as a development environment.


## Installing the Telescope Controller Application

Download this repository, and place the contained sub-directories as instructed here:

The files in the "Arduino Telescope Interface" directory contain an Eagle schematic of how to 
connect various interfaces.  If you have the Eagle PCB software installed, this directory 
should be copied to be a subdirectory under the EAGLE/projects/ directory.

There is also a draw.io based diagram showing at a high level how all the parts communicate 
with each other. This includes both the draw.io diagram source, and a 
'TelescopeController4_0.drawio.png' image file created by draw.io.

The TC4_0/, TC4_0diagnostics/, and TC4_0powerMonitor/ directories are three separate Arduino sketches, and should each be copied to the Arduino/ directory on your computer, where Arduino IDE sketch projects are stored.


## Installing the Required Arduino Libraries

As might be expected in an application this large and complex, there are a number of Arduino libraries
that have to be installed.  Most of these can be installed via the Arduino IDE Library Manager, but
not all.

Here is a list of the libraries that the Arduino IDE Library Manager knows about that must be installed:


ANSI  https://github.com/RobTillaart/ANSI <br>
CurveFitting  https://github.com/Rotario/arduinoCurveFitting <br>
Encoder  https://github.com/PaulStoffregen/Encoder <br>
extEEPROM  https://github.com/PaoloP74/extEEPROM <br>
SCL3300  https://github.com/DavidArmstrong/SCL3300 <br>
Seeed Arduino RTC  https://github.com/Seeed-Studio/Seeed_Arduino_RTC <br>
Serial Controlled Motor Driver  https://github.com/sparkfun/SparkFun_Serial_Controlled_Motor_Driver_Arduino_Library <br>
SiderealPlanets  https://github.com/DavidArmstrong/SiderealPlanets <br>
SiderealObjects  https://github.com/DavidArmstrong/SiderealObjects <br>
Sparkfun BME280  https://github.com/sparkfun/SparkFun_BME280_Arduino_Library <br>
Sparkfun MMC5983MA Magnetometer Arduino Library  https://github.com/sparkfun/SparkFun_MMC5983MA_Magnetometer_Arduino_Library <br>
Sparkfun SerLCD Arduino Library  https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library <br>
Sparkfun u-blox GNSS Arduino Library  https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library <br>
WMM_Tinier  https://github.com/DavidArmstrong/WMM_Tinier


There are also a few libraries that must be loaded directly from their on-line repositories:

AMD51 Interrupt Timer library  https://github.com/Dennis-van-Gils/SAMD51_InterruptTimer <br>
HMC6352-Arduino-Library  https://github.com/funflin/HMC6352-Arduino-Library <br>
Sparkfun HMC6343 Arduino Library  https://github.com/sparkfun/SparkFun_HMC6343_Arduino_Library <br>
IRLib2  https://github.com/cyborg5/IRLib2


NOTE: That last library, 'IRLib2', has one file that MUST have two lines modified so that
the sketch will compile successfully. In the library file at IRLibProtocols\IRLibSAMD51.h<br>
You must change lines 18-19 as follows:

```
//#define IR_TCn 3
#define IR_TCn 4
```
Also, in this same file, you must fix line 31 to add in the missing quote character at the end:

```
    #error "Unsupported output pin on Adafruit Metro M4"
```

Otherwise, if this is not done the application will NOT compile.

Finally, the optional TC4_0powerMonitor, if used, will require two additional libraries to be installed:

Adafruit INA219  https://github.com/adafruit/Adafruit_INA219 <br>
Sparkfun Micro OLED Breakout  https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library <br>


## Initial Testing

First use the TC4_0diagnostics.ino sketch to test the installed libraries and hardware interfaces
before going to the TC4_0.ino sketch.  The test sketch goes through each piece of hardware, 
one at a time, to make sure each can be reached and responds as expected when it is
initialized.  Please refer to the Readme file that is with that sketch for specific 
instructions on how to set up and run the application.

Note that while two or three different Magnetic Compass Libraries may loaded, only one will actually
be used by the application. It depends on which one is physically present, and on configuring
the software options.

After the TC4_0diagnostics.ino sketch tests pass, you can move on to compiling and using
the TC4_0.ino application, located in the TC4_0 folder.

Finally, as an optional utility, there is the TC4_0powerMonitor.ino sketch.  This allows real-time
monitoring of a 12 volt gel-cell battery, should that be used to power the system.  It includes
the capability of providing an estimate of how much power is available to use. The sketch is hosted
on a separate SAMD21 MCU, so it runs independently of the main controller.

Note: DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.

==========================<br>
## List of Recommended Hardware
(Details are listed later in this doc.)

0. I2C EEPROM (required) - Needed to store persistent data.  Otherwise you have to either
re-enter that data every time the system is powered up, or change the code itself 
to make your data values the default. Not having this will force the program to always
start in demo mode.  (Pins SDA, SCL; IIC address 0x50)

1. Momentary Pushbutton (Leave out at your own risk) - This is used with the reset button
to force a clean re-initialization of the system should things just get into a bad state.
(i.e. Like bad values saved into the EEPROM listed in #1 above.) (Pin 19 = A5)

2. A Quadrature Optical Encoder (required) - You will need mounting hardware for this 
encoder on the Azimuth axis.  This will allow using the program, at a minimum, as Digital Setting Circles on your 
telescope.  (Pins A1,A2 for azimuth)

2a. Murata SCL3300 SPI inclinometer - You will need an inclinometer, mounted on the telescope tube, for Altitude orientation determination.
( pins 22,23,24 on ICSP header, and Chip Select Pin 9 )

3. Level shifter (required) - Needed to translate the 5 volt signals from the Azimuth encoder to
3.3 volts for the processor board.  (I haven't seen 3.3 volt quadrature encoders made yet in 
the resolutions needed for telescope positioning.)

4. I2C GPS receiver module (required) - for Time, date, elevation, Latitude and Longitude input.
(Also used for Precession correction and Sun, Moon, Planet coordinate calculations.)
 (IIC address 0x42)


All other hardware that can be added is optional, to some degree.  It depends on what 
you are willing to put up with in the dark, what you consider usable, and how much you 
can spend, in both time and money.

Consider these options:

5. Xbee modules to go wireless.
Screen output can be sent to Serial2 in the code, which will send UART
data to D4(Rx)/D7(Tx) for the SAMD51.  Using Serial2 
may be faster than relying on the Serial port, which is the default.  
The option is enabled at the top of file TC4_0.ino.  The other end of the XBee connection should have a VT100 compatible terminal emulator program.  (I use Tera-Term for Windows 11.)

6. Remove requirement for a laptop with:<br>
  A. 4x20 character LCD display (Pin D1 - Serial TX is recommended) OR use I2C bus<br>
  B. IR detector (Pin 11 = D11)<br>
  C. TV/DVD IR remote with backlit kaypad - Instant wireless handbox control

7. Refraction correction:
I2C BME280 Barometric Pressure, Temperature, Humidity Sensor (IIC address 0x77)

8. Initial telescope orientation to North:
(Not needed at all for a permanently mounted telescope.  See #13 below.  It's an aid
for portable telescopes, as it can help you find alignment stars when setting up.)
I2C Magnetic Compass (HMC6352 IIC address 0x21) 
OR (MMC5983MA IIC address 0x30)
OR (HMC6343 IIC address 0x32, 0x33 - recommended)

9. Account for mechanical level of telescope:
(Portable systems can benefit from this option.  Permanently mounted telescopes should be
mounted properly anyway, which makes this unnecessary.  See #13 below.)
Murata SCL3300 SPI inclinometer - for an Alt-Azimuth base for level determination
(pins 22,23,24 on ICSP header, and Chip Select Pin 10 )<br>

10. Separate Status display, just because I think it's cool:
I2C OLED display (IIC address 0x3c)

11. Battery supply Voltage/Current Monitoring:
(Because you really want to know how long you have before the battery dies.)
I2C Voltage/Current sensor (IIC address 0x40)

12. Motors to make your telescope a "GoTo" system:
Recommend two DC motors, rather than stepper motors.  But it's your choice.
Motor electronics - SparkFun Qwiic Motor Driver (I2C address 0x5D)
The How-To for this is your responsibility, which is why I list it as nearly dead last.  
You choose the drive system.

13. Permanently mounted telescopes can automate initialization further by installing:
Reference Sensor to determine specific Azimuth reference point in that axis.  (Pin 8)

Now if you have all that, then zero alignment stars are needed to set up the system
for an observing session.  In fact, it becomes zero alignment, mostly.  Just turn it on, let it do its setup, and tell it where to go. (I state 'mostly' because the controller still uses a tweaking star/planet on initialization to account for errors. But the system will point to that object for you.)

======================================<br>
## Detailed Hardware Information

For this system, the hardware listed below is assumed as being used.  Some substitutions
are possible, but the code might have to be changed accordingly. You will have to do your homework.

Sparkfun Mini Pushbutton switch: https://www.sparkfun.com/products/97 <br>
Hint: Consider getting a Protoshield Kit to mount these parts. It even comes with a pushbutton:  https://www.sparkfun.com/products/13820

Sparkfun Buck-Boost Converter (recommended, because one needs both 5 and 11 volt supplies): <br>
https://www.sparkfun.com/products/15208 <br>
Note: I use an additional Converter to provide a higher voltage to the Motor Controller Board to power the motors.

In addition, an extra 3.3 volt power source is required since there are a maximum number of QWIIC devices being used:<br>
(If that extra 3.3 volt supply isn't installed, the sketch is guaranteed to hang.  Or the I2C interface on the SAMD processor will burn out.  Both effects are known from experience.)<br>
https://www.sparkfun.com/products/18356 <br>
https://www.sparkfun.com/products/14495 <br>
And while you're at it, be absolutely sure that the I2C clock and data lines both have pull-up
resistors (4.7k nominal) tied to +3.3 volts.

Sparkfun bi-directional level shifter. (Used for input from two quadrature encoders.):<br>
https://www.sparkfun.com/products/12009

2 Quadrature Encoders: (Recommend US digital S2 series.)<br>
https://www.usdigital.com/products/encoders/incremental/rotary/shaft/S2 <br>
For telescope mounting hardware for encoders, try: http://skyeng.com/ <br>
They have encoders and connecting cables too.  (or 'roll your own')
And to connect the encoders cable to the processor board, use an RJ45 jack:<br>
https://www.sparkfun.com/products/716 and https://www.sparkfun.com/products/643<br>
I may also try one of these encoder kits: https://www.astrodevices.com/shop/index.php?product/page/170/Explore+Scientific+Truss+Dobsonian+Encoder+Kit

Various I2C (QWIIC) devices:  https://www.sparkfun.com/qwiic

GPS: https://www.sparkfun.com/products/15210 or for higher accuracy in RTK mode: https://www.sparkfun.com/products/15005 or https://www.sparkfun.com/products/15136

EEPROM: https://www.sparkfun.com/products/525 or https://www.sparkfun.com/products/14764

BME280 Barometric pressure/temperature sensor: https://www.sparkfun.com/products/13676 or https://www.sparkfun.com/products/14348 or the latest at https://www.sparkfun.com/products/15440 (QWIIC version - recommended)

Magnetic Compass: (Old) https://www.sparkfun.com/products/retired/7915 or you might instead try the newer version at: https://www.sparkfun.com/products/12916 (more expensive, more accurate)<br>
Newest: https://www.sparkfun.com/products/19034 (recommended)

MicroOLED display: https://www.sparkfun.com/products/14532

Battery Current/Voltage Measurement: https://www.sparkfun.com/products/15176

Motor Driver: https://www.sparkfun.com/products/13911 (older version)<br>
or https://www.sparkfun.com/products/15451 (newer version)

For the optional 4x20 LCD display, I use the older PIC based one:<br>
https://www.sparkfun.com/products/9568 <br>
Or one could substitute one of the newer ones which can be used via the UART or I2C interfaces:<br>
4x20 SerLCD/OpenLCD display: https://www.sparkfun.com/products/14074 <br>
QWIIC version: https://www.sparkfun.com/products/16398

IR detector - for use with an IR remote with backlit keypad: https://www.sparkfun.com/products/10266 or https://www.sparkfun.com/products/retired/8554 ( I bought this years ago because I knew I would use it for this project.)<br>
You choose the IR remote yourself.  I've seen them on Amazon, and Wally-World has it too.

And to avoid having to find alignment stars:<br>
SPI Murata SCL3300-D01-1 inclinometer: https://www.mouser.com/ProductDetail/Murata-Electronics/SCL3300-D01-PCB?qs=vLWxofP3U2yrcJsOYvcWaQ%3D%3D <br>
https://www.digikey.com/product-detail/en/murata-electronics-north-america/SCL3300-D01-10/490-18218-1-ND/9950619 <br>
Note: Without it, one is left with having to manually determine level the of the telescope every single time it's set up.


====================================<br>
## Supporting Documentation

EEPROM hookup guide: https://learn.sparkfun.com/tutorials/reading-and-writing-serial-eeproms <br>
Serial Enabled LCD hookup guide: https://learn.sparkfun.com/tutorials/pic-based-serial-enabled-character-lcd-hookup-guide
--> http://playground.arduino.cc/Code/SerLCD <br>
QWIIC Serial Enabled LCD hookup guide: https://learn.sparkfun.com/tutorials/avr-based-serial-enabled-lcds-hookup-guide <br>
--> The SparkFun SerLCD Arduino library can be downloaded with the Arduino library manager by searching 'SparkFun SerLCD'


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
was going to reach its limit.  CPU usage was getting higher as well.  The decision was made
to migrate to the SAMD51, which had twice the flash space, and nearly three times the 
raw speed. Hence the use of the Adafruit Metro M4 board. And I could still use the same
Arduino form factor protoboards I constructed that plug into the headers on those boards.

You will notice some use of 'caps' in the current source.  That's because the
Arduino code is a translation of the original FIG-Forth code, which had a 
proven record in the field, literally.  However, that Forth code was written in a
previous millenium.  Such ancient coding is considered painful to look at by
today's standards.  You are welcome to rewrite it, but I don't have the time.
There was enough rewriting done for the new system.

====================================<br>
## Bibliography:

1. Van Flandern, T. C. and K. F. Pulkkinen. "Low-Precision Formulae For
Planetary Positions."   The Astrophysical Journal Supplement Series,
Vol. 41, November 1979, P. 391-411
http://adsabs.harvard.edu/full/1979ApJS...41..391V

2. The Astronomical Almanac. Washington D. C.:
U. S. Government Printing Office. (There is an edition for each year.)

3. Duffet-Smith, Peter. Practical Astronomy With Your Calculator.
New York: Cambridge University Press, 1979.

4. Duffet-Smith, Peter. Astronomy With Your Personal Computer. New York:
Cambridge University Press, 1985.
https://oiipdf.com/astronomy-with-your-personal-computer

5.  Trueblood, Mark and Russell Genet. Microcomputer Control of
Telescopes. Richmond, Virginia: Willmann-Bell, Inc., 1985

6. Hirshfeld, Alan and Roger Sinnott. Sky Catalogue 2000.0 Volume-1:
    Stars to 8.0 Cambridge, Massachusetts: Sky Publishing Corporation, 1985

7. Hirshfeld, Alan and Roger Sinnott. Sky Catalogue 2000.0 Volume-2:
    Double Stars, Variable Stars and Nonstellar Objects Cambridge, 
    Massachusetts:  Sky Publishing Corporation, 1985
	https://heasarc.gsfc.nasa.gov/W3Browse/all/ngc2000.html

8. Taki, Toshimi. "A New Concept in Computer-Aided Telescopes."
Sky and Telescope, February 1989, P. 194-196
http://takitoshimi.starfree.jp/aim/aim.htm

9. Jean Meeus, Astronomical Formulae for Calculators, 1985, Willmann-Bell, Inc.

10. Jean Meeus, Astronomical Algorithms, 1991, Willmann-Bell, Inc.

11. Smart, W. M. Text-Book on Spherical Astronomy, 6th ed.
Cambridge, England: Cambridge University Press, 1960. Zwillinger,
D. (Ed.)

12. Taki, Toshimi. "Matrix Method for Coordinates Transformation (950KB) in pdf file". http://www.takitoshimi.shop/matrix/matrix_method_rev_e.pdf

13. Celestial Objects Online Database: http://www.astronexus.com/hyg and https://github.com/astronexus/HYG-Database

14. List of Bright Stars. https://en.wikipedia.org/wiki/List_of_brightest_stars
