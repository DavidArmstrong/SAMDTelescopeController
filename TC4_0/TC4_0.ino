/* Arduino SAMD51-based Telescope Controller 4.00.00 - December 2025
// See MIT LICENSE.md and ReadMe.md files for essential information
// Highly tailored to the AdaFruit M4 Metro
// The Sparkfun Micro-Mod system with SAMD51 processor can probably also be made to work
// DO NOT ATTEMPT TO LOAD THIS ONTO A STANDARD UNO */

/* To Be Done
1. Fix target computations
2. Tweaking Star - allow 3 choices
3. Better init routines, so sketch doesn't have to be recompiled
*/

/****** Update the Defines in this section, as needed ********

/* NOTE: In the library file at IRLibProtocols\IRLibSAMD51.h
// You must change lines 18-19 as follows:
//#define IR_TCn 3
#define IR_TCn 4

// Also, in this same file, you must fix line 31 to add in the missing quote character at the end:
    #error "Unsupported output pin on Adafruit Metro M4"
// Otherwise, the application will NOT compile */

// Azimuth Encoder count range, if known.  If not, leave defined at 0L
#define AZIMUTH_ENCODER_RANGE    10000L

// Uncomment if Azimuth Encoder is changing counts backwards
//#define SWAP_AZIMUTH_ENCODER_AB

// Uncomment if associated Motor is moving backwards
//#define REVERSE_AZIMUTH_MOTOR_DIRECTION
//#define REVERSE_ALTITUDE_MOTOR_DIRECTION

/* Default Time Zone must be a real number, usually in the range -12.0 to +12.0
// This is in hours offset from GMT */
#define DEFAULT_TIME_ZONE    -8.0    /* PST */
// Daylight saving usage - Auto figures out if DST is active or not in US locations only
#define DEFAULT_USE_DST_AUTO_IN_US    true
#define DEFAULT_USE_DST               false

// Account for Weirdness with SerialUSB
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

// Terminal Display Output - Pick one - If at all possible, use Serial2
#define TCterminal Serial
/* Serial2 uses PINS D7-TX, D4-RX for SAMD51
// This can be set to go to an Xbee module for wireless interaction */
//#define TCterminal Serial2

/* LCD output - Serial1 is recommended for either PIC LCD or AVR LCD
// Although the AVR LCD hardware also has an IIC option
// Serial1 uses PINS D1-TX, D0-RX */
#define TC_LCD Serial1

// Comment out if not using old magnetic compass
//#define __HMC6352__
// Comment out if not using the new HMC6343 magnetic compass
#define __HMC6343__

/* Define to allow using a different section of the EEPROM
// if the first page(s) become(s) unusable
// Just change '0' to the even byte number to start at */
#define EEOFFSET 0

//****** End of User Defined Section ******************************************

#include "tcheader.h"

void setup() {
  SUPC->VREG.bit.SEL = 1; // Save 4 mA on Metro M4 board
  RRAAZ = 0L; // Assume Azimuth axis range is unknown
  if (AZIMUTH_ENCODER_RANGE != 0L) RRAAZ = AZIMUTH_ENCODER_RANGE;
  RDECAL = pow(2, 14);
  eecharbuf.strunion.DTZONE = DEFAULT_TIME_ZONE;
  DSTFLAG = eecharbuf.strunion.DSTFLAG = DEFAULT_USE_DST;
  DSTAUTOFLAG = DEFAULT_USE_DST_AUTO_IN_US;

  tcintro(); // Initialize hardware and do Startup screen
}

void loop() {
  TC_main(); // main processing loop
  WAITASEC(2); // Need to leave enough time to handle tasks
}
