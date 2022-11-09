# SAMDTelescopeController
Arduino SAMD51-based Telescope Controller 4.00.00

By David Armstrong, October 2022<br>
See MIT LICENSE.md file<br>
=============================<br>

Note: This project is unfinished, although most of the work has been done.  I'm making it public 
so others can build on this framework.

This Sketch implements one possible design for a Telescope Controller, using the
Arduino IDE as a development environment.  It's purpose is to allow me
to use my telescope to look at stuff, instead of spending a lot of time setting it up.  So,
I've done what I can to remove the requirement for alignment stars.  I've reached the point
where attempting to find any object via a finder is next to impossible, due to my physical
limitations.  So, let's just skip that and move on.

(The system can be used just as Digital Setting circles with no motors. But that means manually
moving the 'scope around. Of couese, motors can be added later.)

There is a Draw.io based PNG image file showing the basic buiding blocks that make up the system, 
and how all the parts communicate with each other.
I've also included an Eagle schematic of how to construct various interfaces.

Note: DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.


This application requires a SAMD51 based processor.  Suggestions:<br>
Option 1: Adafruit Metro M4 - based on SAMD51 (Used in my tests)<br>
https://www.adafruit.com/product/3382 <br>
Option 2: Sparkfun Micro-Mod system with SAMD51 processor<br>
https://www.sparkfun.com/products/16791 <br>
https://www.sparkfun.com/products/16885 

Note: Any SAMD51 processor Arduino compatible board should work to some degree.

If the sketch compiles and loads, it has the needed libraries.  However,
if one get an error, the code should at least give a URL for where each
library can be downloaded from.

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
Software Behavior

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


==========================<br>
Here's a list of recommended hardware: (Details are listed later in this doc.)

0. I2C EEPROM (required) - Needed to store persistent data.  Otherwise you have to either
re-enter that data every time the system is powered up, or change the code itself 
to make your data values the default. Not having this will force the program to always
start in demo mode.  (Pins SDA, SCL; IIC address 0x50)

1. Momentary Pushbutton (Leave out at your own risk) - This is used with the reset button
to force a clean re-initialization of the system should things just get into a bad state.
(i.e. Like bad values saved into the EEPROM listed in #1 above.) (Pin 19 = A5)

2. Two Quadrature Optical Encoders (required) - And you need mounting hardware for those 
encoders.  This will allow using the program as Digital Setting Circles on your 
telescope.  (Pins A1,A2 for azimuth and A3,A4 for altitude)

3. Level shifter (required) - Needed to translate the 5 volt signals from the encoders to
3.3 volts for the processor board.  (I haven't seen 3.3 volt quadrature encoders made yet in 
the resolutions needed for telescope positioning.)

4. I2C GPS receiver module (required) - for Time, date, elevation, Latitude and Longitude input.
(Also used for Precession correction and Sun, Moon, Planet coordinate calculations.)
 (IIC address 0x42)


All other hardware that can be added is optional, to some degree.  It depends on what 
you are willing to put up with in the dark, what you consider usable, and how much you 
can spend, in both time and money.

Consider these options:

5. Add in Xbee modules to go wireless.
Screen output can be sent to Serial2 in the code, which will send UART
data to D4(Rx)/D7(Tx) for the SAMD51.  Using Serial2 
may faster than relying on the Serial port, which is the default.  
The option is enabled at the top of file TC4_0.ino.  The other end of the XBee connection should have a VT100 compatible terminal emulator program.  (I use Tera-Term for Windows 10.)

6. Remove requirement for a laptop with:
  A. 4x20 character LCD display (Pin D1 - Serial TX is recommended) OR use I2C bus
  B. IR detector (Pin 11 = D11)
  C. TV/DVD IR remote with backlit kaypad - Instant wireless handbox control

7. Refraction correction:
I2C BME280 Barometric Pressure, Temperature, Humidity Sensor (IIC address 0x77)

8. Initial telescope orientation to North:
(Not needed at all for a permanently mounted telescope.  See #13 below.  It's an aid
for portable telescopes, as it can help you find alignment stars when setting up.)
I2C Magnetic Compass (HMC6352 IIC address 0x21) 
OR (MMC5983MA IIC address 0x30 - recommended)

9. Account for mechanical level of telescope:
(Portable systems can benefit from this option.  Permanently mounted telescopes should be
mounted properly anyway, which makes this unnecessary.  See #13 below.)
Murata SCL3300 SPI inclinometer - for an Alt-Azimuth base for level determination
(pins 22,23,24 on ICSP header, and Chip Select Pin 10 )
Note: Add a second SCL3300, mounted on the telescope tube, for Altitude orientation determination.
( Chip Select Pin 9 )

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
2 Reference Sensors to determine specific reference points in each axis.  (Pins 5,8)
Note: The Horizon/Altitude reference may still be good to have for a portable system.  It can be used with the inclinometer to determine the real horizon point, independent of an alignment star.

Now if you have all that, then zero alignment stars are needed to set up the system
for an observing session.  In fact, it becomes zero alignment, period.  Just turn it on, let it do its setup, and tell it where to go.

======================================<br>
Detailed Hardware Information:
For this system, the hardware listed below is assumed as being used.  Some substitutions
are possible, but the code might have to be changed accordingly.
Do your homework.

Sparkfun Mini Pushbutton switch: https://www.sparkfun.com/products/97
Hint: Consider getting a Protoshield Kit to mount these parts. It even comes with a pushbutton:  https://www.sparkfun.com/products/13820

Sparkfun Buck-Boost Converter (recommended, because one needs both 5 and 11 volt supplies): <br>
https://www.sparkfun.com/products/15208 <br>
Note: I use a second Converter to provide a higher voltage to the Motor Controller Board to power the motors.

In addition, an extra 3.3 volt power source is *required* since there are a maximum number of QWIIC devices being used:<br>
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
https://www.sparkfun.com/products/716 and https://www.sparkfun.com/products/643

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
Software: (Only needed if you want to compile, modify, and/or hork the system.)

You MUST set up the Arduino IDE precisely as listed on the Adafruit guide for the Metro M4:<br>
https://learn.adafruit.com/adafruit-metro-m4-express-featuring-atsamd51

Arduino Libraries:<br>
RTC by Seeed Studio https://github.com/Seeed-Studio/Seeed_Arduino_RTC (SAMD51) <br>
Adafruit INA219 by Adafruit https://github.com/adafruit/Adafruit_INA219 <br>
extEEPROM by Jack Christensen https://github.com/PaoloP74/extEEPROM <br>
https://github.com/cyborg5/IRLib2 (SAMD51)

Sparkfun BME280 by Sparkfun Electronics https://github.com/sparkfun/SparkFun_BME280_Arduino_Library <br>
Sparkfun Micro OLED Breakout by Sparkfun Electronics https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library <br>
Sparkfun Ublox Arduino Library by Sparkfun Electronics https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library <br>
Murata SCL3300 Library https://github.com/DavidArmstrong/Arduino-SCL3300

Other Libraries:<br>
Math: https://www.arduino.cc/en/math/h <br>
Astronomy Routines: https://github.com/DavidArmstrong/SiderealPlanets <br>
Object Library: https://github.com/DavidArmstrong/SiderealObjects <br>
Magnetic Variation: https://github.com/DavidArmstrong/WMM_Tinier <br>
VT100/ANSI terminal support: https://github.com/RobTillaart/ANSI <br>
Least Squares Regression Curve Fitting: https://github.com/Rotario/arduinoCurveFitting

EEPROM hookup guide: https://learn.sparkfun.com/tutorials/reading-and-writing-serial-eeproms <br>
Serial Enabled LCD hookup guide: https://learn.sparkfun.com/tutorials/pic-based-serial-enabled-character-lcd-hookup-guide
--> http://playground.arduino.cc/Code/SerLCD <br>
QWIIC Serial Enabled LCD hookup guide: https://learn.sparkfun.com/tutorials/avr-based-serial-enabled-lcds-hookup-guide <br>
--> The SparkFun SerLCD Arduino library can be downloaded with the Arduino library manager by searching 'SparkFun SerLCD'

====================================<br>
Bibliography:

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

