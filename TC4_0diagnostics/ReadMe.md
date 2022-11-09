# SAMDTelescopeController
Arduino SAMD51-based Telescope Controller 4.00.00 Diagnostics Sketch

By David Armstrong, October 2022<br>
See MIT LICENSE.md file<br>
==================================================================

This Sketch is designed to be used as a precursor sketch to TC4_0,
the SAMD processor based Telescope Controller 4.00.00 sketch.

It's purpose is to help make sure all the needed software libraries
are installed, and that the associated required hardware is present and
working.

NOTE: In the library file at IRLibProtocols\IRLibSAMD51.h<br>
You must change lines 18-19 as follows:

'''
//#define IR_TCn 3
#define IR_TCn 4
'''

Otherwise, the application will NOT compile.

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
program start from the beginning.
https://learn.sparkfun.com/tutorials/terminal-basics/tera-term-windows

At the beginning of the file is a section to allow setting various options and known values.
If any of these are changed, identical changes will need to be made in the TC4_0.ino sketch.
