// Arduino SAMD-based Telescope Controller 4.00.00 - September 2022
// See MIT LICENSE.md file and ReadMe.md file for essential information
// Highly tailored to the Sparkfun Redboard Turbo or AdaFruit M4 Metro
// DO NOT ATTEMPT TO LOAD THIS ONTO A STANDARD UNO

#include "tcheader.h"

// Account for Redboard Turbo/SAMD21 Weirdness with SerialUSB
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

// Display - Choose one for Terminal output - If at all possible, use Serial2
#define TCterminal Serial
// Serial2 uses PINS D2-TX, D3-RX
//#define TCterminal Serial2

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
