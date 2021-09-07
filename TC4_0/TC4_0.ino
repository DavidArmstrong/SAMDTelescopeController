// Arduino SAMD-based Telescope Controller 4.00.00 - September 2020
// See MIT LICENSE.md file and ReadMe.md file for essential information
// Highly tailored to the Sparkfun Redboard Turbo
// DO NOT ATTEMPT TO LOAD THIS ONTO A STANDARD UNO

#include "tcheader.h"

// Display - Choose one for Terminal output - If at all possible, use Serial2
/*
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  #define TCterminal SERIAL_PORT_USBVIRTUAL
#else
  #define TCterminal Serial
#endif
// */
// Serial2 uses PINS D2-TX, D3-RX
#define TCterminal Serial2
ANSI ansi(&TCterminal); // VT100 support

// LCD output - Serial1 is recommended for either PIC LCD or AVR LCD
// Although the AVR LCD hardware also has an IIC option
// Serial1 uses PINS D1-TX, D0-RX
#define TC_LCD Serial1

void setup() {
  tcintro(); // Initialize hardware and do Startup screen
}

void loop() {
  TC_main(); // main processing loop
}
