Arduino SAMD-based Telescope Controller 4.00.00
By B David Armstrong, 2020
=============================
If you are here, you are at least interested.  Quickest way for you to see a demo:

0. DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.

1. Get this hardware, Required for anything and everything:
Sparkfun Redboard Turbo - similar to Arduino Zero - based on SAMD21
https://www.sparkfun.com/products/14812
Note: Any SAMD Arduino compatible board should work to some degree.

2. Connect a USB cable to the Turbo board from a Windows 10 system.

3. Double click the reset button on the Turbo.  This gets into bootloader mode. A folder
 should open up on the Windows screen.

4. Drag and drop the 'current.uf2' file from this project into the USB folder that 
appears.  Lights will blink on the board as it installs the code.

5. Use the terminal program Tera-Term to access the Turbo via USB.  You may need to
 press the Reset button again to see the program start from the beginning.
https://learn.sparkfun.com/tutorials/terminal-basics/tera-term-windows

At this point, you can start playing with the software with just the Turbo board.  
This let's you see how the software works, at least from a Terminal program.  

==========================
Once you've checked out the program, you can then decide if you want to proceed with
adding hardware to make it function with your telescope.  Here's the recommended order 
to add stuff: (Gory details are listed later in this doc.)

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
3.3 volts for the Turbo board.  (I haven't seen 3.3 volt quadrature encoders made yet in 
the resolutions needed for telescope positioning.)


All other hardware that can be added is optional, to some degree.  It depends on what 
you are willing to put up with in the dark, what you consider usable, and how much you 
can spend on feeding the beast, in both time and money.

Consider these options:

4. Cut the cord to your laptop by adding in Xbee modules to go wireless.
Screen output can be sent to Serial2 in the code, which will send UART
data to D2 (Tx), and D3 (Rx).  Using Serial2 will be faster than relying
on the SerialUSB port, which is the default.  The option is enabled at the
top of file TC4_0.ino.

5. Unleash the system from Tera-Term on a laptop with:
4x20 character LCD display (Pin D2 - SERCOM Serial TX) OR use I2C bus
IR detector (Pin 18 = A4)
TV/DVD IR remote with backlit kaypad - Instant wireless handbox control

6. Time, date, elevation, Latitude and Longitude input:
(Also used for Precession correction and Sun, Moon, Planet coordinate calculations.)
I2C GPS receiver module (IIC address 0x42)

7. Refraction correction:
I2C BME280 Barometric Pressure, Temperature, Humidity Sensor (IIC address 0x77)

8. Initial telescope orientation to North:
(Not needed at all for a permanently mounted telescope.  See #12 below.  It's an aid
for portable telescopes, as it can help you find alignment stars when setting up.)
I2C Magnetic Compass (HMC6352 IIC address 0x21) OR (HMC6343 IIC address 0x19)

9. Account for mechanical level of telescope:
(Portable systems can benefit from this option.  Permanently mounted telescopes should be
mounted properly anyway, which makes this unnecessary.  See #12 below.)
Murata SPI inclinometer - for an Alt-Azimuth base for level determination
(pins 22,23,24 on ICSP header, and Chip Select Pin 10 )

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
2 Reference Sensors to determine specific reference points in each axis.  (Pins 4,5)
Note: The Horizon/Altitude reference is still good to have for a portable system.  It can be used with the inclinometer to determine the real horizon point, independent of an alignment star.

Now if you have ALL that, then zero alignment stars are needed to set up the system
for an observing session.  In fact, it becomes zero alignment, period.  Just turn it on, let it do its setup, and tell it where to go.

======================================
Gory Detailed Hardware Information:
For this system, the hardware listed below is assumed as being used.  Some substitutions
are possible, but the code might have to be changed accordingly.
Do your homework.

Sparkfun Mini Pushbutton switch: https://www.sparkfun.com/products/97
Hint: Consider getting a Protoshield Kit to mount these parts. It even comes with a pushbutton:  https://www.sparkfun.com/products/13820

Sparkfun Buck-Boost Converter (recommended, because one needs both 5 and 3.3 volt supplies): https://www.sparkfun.com/products/15208
Note: I use a second Converter to provide a higher voltage to the Motor Controller Board to power the motors.

Sparkfun bi-directional level shifter. (Used for input from two quadrature encoders.): https://www.sparkfun.com/products/12009

2 Quadrature Encoders: (Recommend US digital S2 series.) https://www.usdigital.com/products/encoders/incremental/rotary/shaft/S2
For telescope mounting hardware for encoders, try: http://skyeng.com/
They have encoders and connecting cables too.  (or 'roll your own')
And to connect the encoders cable to the Turbo, use an RJ45 jack: https://www.sparkfun.com/products/716 and https://www.sparkfun.com/products/643

Various I2C (QWIIC) devices:  https://www.sparkfun.com/qwiic
GPS: https://www.sparkfun.com/products/15210 or for higher accuracy in RTK mode: https://www.sparkfun.com/products/15005 or https://www.sparkfun.com/products/15136
EEPROM: https://www.sparkfun.com/products/525 or https://www.sparkfun.com/products/14764
BME280 Barometric pressure/temperature sensor: https://www.sparkfun.com/products/13676 or https://www.sparkfun.com/products/14348 or the latest at https://www.sparkfun.com/products/15440 (QWIIC version - recommended)
Magnetic Compass: (Old) https://www.sparkfun.com/products/retired/7915 or you might instead try the newer version at: https://www.sparkfun.com/products/12916 (more expensive, more accurate)
https://www.digikey.com/product-detail/en/sparkfun-electronics/SEN-12916/1568-1557-ND/5764505
MicroOLED display: https://www.sparkfun.com/products/14532
Battery Current/Voltage Measurement: https://www.sparkfun.com/products/15176
Motor Driver: https://www.sparkfun.com/products/13911 (older version)
or https://www.sparkfun.com/products/15451 (newer version)

For the optional 4x20 LCD display, I use the older PIC based one:
https://www.sparkfun.com/products/9568
Or One could substitute the newer one which can be used via the UART or I2C interfaces:
4x20 SerLCD/OpenLCD display: https://www.sparkfun.com/products/14074

IR detector - for use with an IR remote with backlit keypad: https://www.sparkfun.com/products/10266 or https://www.sparkfun.com/products/retired/8554 ( I bought this years ago because I knew I would use it for this project.)
You choose the IR remote yourself.  I've seen them on Amazon, and Wally-World has it too.

And just for kicks and giggles, because I despise having to find alignment stars:
SPI Murata SCL3300-D01-1 inclinometer: https://www.mouser.com/ProductDetail/Murata-Electronics/SCL3300-D01-PCB?qs=vLWxofP3U2yrcJsOYvcWaQ%3D%3D
https://www.digikey.com/product-detail/en/murata-electronics-north-america/SCL3300-D01-10/490-18218-1-ND/9950619
Note: Yeah, it's a pricey little item.  But without it, one is left with having to manually level the telescope every single time it's set up.  I've done that enough now.

====================================
Software: (Only needed if you want to compile, modify, and/or hork the system.)

You MUST set up the Arduino IDE precisely as listed on the Sparkfun Turbo Hookup Guide: https://learn.sparkfun.com/tutorials/redboard-turbo-hookup-guide
(This includes versions listed for the SAMD libraries, etc.)

Arduino Libraries:
RTCZero by Arduino https://www.arduino.cc/en/Reference/RTC
Adafruit INA219 by Adafruit https://github.com/adafruit/Adafruit_INA219
Encoder by Paul Stoffregen https://www.pjrc.com/teensy/td_libs_Encoder.html
extEEPROM by Jack Christensen https://github.com/PaoloP74/extEEPROM
IRremote by shiffiff https://github.com/z3t0/Arduino-IRremote
Serial Controlled Motor Driver by Sparkfun Electronics https://github.com/sparkfun/Serial_Controlled_Motor_Driver
Sparkfun BME280 by Sparkfun Electronics https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
Sparkfun Micro OLED Breakout by Sparkfun Electronics https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library
Sparkfun Ublox Arduino Library by Sparkfun Electronics http://librarymanager/All#SparkFun_Ublox_GPS

Other Libraries:
Old Magnetic Compass: https://github.com/funflin/HMC6352-Arduino-Library
New Magnetic Compass: https://github.com/sparkfun/SparkFun_HMC6343_Arduino_Library
Math: https://www.arduino.cc/en/math/h

EEPROM hookup guide: https://learn.sparkfun.com/tutorials/reading-and-writing-serial-eeproms
Serial Enabled LCD hookup guide: https://learn.sparkfun.com/tutorials/pic-based-serial-enabled-character-lcd-hookup-guide
--> http://playground.arduino.cc/Code/SerLCD
QWIIC Serial Enabled LCD hookup guide: https://learn.sparkfun.com/tutorials/avr-based-serial-enabled-lcds-hookup-guide
--> The SparkFun SerLCD Arduino library can be downloaded with the Arduino library manager by searching 'SparkFun SerLCD'

====================================
Bibliography:  (Just to prove that I did MY homework.)

1. Van Flandern, T. C. and K. F. Pulkkinen. "Low-Precision Formulae For
Planetary Positions."   The Astrophysical Journal Supplement Series,
Vol. 41, November 1979, P. 391-411

2. The Astronomical Almanac. Washington D. C.:
U. S. Government Printing Office. (There is an edition for each year.)

3. Duffet-Smith, Peter. Practical Astronomy With Your Calculator.
New York: Cambridge University Press, 1979.

4. Duffet-Smith, Peter. Astronomy With Your Personal Computer. New York:
Cambridge University Press, 1985.

5.  Trueblood, Mark and Russell Genet. Microcomputer Control of
Telescopes. Richmond, Virginia: Willmann-Bell, Inc., 1985

6. Hirshfeld, Alan and Roger Sinnott. Sky Catalogue 2000.0 Volume-1:
    Stars to 8.0 Cambridge, Massachusetts: Sky Publishing Corporation, 1985

7. Hirshfeld, Alan and Roger Sinnott. Sky Catalogue 2000.0 Volume-2:
    Double Stars, Variable Stars and Nonstellar Objects Cambridge, 
    Massachusetts:  Sky Publishing Corporation, 1985

8. Taki, Toshimi. "A New Concept in Computer-Aided Telescopes."
Sky and Telescope, February 1989, P. 194-196

9. Jean Meeus, Astronomical Formulae for Calculators, 1985, Willmann-Bell, Inc.

10. Jean Meeus, Astronomical Algorithms, 1991, Willmann-Bell, Inc.

11. Smart, W. M. Text-Book on Spherical Astronomy, 6th ed.
Cambridge, England: Cambridge University Press, 1960. Zwillinger,
D. (Ed.)

12. Taki, Toshimi. "Matrix Method for Coordinates Transformation (950KB) in pdf file". http://www.takitoshimi.shop/matrix/matrix_method_rev_e.pdf

13. Celestial Objects Online Database: http://www.astronexus.com/hyg and https://github.com/astronexus/HYG-Database

====================================
History:  (Why is this Version 4?)

Version 1.0 began on that wide computer paper in 1982.  The initial
hardware was built in 1983, using a 6809 CPU at 1 MHz and 2K RAM with
64K of EPROM.  (Only 61K was available though.)
The software really didn't get to the point of being usable
until 1995 or so.  The original code was in assembly and FIG-Forth.
Version 2.0 was built in 1998 with the 68B09, but at 2 MHz, with
the same 2K RAM and 128K EPROM.  GAL chips were used to cut down on
chip count and allow full encoder resolution.
Version 3.0 was planned around 2003, but was never completed.  It
was based on using a Freescale HCS12XD512 MCU.  However, when time
finally became available for software development, it was EOL (2018).  
That, and the fact that my development tools would not now work under 
Windows 10, meant I could either try rewriting my tools, or go to 
'something else'.  Several attempts at updating my Visual C++ IDE ended 
in extreme frustration.  (Thank you Microsoft!)  It was time for 'something else'.
Version 4.0 was decided on when I got my hands on a Sparkfun Turbo Redboard.
I'd always thought the Arduino line of stuff wasn't up to the task, but
this board proved me wrong.  It had real dev tools.  And it had a lot of
libraries that were tested and extensive.  Software tool availability
often ends up determining what hardware will be used for a system.

You will notice a lot of 'caps' in the current source.  That's because the
Arduino code is a translation of the original FIG-Forth code, which had a 
proven record in the field, literally.  However, that code was written in a
previous millenium.  Such ancient coding is considered painful to look at by
today's standards.  You are welcome to rewrite it, but I don't have the time.
There was enough rewriting done for the new system.