/* Arduino SAMD-based Telescope Controller 4.00.00 Diagnostics - July 2022
// See MIT LICENSE.md file and ReadMe.md file for essential information
// Highly tailored to the AdaFruit M4 Metro
// DO NOT ATTEMPT TO LOAD THIS ONTO A STANDARD UNO
//
// This sketch should be used to verify software library installation
// and hardware connections before attempting to use the TC4_0.ino sketch */

//****** Update the Defines in this section, as needed ********

/* NOTE: In the library file at IRLibProtocols\IRLibSAMD51.h
// You must change lines 18-19 as follows:
//#define IR_TCn 3
#define IR_TCn 4
// Otherwise, the application will NOT compile */

// Set to 'true' for older PIC based LCD, 'false' for newer AVR based LCD
#define LCDpicflag true

// Uncomment if associated Encoder is changing counts backwards
//#define SWAP_AZIMUTH_ENCODER_AB
//#define SWAP_ALTITUDE_ENCODER_AB

// Uncomment if associated Motor is moving backwards
//#define REVERSE_AZIMUTH_MOTOR_DIRECTION
//#define REVERSE_ALTITUDE_MOTOR_DIRECTION

// Comment out if not using old magnetic compass
//#define __HMC6352__

//****** End of User Defined Section ******************************************

// Account for Weirdness with SerialUSB
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#define Serial SERIAL_PORT_USBVIRTUAL
#endif
#define TCterminal Serial
#define TC_LCD Serial1

// Arduino Digital Pin Usage declarations
const int HORIZONlim = 5; // D5 - Altitude axis Horizon reference point
const int ZENITHlim = 6; // D6 - Altitude axis Zenith point
// D7 = Serial2 Tx - to XBee Green (SAMD51) */
const int AZREFsensor = 8; /* D8 - Azimuth axis reference point */
const int SPI_SS2 = 9; // D9 - telescope tube inclinometer sensor select
const int SPI_SS1 = 10; // D10 - Rocker Base inclinometer sensor select
// IR Detector Pin - Use TV/DVD IR Remote as a handbox control
const int IR_RECVpin = 11; /* D11 - IR Remote sensor */
#define RAAZ_pinA   15
#define RAAZ_pinB   16
#define DECAL_pinA  17
#define DECAL_pinB  18
const int LOCKBTN = 19; /* A5 */
// define IR receive pin - for IR Remote Library routines
int RECV_PIN = IR_RECVpin;

const char hitkey[] = "Hit Key to continue";
const int MotorDriver_ADR = 0x5D; //Qwiic Motor Driver
// Motor Defines to make it more readable
#define ALTITUDE_MOTOR 0
#define AZIMUTH_MOTOR 1
#define CW_DIRECTION 0
#define CCW_DIRECTION 1
#define STOP_SPEED 0
#define SLOW_SPEED 5
#define FAST_SPEED 255

//===========================================
// Test required library installation
#ifndef __Libraries__
#include <Encoder.h> // https://github.com/PaulStoffregen/Encoder
#include "wiring_private.h"
#include <Arduino.h>
#include "pins_arduino.h"
#include <Wire.h>
#include <stdint.h>
#include <string.h>

#include "RTC_SAMD51.h" // https://github.com/Seeed-Studio/Seeed_Arduino_RTC
#include "DateTime.h"

#include <math.h>
#include "SiderealPlanets.h" // https://github.com/DavidArmstrong/SiderealPlanets
#include "SiderealObjects.h" // https://github.com/DavidArmstrong/SiderealObjects
#include <curveFitting.h> // https://github.com/Rotario/arduinoCurveFitting
#include "WMM_Tinier.h" // https://github.com/DavidArmstrong/WMM_Tinier

#include "extEEPROM.h" // https://github.com/PaoloP74/extEEPROM
#include <SerLCD.h> // https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library
#include <SFE_MicroOLED.h>  // https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library
#include "ansi.h" // https://github.com/RobTillaart/ANSI
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library

#include <Adafruit_INA219.h> // https://github.com/adafruit/Adafruit_INA219
#include "SparkFunBME280.h" // https://github.com/sparkfun/SparkFun_BME280_Arduino_Library

#include <IRLibDecodeBase.h> // https://github.com/cyborg5/IRLib2 
#include <IRLib_P01_NEC.h>   // Now include only the protocols you wish
#include <IRLib_P02_Sony.h>  // to actually use. The lowest numbered
#include <IRLib_P07_NECx.h>  // must be first but others can be any order.
//#include <IRLib_P09_GICable.h>
//#include <IRLib_P11_RCMM.h>
#include <IRLibCombo.h>     // After all protocols, include this
// All of the above automatically creates a universal decoder
// class called "IRdecode" containing only the protocols you want.
// Now declare an instance of that decoder.
IRdecode irdecoder;
// Include a receiver either this or IRLibRecvPCI or IRLibRecvLoop
#include <IRLibRecv.h>

#include "SCMD.h" // https://github.com/sparkfun/Serial_Controlled_Motor_Driver
#include "SCMD_config.h"

// Magnetic Compass Libraries
#ifdef __HMC6352__
#include <HMC6352.h> // https://github.com/funflin/HMC6352-Arduino-Library
#endif
//#include "SFE_HMC6343.h" // https://github.com/sparkfun/SparkFun_HMC6343_Arduino_Library
#include <SparkFun_MMC5983MA_Arduino_Library.h> // https://github.com/sparkfun/SparkFun_MMC5983MA_Magnetometer_Arduino_Library

#include <SPI.h>
#include "SCL3300.h" // https://github.com/DavidArmstrong/Arduino-SCL3300

//============================
// Hareware/Software Declarations
// Serial2 is second UART port for Xbee wireless ANSI terminal, as an alternative to Serial
#define PIN_SERIAL2_RX 4    /* PB13 */
#define PAD_SERIAL2_RX (SERCOM_RX_PAD_1)
#define PIN_SERIAL2_TX 7    /* PB12 */
#define PAD_SERIAL2_TX (UART_TX_PAD_0)
Uart Serial2( &sercom4, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX );
Stream *SERIALOUT = &Serial2;

#ifndef SWAP_AZIMUTH_ENCODER_AB
Encoder RAAZenc( RAAZ_pinA , RAAZ_pinB ); //Incremental Quadrature Encoder for RA/Aximuth
#else
Encoder RAAZenc( RAAZ_pinB , RAAZ_pinA ); //Incremental Quadrature Encoder for RA/Aximuth
#endif
#ifndef SWAP_ALTITUDE_ENCODER_AB
Encoder DECALenc( DECAL_pinA , DECAL_pinB ); //Incremental Quadrature Encoder for Dec/Altitude
#else
Encoder DECALenc( DECAL_pinB , DECAL_pinA ); //Incremental Quadrature Encoder for Dec/Altitude
#endif

IRrecv irrecv(RECV_PIN);  //pin number for the receiver
RTC_SAMD51 rtczero;

const int PIN_RESET = 44;
#define DC_JUMPER 1
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration
BME280 bme280; //Temperature, Barometric Pressure, Humidity Sensor
SFE_UBLOX_GNSS ubloxGPS;
Adafruit_INA219 ina219; // Bus voltage monitor

// IIC address of EEPROM - 24LC256 or 24C512C-L */
//const int EEPROM_ADR = 0x55;
const int EEPROM_ADR = 0x50;
boolean I2CEEPROMpresent, GPSpresent, BMEpresent;
// 256 kbits = 32K bytes for EEPROM
//extEEPROM tcEEPROM(kbits_256, 1, 64, EEPROM_ADR);
// 512 kbits = 64K bytes for EEPROM
extEEPROM tcEEPROM(kbits_512, 1, 64, EEPROM_ADR);

SiderealPlanets myAstro;
SiderealObjects myObjects;
boolean MotorDriverflag;
SCMD i2cMotorDriver; //Serial Controlled Motor Driver

double FMAGHDG; // Magnetic Compass heading
//SFE_HMC6343 dobHMC6343; // Declare the compass object
SFE_MMC5983MA MMC5983MAmag;
WMM_Tinier myDeclination;
SCL3300 rockerTilt, tubeTilt; // inclinometers
ANSI ansi(&TCterminal); // VT100 support
ANSI ansi2(&Serial2); // VT100 support
#endif

/* Function declarations
// And for SAMD51 - can't use sercom5, 3, or 2
// sercom3 - Serial1 on D0/D1
// sercom5 - I2C pins for SCL/SDA
// sercom2 - SPI */
void SERCOM4_0_Handler() { Serial2.IrqHandler(); }
void SERCOM4_1_Handler() { Serial2.IrqHandler(); }
void SERCOM4_2_Handler() { Serial2.IrqHandler(); }
void SERCOM4_3_Handler() { Serial2.IrqHandler(); }

void newdelay( long interval ) {
  // delay without blocking
  // See https://www.arduino.cc/en/tutorial/BlinkWithoutDelay
  // Generally, you should use "unsigned long" for variables that hold time
  // The value will quickly become too large for an int to store
  unsigned long previousMillis = millis();
  unsigned long currentMillis = millis();

  while (currentMillis - previousMillis < interval) {
    currentMillis = millis();
  }
}

void TERMclear() {
  //Clear VT100 screen and home cursor
  ansi.clearScreen();
  ansi2.clearScreen();
}

void LCDclear() { // Clear display and home cursor
  if (LCDpicflag) {
    // PIC based LCD on TC_LCD TxD line
    TC_LCD.write(0xfe); // = 254 decimal
    TC_LCD.write(0x01);
  } else {
    // OpenLCD on TC_LCD TXD line
    TC_LCD.write('|'); //Send setting character = 124 = 0x7c
    TC_LCD.write('-'); //Send clear display character
  }
}

char KEY() { // return a character of input from any source
  char n = ' ';
  long xnL;
  boolean rcvd = false;

  do {
    if (TCterminal.available()) {
      n = TCterminal.read();
      rcvd = true;
    } else if (Serial2.available()) {
        n = Serial2.read();
        rcvd = true;
    }
  } while (rcvd == false);
  return n;
}

void LCDprint(char tmp) { TC_LCD.print(tmp); }
void LCDprint(const char* tmp) { TC_LCD.print(tmp); }

int ACCEPT(char *buf, int limit) {
  // read a line from input into buffer, return char count
  int x;
  int ptr = 0;
  *buf = 0;

  for (x = 0; x < limit; x++) {
    buf[x] = 0;
  }
  while (1) {
    x = KEY();
    if (x == 0x0D) { // cr == end of line
      buf[ptr] = 0; // flag end of line
      return ptr; // return char count
    }
    if (x == 0x08) { // backspace
      if (ptr > 0) {
        ptr--;
        buf[ptr] = 0;
        TCterminal.print ("\b \b"); // erase bs'd char
        Serial2.print("\b \b"); // erase bs'd char
      }
    } else {
      if (ptr < (limit - 1)) {
        TCterminal.print ((char) x); // echo char
        Serial2.print ((char) x); // echo char
        buf[ptr] = x; // store in buffer
        ptr++;
      }
    }
  }
}

void setup() {
  Serial.begin(115200); //May use VT100 compatible terminal emulator
  while (!Serial && !Serial2); //Wait for user to open terminal
  newdelay(2000);
  TCterminal.println(" Start with Test of Serial Output.\n");
  TCterminal.println(" Init I2C.\n");
  Wire.begin(); // I2C init
  TCterminal.println(" Init Serial1 for LCD.\n");
  Serial1.begin(9600); // serial 4x20 LCD display

  // For SAMD51 Serial2
  TCterminal.println(" Init Serial2 for XeeBee.\n");
  pinPeripheral(4, PIO_SERCOM); // Define Pins for Serial2 UART port
  pinPeripheral(7, PIO_SERCOM);
  TCterminal.println(" Do Serial2 begin().\n");
  Serial2.begin(9600);
  newdelay(1600); // Give time for Serial2 to init

  TCterminal.println(" Init limits and pushbutton inputs.\n");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(AZREFsensor, INPUT_PULLUP);
  pinMode(HORIZONlim, INPUT_PULLUP);
  pinMode(ZENITHlim, INPUT_PULLUP);
  pinMode(LOCKBTN, INPUT_PULLUP);
  newdelay(1600); // Give time to see this.

  // Test 0 - Start means required libraries are installed
  // Test 1 - Terminal/LCD outputs
  TERMclear();
  TCterminal.println("Telescope Controller 4.0 Diagnostics Sketch");
  TCterminal.println("\nRequired Arduino Libraries are installed successfully");
  TCterminal.println("Default USB Serial Interface is working\n");
  LCDclear();
  TC_LCD.print("Telescope Controller 4.0 Diagnostics Sketch - LCD working ");
  Serial2.println("Telescope Controller 4.0 Diagnostics Sketch");
  Serial2.println("\nRequired Arduino Libraries are installed successfully");
  Serial2.println("Alternate Xbee Wireless Serial Interface is working\n");

  // Test 2 - Terminal input
  TCterminal.println("Type letters to test input - Enter when done:");
  Serial2.println("Type letters to test input - Enter when done:");
  char PAD[80];
  ACCEPT(PAD, 80); //Get 80 chars max
  TCterminal.println("\n\rInput Test done\n");
  Serial2.println("\n\rInput Test done\n");

  // Test 3 - Azimuth Encoder check
  TCterminal.println("\nAzimuth Encoder check");
  Serial2.println("\nAzimuth Encoder check");
  TCterminal.println("Counts must increase when going from North to East to South to West!");
  Serial2.println("Counts must increase when going from North to East to South to West!");
  TCterminal.println("If not, uncomment line #define SWAP_AZIMUTH_ENCODER_AB");
  Serial2.println("If not, uncomment line #define SWAP_AZIMUTH_ENCODER_AB");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  RAAZenc.write(0L);
  long RAAZ;
  while (!(TCterminal.available() || Serial2.available())) {
    RAAZ = RAAZenc.read();
    TCterminal.print("Azimuth Encoder Position: ");
    Serial2.print("Azimuth Encoder Position: ");
    TCterminal.println(RAAZ);
    Serial2.println(RAAZ);
    newdelay(100);
    ansi.cursorUp(1);
    ansi2.cursorUp(1);
  }
  KEY();
  TCterminal.println("");
  Serial2.println("");
  
  // Test 4 - Altitude Encoder check
  TCterminal.println("\nAltitude Encoder check");
  Serial2.println("\nAltitude Encoder check");
  TCterminal.println("Counts must increase when going from Horizon to Zenith!");
  Serial2.println("Counts must increase when going from Horizon to Zenith!");
  TCterminal.println("If not, uncomment line #define SWAP_ALTITUDE_ENCODER_AB");
  Serial2.println("If not, uncomment line #define SWAP_ALTITUDE_ENCODER_AB");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  DECALenc.write(0L);
  long DECAL;
  while (!(TCterminal.available() || Serial2.available())) {
    DECAL = DECALenc.read();
    TCterminal.print("Altitude Encoder Position: ");
    Serial2.print("Altitude Encoder Position: ");
    TCterminal.println(DECAL);
    Serial2.println(DECAL);
    newdelay(100);
    ansi.cursorUp(1);
    ansi2.cursorUp(1);
  }
  KEY();
  TCterminal.println("\n");
  Serial2.println("\n");
  
  
  // Test 5 - EEPROM detect - verify presence of I2C EEPROM
  byte EEPROMstatus = tcEEPROM.begin(extEEPROM::twiClock100kHz);
  I2CEEPROMpresent = true;
  TCterminal.print("I2C EEPROM ");
  Serial2.print("I2C EEPROM ");
  if ( EEPROMstatus != 0 ) {
    I2CEEPROMpresent = false;
    TCterminal.print("*NOT* ");
    Serial2.print("*NOT* ");
  }
  TCterminal.println("detected.");
  Serial2.println("detected.");

  // Test 6 - u-blox GPS detection
  TCterminal.print("u-blox GPS ");
  Serial2.print("u-blox GPS ");
  if ((ubloxGPS.begin() == false) || !I2CEEPROMpresent ) {
    TCterminal.print("*NOT* ");
    Serial2.print("*NOT* ");
  }
  TCterminal.println("detected.");
  Serial2.println("detected.");

  // Test 7 - Murata SCL3300 Inclinometer Sensors detection
  TCterminal.print("Rocker Murata SCL3300 inclinometer ");
  Serial2.print("Rocker Murata SCL3300 inclinometer ");
  if (!rockerTilt.begin(SPI_SS1)) {
    TCterminal.print("not ");
    Serial2.print("not ");
  } else {
	rockerTilt.powerDownMode();
  }
  TCterminal.println("detected.");
  Serial2.println("detected.");
  
  TCterminal.print("Tube Murata SCL3300 inclinometer ");
  Serial2.print("Tube Murata SCL3300 inclinometer ");
  if (tubeTilt.begin(SPI_SS2)) {
    TCterminal.println("detected.");
    Serial2.println("detected.");
    // Put this inclinometer into Mode 1, since we are using it to measure 0-90 Degree angles
    tubeTilt.setMode(1);
    TCterminal.print("Tube Murata SCL3300 inclinometer ");
    Serial2.print("Tube Murata SCL3300 inclinometer ");
    if (tubeTilt.begin(SPI_SS2)) {
      TCterminal.print("now in");
      Serial2.print("now in");
    } else {
      TCterminal.print("failed to transition to");
      Serial2.print("failed to transition to");
    }
    TCterminal.println(" Mode 1.");
    Serial2.println(" Mode 1.");
	tubeTilt.powerDownMode();
  } else {
    TCterminal.println("not detected.");
    Serial2.println("not detected.");
  }

  // Test 8 - Several types of Magnetic Compass detection
  TCterminal.println("\nCheck for various magnetic compass hardware:");
  Serial2.println("\nCheck for various magnetic compass hardware:");
#ifdef __HMC6352__
  HMC6352.Wake();  // This is the really old EOL compass
  newdelay(20);
  FMAGHDG = HMC6352.GetHeading();
  HMC6352.Sleep();
  if (FMAGHDG > 360.1) { //Old Magnetic Compass not detected at default I2C address
    TCterminal.println("HMC6352 Magnetic Compass not detected.");
    Serial2.println("HMC6352 Magnetic Compass not detected.");
#endif
    /* Initialize the HMC6343 and verify its physical presence
    if (dobHMC6343.init()) {
      TCterminal.println("HMC6343 Magnetic Compass detected.");
      Serial2.println("HMC6343 Magnetic Compass detected.");
    } // */
    TCterminal.println("MMC5983MA Magnetic Compass ");
    Serial2.println("MMC5983MA Magnetic Compass ");
    if (MMC5983MAmag.begin()) {
      TCterminal.println("detected.");
      Serial2.println("detected.");
      MMC5983MAmag.softReset();
    } else {
      TCterminal.println("not detected.");
      Serial2.println("not detected.");
	}
#ifdef __HMC6352__
  } else { // Old Magnetic Compass online
    TCterminal.println("HMC6352 Magnetic Compass detected.");
    Serial2.println("HMC6352 Magnetic Compass detected.");
  }
#endif
  TCterminal.println("");
  Serial2.println("");

  // Test 9 - Voltage monitor detection
  TCterminal.println("WARNING: Checking for a non-existant INA219 Voltage/Current monitor will hang the system!");
  Serial2.println("WARNING: Checking for a non-existant INA219 Voltage/Current monitor will hang the system!");
  TCterminal.println("Should this check be done? (y/n)");
  Serial2.println("Should this check be done? (y/n)");
  int x = KEY();
  if ( x == 'y' ) {
    TCterminal.print(" INA219 Voltage/Current monitor ");
    Serial2.print(" INA219 Voltage/Current monitor ");
    newdelay(500);
    if (!ina219.begin()) {
      TCterminal.print("not ");
      Serial2.print("not ");
    }
    TCterminal.println("detected.");
    Serial2.println("detected.");
  }

  // Test 10 - BME280 Temperature, Pressure,Humidity Sensor detection
  byte bme280_id = bme280.begin(); //Returns ID of 0x60 if successful
  TCterminal.print("\nBME280 Humidity, Barometric Pressure and Temperature sensor ");
  Serial2.print("\nBME280 Humidity, Barometric Pressure and Temperature sensor ");
  if (bme280_id != 0x60) { // Problem with BME280
    TCterminal.print("not ");
    Serial2.print("not ");
  }
  TCterminal.println("detected.");
  Serial2.println("detected.");

  // Test 11 - Azimuth Reference Sensor Detection
  TCterminal.println("\nAzimuth Reference Sensor check");
  Serial2.println("\nAzimuth Reference Sensor check");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  while (!(TCterminal.available() || Serial2.available())) {
    TCterminal.print("Azimuth Reference Sensor is ");
    Serial2.print("Azimuth Reference Sensor is ");
    if (digitalRead(AZREFsensor) == LOW) {
      TCterminal.println("LOW ");
      Serial2.println("LOW ");
    } else {
      TCterminal.println("HIGH");
      Serial2.println("HIGH");
    }
    newdelay(500);
    ansi.cursorUp(1);
    ansi2.cursorUp(1);
  }
  KEY();
  TCterminal.println("");
  Serial2.println("");

  // Test 12 - Horizon Reference Sensor Detection
  TCterminal.println("\nHorizon Reference Sensor check");
  Serial2.println("\nHorizon Reference Sensor check");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  while (!(TCterminal.available() || Serial2.available())) {
    TCterminal.print("Horizon Reference Sensor is ");
    Serial2.print("Horizon Reference Sensor is ");
    if (digitalRead(HORIZONlim) == LOW) {
      TCterminal.println("LOW ");
      Serial2.println("LOW ");
    } else {
      TCterminal.println("HIGH");
      Serial2.println("HIGH");
    }
    newdelay(500);
    ansi.cursorUp(1);
    ansi2.cursorUp(1);
  }
  KEY();
  TCterminal.println("");
  Serial2.println("");
  
  // Test 13 - Zenith Reference Sensor Detection
  TCterminal.println("\nZenith Reference Sensor check");
  Serial2.println("\nZenith Reference Sensor check");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  while (!(TCterminal.available() || Serial2.available())) {
    TCterminal.print("Zenith Reference Sensor is ");
    Serial2.print("Zenith Reference Sensor is ");
    if (digitalRead(ZENITHlim) == LOW) {
      TCterminal.println("LOW ");
      Serial2.println("LOW ");
    } else {
      TCterminal.println("HIGH");
      Serial2.println("HIGH");
    }
    newdelay(500);
    ansi.cursorUp(1);
    ansi2.cursorUp(1);
  }
  KEY();
  TCterminal.println("");
  Serial2.println("");

  // Test 14 - Lock/Reinitialize Switch Detection
  TCterminal.println("\nLock/Reinitialize Switch check");
  Serial2.println("\nLock/Reinitialize Switch check");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  while (!(TCterminal.available() || Serial2.available())) {
    TCterminal.print("Lock/Reinitialize Switch is ");
    Serial2.print("Lock/Reinitialize Switch is ");
    if (digitalRead(LOCKBTN) == LOW) {
      TCterminal.println("LOW ");
      Serial2.println("LOW ");
    } else {
      TCterminal.println("HIGH");
      Serial2.println("HIGH");
    }
    newdelay(500);
    ansi.cursorUp(1);
    ansi2.cursorUp(1);
  }
  KEY();
  TCterminal.println("");
  Serial2.println("");

  // Test 15 - Motor Driver Board detection and Motor test
  boolean motorDetected = false;
  i2cMotorDriver.settings.commInterface = I2C_MODE;
  i2cMotorDriver.settings.I2CAddress = MotorDriver_ADR;
  TCterminal.print("\nI2C Serial Controlled Motor Driver board ");
  Serial2.print("\nI2C Serial Controlled Motor Driver board ");
  if ( i2cMotorDriver.begin() != 0xA9 ) //See if a valid ID word is returned
  {
    TCterminal.print("not ");
    Serial2.print("not ");
  } else {
    motorDetected = true;
  }
  TCterminal.println("detected.");
  Serial2.println("detected.");
  if (motorDetected) {
    TCterminal.println("Waiting to enable outputs...");
    Serial2.println("Waiting to enable outputs...");
    while ( i2cMotorDriver.ready() == false );
    // Check for Azimuth motor inversion
#ifdef REVERSE_AZIMUTH_MOTOR_DIRECTION
    while( i2cMotorDriver.busy() ); //Waits until the SCMD is available.
    i2cMotorDriver.inversionMode(0, 1); //invert Azimuth motor
#endif
    // Check for Altitude motor inversion
#ifdef REVERSE_ALTITUDE_MOTOR_DIRECTION
    while ( i2cMotorDriver.busy() ); //Waits until the SCMD is available.
    i2cMotorDriver.inversionMode(1, 1); //invert Altitude motor
#endif
    while ( i2cMotorDriver.busy() );
    i2cMotorDriver.enable(); //Enables the output driver hardware
	
    TCterminal.println("\nHit Key to drive Azimuth motor CW for 10 seconds.");
    Serial2.println("\nHit Key to drive Azimuth motor CW for 10 seconds.");
    KEY();
    i2cMotorDriver.setDrive( AZIMUTH_MOTOR, CW_DIRECTION, FAST_SPEED );
    newdelay(10000);
    i2cMotorDriver.setDrive( AZIMUTH_MOTOR, 0, 0 );
    TCterminal.println("If motor moved CCW, uncomment line #define REVERSE_AZIMUTH_MOTOR_DIRECTION");
    Serial2.println("If motor moved CCW, uncomment line #define REVERSE_AZIMUTH_MOTOR_DIRECTION");

    TCterminal.println("\nHit Key to drive Altitude motor towards Zenith for 10 seconds.");
    Serial2.println("\nHit Key to drive Altitude motor towards Zenith for 10 seconds.");
    KEY();
    i2cMotorDriver.setDrive( ALTITUDE_MOTOR, CW_DIRECTION, FAST_SPEED );
    newdelay(10000);
    i2cMotorDriver.setDrive( ALTITUDE_MOTOR, 0, 0 );
    TCterminal.println("If motor moved towards Horizon, uncomment line #define REVERSE_ALTITUDE_MOTOR_DIRECTION");
    Serial2.println("If motor moved towards Horizon, uncomment line #define REVERSE_ALTITUDE_MOTOR_DIRECTION");
  }

  // Test 16 - IR Remote Receiver functionality check
  long xn;
  TCterminal.println("\nIR Remote Receiver functionality check");
  Serial2.println("\nIR Remote Receiver functionality check");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);

  do {
    if (irrecv.getResults()) {
      irdecoder.decode();  //Decode it
      xn = irdecoder.value;
      if (xn != 0xffffffffL) {
        TCterminal.println(xn, HEX);
        Serial2.println(xn, HEX);
      }
      irrecv.enableIRIn(); // Receive the next value
    }
  } while (!(TCterminal.available() || Serial2.available()));
  KEY();

  // Test 17 - OLED display test - Optional
  boolean OledDetected = false;
  TCterminal.println("\nWARNING: Checking for a non-existant OLED display may hang the system!");
  Serial2.println("\nWARNING: Checking for a non-existant OLED display may hang the system!");
  TCterminal.println("Should this check be done? (y/n)");
  Serial2.println("Should this check be done? (y/n)");
  x = KEY();
  if ( x == 'y' ) {
    TCterminal.print("OLED display Test Start");
    Serial2.print("OLED display Test Start");
    newdelay(500);
	  oled.begin();
    oled.clear(ALL);
    oled.display();
	  newdelay(500);
    oled.clear(PAGE);
	  oled.setFontType(0);
    oled.setCursor(0, 0);
	  oled.print("OLED Test ");
	  oled.print("TC 4.00.00");
	  oled.display();
  }

  TCterminal.println( "\n\nTests Completed\n" );
  Serial2.println( "\n\nTests Completed\n" );
}

void loop() {
  // put your main code here, to run repeatedly:
  while (1) ;
}
