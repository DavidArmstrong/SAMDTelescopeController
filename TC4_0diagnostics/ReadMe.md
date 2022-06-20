Arduino SAMD-based Telescope Controller 4.00.00 Diagnostics Sketch
By David Armstrong, June 2022
==================================================================

This Sketch is designed to be used as a precursor sketch to TC4_0,
the SAMD processor based Telescope Controller 4.00.00 sketch.

It's purpose is to help make sure all the needed software libraries
are installed, and that the associated required hardware is present and
working.

If the sketch compiles and loads, you have the needed libraries.  However,
if you get an error, the code should at least give you a URL for where each
library can be downloaded from.

Note: DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.

This application requres a SAMD based processor:
Option 1: Sparkfun Redboard Turbo - similar to Arduino Zero - based on SAMD21
https://www.sparkfun.com/products/14812
Option 2: Adafruit Metro M4 - based on SAMD51 (Preferred at this point)
https://www.adafruit.com/product/3382
Note: Any SAMD processor Arduino compatible board should work to some degree.

On a Windows computer, you can use the terminal program Tera-Term to access the board via USB.
You may need to press the Reset button again after programming via the Arduino IDE to see the 
program start from the beginning.
https://learn.sparkfun.com/tutorials/terminal-basics/tera-term-windows

==========================