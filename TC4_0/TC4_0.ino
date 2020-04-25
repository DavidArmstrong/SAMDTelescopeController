// Arduino SAMD-based Telescope Controller 4.00.00 - May 2019
// See MIT LICENSE.md file and ReadMe.md file for essential information
// Highly tailored to the Sparkfun Redboard Turbo
// DO NOT ATTEMPT TO LOAD THIS ONTO A STANDARD UNO

#include "tcheader.h"

// Display - Choose one for Terminal output - If at all possible, use Serial2
//#define TCterminal SerialUSB
// Serial2 uses PINS D2-TX, D3-RX
#define TCterminal Serial2

// LCD output - Serial1 is recommended for either PIC LCD or AVR LCD
// Although the AVR LCD hardware also has an IIC option
#define TC_LCD Serial1

void setup() {
  tcintro(); // Initialize hardware and do Startup screen
}

void loop() {
  TC_main(); // main processing loop
}
