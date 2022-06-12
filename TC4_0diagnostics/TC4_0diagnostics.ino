/* Arduino SAMD-based Telescope Controller 4.00.00 Diagnostics - September 2022
// See MIT LICENSE.md file and ReadMe.md file for essential information
// Highly tailored to the Sparkfun Redboard Turbo or AdaFruit M4 Metro
// DO NOT ATTEMPT TO LOAD THIS ONTO A STANDARD UNO
//
// This sketch should be used to verify software library installation
// and hardware connections before attempting to use the TC4_0.ino sketch */

//If not using the Metro M4, comment out the next line
#define __METRO_M4__

// Set to 'true' for older PIC based LCD, 'false' for newer AVR based LCD
#define LCDpicflag true

// Arduino Digital Pin Usage declarations
const int HORIZONlim = 5; // D5 - Altitude axis Horizon reference point
const int ZENITHlim = 6; // D6 - Altitude axis Zenith point
  // D7 = Serial2 Tx - to XBee Green (SAMD51) */
const int AZREFsensor = 8; /* D8 - Azimuth axis reference point */

const int SPI_SS2 = 9; // D9 - telescope tube inclinometer sensor select
const int SPI_SS1 = 10; // D10 - Rocker Base inclinometer sensor select
// IR Detector Pin - Use TV/DVD IR Remote as a handbox control
const int IR_RECVpin = 11; /* D11 - IR Remote sensor */
const int LOCKBTN = 19; /* A5 */

// Account for Redboard Turbo/SAMD21 Weirdness with SerialUSB
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#define Serial SERIAL_PORT_USBVIRTUAL
#endif
#define TCterminal Serial
#define TC_LCD Serial1

const char hitkey[] = "Hit Key to continue";
const int MotorDriver_ADR = 0x5D; //Qwiic Motor Driver

//===========================================
// Test required library installation
#include <Encoder.h> // https://www.pjrc.com/teensy/td_libs_Encoder.html
#include "wiring_private.h"
#include <Arduino.h>
#include "pins_arduino.h"
#include <Wire.h>
#include <stdint.h>
#include <string.h>

#ifndef __METRO_M4__
#include <RTCZero.h> // https://www.arduino.cc/en/Reference/RTC
#else
#include "RTC_SAMD51.h" // https://github.com/Seeed-Studio/Seeed_Arduino_RTC
#include "DateTime.h"
#endif

#include <math.h>
#include "SiderealPlanets.h" // https://github.com/DavidArmstrong/SiderealPlanets
#include "SiderealObjects.h" // https://github.com/DavidArmstrong/SiderealObjects
#include <curveFitting.h> // https://github.com/Rotario/arduinoCurveFitting
#include "WMM_Tinier.h" // https://github.com/DavidArmstrong/WMM_Tinier

#include "extEEPROM.h" // https://github.com/PaoloP74/extEEPROM
#include <SerLCD.h> // https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library
#include <SFE_MicroOLED.h>  // https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library
#include "ansi.h" // https://github.com/RobTillaart/ANSI
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // http://librarymanager/All#SparkFun_u-blox_GNSS

#include <Adafruit_INA219.h> // https://github.com/adafruit/Adafruit_INA219
#include "SparkFunBME280.h" // https://github.com/sparkfun/SparkFun_BME280_Arduino_Library

#ifndef __METRO_M4__
#include <IRremote.h> // https://github.com/z3t0/Arduino-IRremote
#else
// https://github.com/cyborg5/IRLib2 - Adds formal SAMD21 and SAMD51 support
#endif

#include "SCMD.h" // https://github.com/sparkfun/Serial_Controlled_Motor_Driver
#include "SCMD_config.h"

// Magnetic Compass Libraries
#include <HMC6352.h> // https://github.com/funflin/HMC6352-Arduino-Library
#include "SFE_HMC6343.h" // https://github.com/sparkfun/SparkFun_HMC6343_Arduino_Library
#include <SparkFun_MMC5983MA_Arduino_Library.h> // http://librarymanager/All#SparkFun_MMC5983MA
// https://github.com/sparkfun/SparkFun_MMC5983MA_Magnetometer_Arduino_Library

#include <SPI.h>
#include "SCL3300.h" // https://github.com/DavidArmstrong/Arduino-SCL3300

//============================
// Hareware/Software Declarations
// Serial2 is second UART port for Xbee wireless ANSI terminal, as an alternative to Serial
#ifndef __METRO_M4__
Uart Serial2 (&sercom2, 3, 2, SERCOM_RX_PAD_1, UART_TX_PAD_2);
#else
#define PIN_SERIAL2_RX 4    /* PB13 */
#define PAD_SERIAL2_RX (SERCOM_RX_PAD_1)
#define PIN_SERIAL2_TX 7    /* PB12 */
#define PAD_SERIAL2_TX (UART_TX_PAD_0)
Uart Serial2( &sercom4, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX );
Stream *SERIALOUT = &Serial2;
#endif

#define RAAZ_pinA   15
#define RAAZ_pinB   16
#define DECAL_pinA  17
#define DECAL_pinB  18
Encoder RAAZenc( RAAZ_pinA , RAAZ_pinB ); //Incremental Quadrature Encoder for RA/Aximuth
Encoder DECALenc( DECAL_pinA , DECAL_pinB ); //Incremental Quadrature Encoder for Dec/Altitude

#ifndef __METRO_M4__
IRrecv irrecv(RECV_PIN); //IR detector - Used for TV Remote input
RTCZero rtczero; // Sparkfun Redboard Turbo internal Real Time Clock
#else
RTC_SAMD51 rtczero;
#endif
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
SFE_HMC6343 dobHMC6343; // Declare the compass object
SFE_MMC5983MA MMC5983MAmag;
WMM_Tinier myDeclination;
SCL3300 rockerTilt, tubeTilt; // inclinometers
ANSI ansi(&TCterminal); // VT100 support
ANSI ansi2(&Serial2); // VT100 support

void setup() {
  Serial.begin(115200); //May use VT100 compatible terminal emulator
  while (!Serial && !Serial2); //Wait for user to open terminal
  newdelay(2000);
  Serial.println(" Start with Test of Serial Output.\n");
  Wire.begin(); // I2C init
  Serial1.begin(9600); // serial 4x20 LCD display

  #ifndef __METRO_M4__
  // For SAMD21 Serial2
  pinPeripheral(2, PIO_SERCOM); // Define Pins for Serial2 UART port
  pinPeripheral(3, PIO_SERCOM_ALT);
  #else
  // For SAMD51 Serial2
  pinPeripheral(4, PIO_SERCOM); // Define Pins for Serial2 UART port
  pinPeripheral(7, PIO_SERCOM);
  #endif
  Serial2.begin(9600);
  newdelay(1600); // Give time for Serial2 to init

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(AZREFsensor, INPUT_PULLUP);
  pinMode(HORIZONlim, INPUT_PULLUP);
  pinMode(ZENITHlim, INPUT_PULLUP);
  pinMode(LOCKBTN, INPUT_PULLUP);

  // Test 0 - Start means required libraries are installed
  // Test 1 - Terminal/LCD outputs
  TERMclear();
  Serial.println("Telescope Controller 4.0 Diagnostics Sketch");
  Serial.println("\nRequired Arduino Libraries are installed successfully");
  Serial.println("Default USB Serial Interface is working\n");
  LCDclear();
  Serial1.print("Telescope Controller 4.0 Diagnostics Sketch - LCD working ");
  Serial2.println("\nTelescope Controller 4.0 Diagnostics Sketch");
  Serial2.println("Required Arduino Libraries are installed successfully");
  Serial2.println("Alternate Xbee Wireless Serial Interface is working\n");

  // Test 2 - Terminal input
  Serial.println("Type letters to test input - Enter when done:");
  Serial2.println("Type letters to test input - Enter when done:");
  char PAD[80];
  ACCEPT(PAD, 80); //Get 80 chars max
  Serial.println("\n\rInput Test done\n");
  Serial2.println("\n\rInput Test done\n");

  // Test 3 - EEPROM detect - verify presence of I2C EEPROM
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

  // Test 4 - u-blox GPS detection
  if ((ubloxGPS.begin() == false) || !I2CEEPROMpresent ) {
    TCterminal.println("GPS *NOT* detected.");
    Serial2.println("GPS *NOT* detected.");
  } else { // GPS online
    TCterminal.println("GPS detected.");
    Serial2.println("GPS detected.");
  }

  // Test 5 - Murata SCL3300 Inclinometer Sensors detection
  TCterminal.print("Rocker Murata SCL3300 inclinometer ");
  Serial2.print("Rocker Murata SCL3300 inclinometer ");
  if (!rockerTilt.begin(SPI_SS1)) {
    TCterminal.print("not ");
    Serial2.print("not ");
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
  } else {
    TCterminal.println("not detected.");
    Serial2.println("not detected.");
  }

  // Test 6 - Several types of Magnetic Compass detection
  Serial.println("\nCheck for various magnetic compass hardware:");
  Serial2.println("\nCheck for various magnetic compass hardware:");
  HMC6352.Wake();  // This is the really old EOL compass
  newdelay(20);
  FMAGHDG = HMC6352.GetHeading();
  HMC6352.Sleep();
  if (FMAGHDG > 360.1) { //Old Magnetic Compass not detected at default I2C address
    TCterminal.println("HMC6352 Magnetic Compass not detected.");
    Serial2.println("HMC6352 Magnetic Compass not detected.");
    // Initialize the HMC6343 and verify its physical presence
    if (dobHMC6343.init()) {
      TCterminal.println("HMC6343 Magnetic Compass detected.");
      Serial2.println("HMC6343 Magnetic Compass detected.");
    }
    if (MMC5983MAmag.begin()) {
      TCterminal.println("MMC5983MA Magnetic Compass detected.");
      Serial2.println("MMC5983MA Magnetic Compass detected.");
      MMC5983MAmag.softReset();
    }
  } else { // Old Magnetic Compass online
    TCterminal.println("HMC6352 Magnetic Compass detected.");
    Serial2.println("HMC6352 Magnetic Compass detected.");
  }
  TCterminal.println("");
  Serial2.println("");

  // Test 7 - Voltage monitor detection
  TCterminal.print("Adafruit INA219 Voltage/Current monitor ");
  Serial2.print("Adafruit INA219 Voltage/Current monitor ");
  if (!ina219.begin()) {
    TCterminal.print("not ");
    Serial2.print("not ");
  }
  TCterminal.println("detected.");
  Serial2.println("detected.");

  // Test 8 - BME280 Temperature, Pressure,Humidity Sensor detection
  byte bme280_id = bme280.begin(); //Returns ID of 0x60 if successful
  TCterminal.print("BME280 Humidity, Barometric Pressure and Temperature sensor ");
  Serial2.print("BME280 Humidity, Barometric Pressure and Temperature sensor ");
  if (bme280_id != 0x60) { // Problem with BME280
    TCterminal.print("not ");
    Serial2.print("not ");
  }
  TCterminal.println("detected.");
  Serial2.println("detected.");

  // Test 9 - Azimuth Reference Sensor Detection
  TCterminal.println("\nAzimuth Reference Sensor check");
  Serial2.println("\nAzimuth Reference Sensor check");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  while (!(TCterminal.available() || Serial2.available())) {
    TCterminal.print("Azimuth Reference Sensor is ");
    Serial2.print("Azimuth Reference Sensor is ");
    if (digitalRead(AZREFsensor) == LOW) {
      TCterminal.println("LOW");
      Serial2.println("LOW");
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

  // Test 10 - Horizon Reference Sensor Detection
  TCterminal.println("\nHorizon Reference Sensor check");
  Serial2.println("\nHorizon Reference Sensor check");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  while (!(TCterminal.available() || Serial2.available())) {
    TCterminal.print("Horizon Reference Sensor is ");
    Serial2.print("Horizon Reference Sensor is ");
    if (digitalRead(HORIZONlim) == LOW) {
      TCterminal.println("LOW");
      Serial2.println("LOW");
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
  
  // Test 11 - Zenith Reference Sensor Detection
  TCterminal.println("\nZenith Reference Sensor check");
  Serial2.println("\nZenith Reference Sensor check");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  while (!(TCterminal.available() || Serial2.available())) {
    TCterminal.print("Zenith Reference Sensor is ");
    Serial2.print("Zenith Reference Sensor is ");
    if (digitalRead(ZENITHlim) == LOW) {
      TCterminal.println("LOW");
      Serial2.println("LOW");
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

  // Test 12 - Lcok/Reinitialize Switch Detection
  TCterminal.println("\nLcok/Reinitialize Switch check");
  Serial2.println("\nLcok/Reinitialize Switch check");
  TCterminal.println(hitkey);
  Serial2.println(hitkey);
  while (!(TCterminal.available() || Serial2.available())) {
    TCterminal.print("Lcok/Reinitialize Switch is ");
    Serial2.print("Lcok/Reinitialize Switch is ");
    if (digitalRead(LOCKBTN) == LOW) {
      TCterminal.println("LOW");
      Serial2.println("LOW");
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

  // Test 13 - Motor Driver Board detection
  i2cMotorDriver.settings.commInterface = I2C_MODE;
  i2cMotorDriver.settings.I2CAddress = MotorDriver_ADR;
  TCterminal.print("\nI2C Serial Controlled Motor Driver board ");
  Serial2.print("\nI2C Serial Controlled Motor Driver board ");
  if ( i2cMotorDriver.begin() != 0xA9 ) //See if a valid ID word is returned
  {
    TCterminal.print("not ");
    Serial2.print("not ");
  }
  TCterminal.println("detected.");
  Serial2.println("detected.");

  TCterminal.println( "\nTests Completed" );
  Serial2.println( "\nTests Completed" );
}

void loop() {
  // put your main code here, to run repeatedly:
  while (1) ;
}

#ifndef __METRO_M4__
void SERCOM2_Handler() {
  Serial2.IrqHandler();
}
#else
// And for SAMD51 - can't use sercom5, 3, or 2
// sercom3 - Serial1 on D0/D1
// sercom5 - I2C pins for SCL/SDA
// sercom2 - SPI
void SERCOM4_0_Handler()
{
  Serial2.IrqHandler();
}
void SERCOM4_1_Handler()
{
  Serial2.IrqHandler();
}
void SERCOM4_2_Handler()
{
  Serial2.IrqHandler();
}
void SERCOM4_3_Handler()
{
  Serial2.IrqHandler();
}
#endif

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
  //TCterminal.print(" ");
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
