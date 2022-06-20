/* Arduino SAMD-based Telescope Controller 4.00.00 - September 2022
// See MIT LICENSE.md file and ReadMe.md file for essential information
// Highly tailored to the Sparkfun Redboard Turbo or AdaFruit M4 Metro
// DO NOT ATTEMPT TO LOAD THIS ONTO A STANDARD UNO */


// Encoder count range per axis, if known.  If not, leave defined at 0L
#define AZIMUTH_ENCODER_RANGE    10000L
#define ALTITUDE_ENCODER_RANGE    4000L

// Default Time Zone must be a real number, usually in the range -12.0 to +12.0
// This is in hours offset from GMT
#define DEFAULT_TIME_ZONE    -8.0    /* PST */
// Daylight saving usage - Auto figures out if DST is active or not in US locations only
#define DEFAULT_USE_DST_AUTO_IN_US    true
#define DEFAULT_USE_DST               false

//If not using the Metro M4, comment out the next line
#define __METRO_M4__

// Account for Redboard Turbo/SAMD21 Weirdness with SerialUSB
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

// Terminal Display Output - Pick one - If at all possible, use Serial2
#define TCterminal Serial
// Serial2 uses PINS D2-TX, D3-RX for SAMD21; PINS D7-TX, D4-RX for SAMD51
// This can be set to go to an Xbee module for wireless interaction
//#define TCterminal Serial2

/* LCD output - Serial1 is recommended for either PIC LCD or AVR LCD
// Although the AVR LCD hardware also has an IIC option
// Serial1 uses PINS D1-TX, D0-RX */
#define TC_LCD Serial1

#include "tcheader.h"

void setup() {
  RRAAZ = RDECAL = 0L;
  if (AZIMUTH_ENCODER_RANGE != 0L) RRAAZ = AZIMUTH_ENCODER_RANGE;
  if (ALTITUDE_ENCODER_RANGE != 0L) RDECAL = ALTITUDE_ENCODER_RANGE;
  TZONE = eecharbuf.strunion.DTZONE = DEFAULT_TIME_ZONE;
  DSTFLAG = eecharbuf.strunion.DSTFLAG = DEFAULT_USE_DST;
  DSTAUTOFLAG = DEFAULT_USE_DST_AUTO_IN_US;

  tcintro(); // Initialize hardware and do Startup screen
}

void loop() {
  TC_main(); // main processing loop
}
