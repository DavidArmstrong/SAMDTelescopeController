# SAMDTelescopeController
Arduino SAMD51-based Telescope Controller 4.00.00 Diagnostics Sketch

#### By David Armstrong, August 2023
 - See MIT LICENSE.md file

==================================================================

This Sketch is designed to be used as a precursor sketch to TC4_0,
the SAMD51 processor based Telescope Controller 4.00.00 sketch.

Its purpose is to verify that all the needed software libraries
are installed, and that the associated required hardware is present and
working.

The application requires a SAMD51 based processor:<br>
Option 1: Adafruit Metro M4 - based on SAMD51 (Used in my tests)<br>
https://www.adafruit.com/product/3382 <br>
Option 2: Sparkfun Micro-Mod system with SAMD51 processor<br>
https://www.sparkfun.com/products/16791 <br>
https://www.sparkfun.com/products/16885 

Any SAMD51 processor Arduino compatible board should work to some degree.


## Tailoring the Sketch For Your System

At the beginning of the file is a section to allow setting various options and known values.
If any of these are changed, identical changes will need to be made in the TC4_0.ino sketch.

Line 18: LCDpicflag - Set to 'true' for older PIC based LCD, 'false' for newer AVR based LCD. This sketch assumes that Serial1 is used to communicate with the LCD display, if used.

Lines 21-22: Uncomment if associated Encoder is changing counts backwards.

Lines 25-26: Uncomment if associated Motor is moving backwards.

Line 29: Comment out if not using old HMC6352 magnetic compass. The sketch assumes the use of SFE_MMC5983MA by default.

Lines 41-53: Provided for reference on Arduino Pin Usage in this project.


## Preparing The Libraries

All of the libraries referenced in the sketch have to be available
before the sketch will compile successfully. If the sketch compiles 
and loads, you have the needed libraries.  However,
if you get an error, the source code lists a URL to go to
download the missing library.

NOTE: In the library file at IRLibProtocols\IRLibSAMD51.h<br>
You must change lines 18-19 as follows:

```
//#define IR_TCn 3
#define IR_TCn 4
```

Otherwise, the application will NOT compile.

DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.


## Sketch Output

On a Windows computer, you can use the terminal program Tera-Term to access the board via USB.
However, any VT-100 compatible terminal program should work with the sketch.
The default Arduino IDE Serial output window can also be pressed into service here.

You may need to press the Reset button again after programming via the Arduino IDE to see the 
program output from the start of the sketch.

https://learn.sparkfun.com/tutorials/terminal-basics/tera-term-windows


## Tests that are Run

The following is a list of all the tests run by the sketch:

0. Sketch Start means required libraries are installed.  A compile fail will occur if a library is missing.  The code includes the URL to use to fetch each library, as a comment, with each statement that references a library.

1a. Output to VT-100 compatible terminal application via USB using Serial.

1b. Output to 4x20 character LCD using Serial1.

1c. Output to VT-100 compatible terminal application via Xee-Bee using Serial2.

2. Input via either Serial and/or Serial2. Use a Carriage Return/Enter key to terminate the test.

3. Azimuth Encoder check. Counts must increase when going from North to East to South to West.
If not, uncomment line #define SWAP_AZIMUTH_ENCODER_AB<br>
Use a Carriage Return/Enter key to terminate the test.

4. Altitude Encoder check. Counts must increase when going from Horizon to Zenith.
If not, uncomment line #define SWAP_ALTITUDE_ENCODER_AB<br>
Use a Carriage Return/Enter key to terminate the test.

5. Verify presence of I2C EEPROM.

6. u-blox GPS detection.

7. Murata SCL3300 Inclinometer Sensors detection.  Detects either/both rocker and tube tilt sensors.

8. MMC5983MA Magnetic Compass detection.

9. INA219 Voltage/Current sensor detection. 
WARNING: Checking for a non-existant INA219 Voltage/Current monitor will hang the system!
An option to bypass the test it given to avaid this problem.

10. BME280 Temperature, Pressure,Humidity Sensor detection.

11. Azimuth Reference Sensor Detection. The test will indication if the digital line is HIGH or LOW.
Use a Carriage Return/Enter key to terminate the test.

12. Horizon Reference Sensor Detection. The test will indication if the digital line is HIGH or LOW.
Use a Carriage Return/Enter key to terminate the test.

13. Zenith Reference Sensor Detection. The test will indication if the digital line is HIGH or LOW.
Use a Carriage Return/Enter key to terminate the test.

14. Lock/Reinitialize Switch Detection. The test will indication if the digital line is HIGH or LOW.
Use a Carriage Return/Enter key to terminate the test.

15. I2C Serial Controlled Motor Driver Board detection and Motor tests. Sketch output provides instructions on how the motors should move for this test.

16. IR Remote Receiver functionality check.

17. OLED display test. 
WARNING: Checking for a non-existant OLED display will hang the system!
An option to bypass the test it given to avaid this problem.

Once these tests pass to your satisfaction, you are ready to proceed to the TC4_0 sketch, which is the 
actual Telescope Controller application.
