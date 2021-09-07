// Telescope Controller 4.0 - Header file

#ifndef TC4_0_H
#define TC4_0_H
const char tcversion[] = "4.00.00";
/*==========================
// Arduino SAMD Board Pin Definitions: digital from 0 to 13, Analog from 14 to 19
// D0 = Serial1 Rx - not needed, but can't be used elsewhere either
// D1 = Serial1 Tx - to 4x20 LCD
// D2 = Serial2 Tx - to XBee
// D3 = Serial2 Rx - from XBee
// Pins used for optional Reference sensors */
const int AZREFsensor = 4; /* D4 - Azimuth axis reference point */
const int HORIZONlim = 5; /* D5 - Altitude axis Horizon reference point */
/* const int ZENITHlim = 6; // D6 - Altitude axis Zenith point
//const int EQUATORlim = 7; // D7 - Equatorial mount Right Ascension axis Meridian Reference
// D8 = Not Used */
const int SPI_SS2 = 9; // D9 - Rocker Base inclinometer sensor select
const int SPI_SS1 = 10; // D10 - telescope tube inclinometer sensor select
// IR Detector Pin - Option to use TV/DVD IR Remote as a handbox control
const int IR_RECVpin = 11; /* D11 - IR Remote sensor */
// D12 = Not Used
const int BLUE_LED = 13; // Blue "stat" LED on pin D13

// D14 = A0 - Not Used
// Define Encoder Pins - Must have this if nothing else
const int RAAZ_pinA = 15; // A1 - Azimuth Encoder
const int RAAZ_pinB = 16; // A2 
const int DECAL_pinA = 17; // A3 - Altitude Encoder
const int DECAL_pinB = 18; // A4
// Real Hardware button for forced re-init, position Lock, etc.
const int LOCKBTN = 19; /* A5 */
/*
// D20 = I2C SDA
// D21 = I2C SCL
// Use ICSP pins here for optional Murata SCA3300 inclinometer(s)
// D22 = MISO
// D23 = MOSI
// D24 = SCK */
const int RX_LED = PIN_LED_RXL; // RX LED on pin 25, use the predefined PIN_LED_RXL to make sure
const int TX_LED = PIN_LED_TXL; // TX LED on pin 26, use the predefined PIN_LED_TXL to make sure
/* D27-29 = USB
// D30-31 = EDBG Serial TX, RX

//The OLED library assumes a reset pin is necessary.
//The Qwiic OLED has RST hard-wired, so pick an IO pin that is not being used */
const int PIN_RESET = 44; //This is for the Neo-Pixel LED, which isn't used
// D45 = Last pin defined for SAMD chip

/*==================================
// I2C Addresses used
// I2C Magnetic Compass (HMC6352 address 0x21) (HMC6343 address 0x19)
// I2C OLED display (IIC address 0x3c)
// I2C Voltage/Current sensor (IIC address 0x40)
// I2C GPS receiver module (IIC address 0x42)
// IIC address of EEPROM - 24LC256 or 24C512C-L */
const int EEPROM_ADR = 0x50;
/* Motor Driver config pattern "1000" on board for address 0x5D
//const int MotorDriver_ADR = 0x5D; //Qwiic Motor Driver
// DS1307 I2C 5v RTC (IIC address 0x68) */
const int DS1307_ADR = 0x68;
const int LCDi2c_ADR = 0x72; //Default for Sparkfun OpenLCD
// BME280 Temperature, Barometric Pressure, Relative Humidity Sensoe I2C address
const int BME280_ADR = 0x77;
/* This OLED definition needed - The DC_JUMPER is the I2C Address Select jumper. 
// Set to 1 if the jumper is open (Default), or set to 0 if it's closed. */
#define DC_JUMPER 1 

//====================================================
// Libraries!  This is why I use Arduino, seriously.
#include "wiring_private.h" // pinPeripheral() function for Serial2 UART
#include <Arduino.h>
#include "pins_arduino.h"
#include <Wire.h>  // Include Wire if you're using I2C
#include <stdint.h>
#include <string.h>
#include <RTCZero.h> //SAMD Real Time Clock https://www.arduino.cc/en/Reference/RTC
#include <math.h> //Oh yeah.....
#include <SiderealPlanets.h> //Astronomy routines
#include <SiderealObjects.h> //Astronomy Tables
#include <DobsonianControl.h>
#include "extEEPROM.h" // I2C EEPROM https://github.com/PaoloP74/extEEPROM
#include <SerLCD.h> //Serial LCD library: https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library
#include <SFE_MicroOLED.h>  // SFE_MicroOLED library https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library
#include "ansi.h" // VT100 support https://github.com/RobTillaart/ANSI
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // http://librarymanager/All#SparkFun_Ublox_GPS
#include <Adafruit_INA219.h> //Power/current/voltage monitor https://github.com/adafruit/Adafruit_INA219
#include "SparkFunBME280.h" //Temperature, Pressure, Humidity sensor https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
#include <IRremote.h> //Use TV/DVD IR remote https://github.com/z3t0/Arduino-IRremote
#include <SPI.h> //Need for Murata inclinometer sensor
#include <SCL3300.h> //Oooo, where did this come from?!  :-)

//=========================================
// EEPROM definitions
const int MAXUCOORD = 200; // maximum number of User Defined Coordinates
struct EEstruct {
  double dummy;
  double FLATITUDE;
  double FLONGITUDE;
  float DTEMPF; // default Farenheit temperature
  float DTZONE; // Default Time zone offset from GMT
  // IR decode Table - 20 chars match one of 20 long numbers
  long IRinput[32]; // Raw IR remote button numbers
  char IRchar[32]; // Corresponding ASCII char for each number
  // User defined coordinates
  long UCOORD[MAXUCOORD * 2]; // save space for user defined objects
  long RRAAZ; //Default encoder range for RA/Azimuth
  long RDECAL; //Default encoder range for Declination/Altitude
  int ELEVATION; // default elevation
  int EEchk;
  boolean DSTFLAG; // Set to true if Daylight Savings time is in effect
  boolean NOLIM; //using Limits?
  boolean ERRFLAG; // error variable flag
  boolean PFLAG;  // Precession flag
  boolean RFLAG;  // True if Refraction calculations enabled
  boolean MotorDriverflag;
  boolean OLEDflag; // OLED display used?
  boolean LCDpicflag; // PIC-based LCD display used?
  boolean LCDi2cflag; // OpenLCD I2C display used?
  boolean IRSETUPflag; // Set if IR remote has been set up
  boolean TCIFLAG; // Set if Telescope Control interface is configured
  //Note: if both LCDpicflag and LCDi2cflag are true, then an OpenLCD device is on TC_LCD
  boolean INA219flag; // INA219 I2C voltage monior used?
  char SiteID[40];
};

union EEarray {
  EEstruct strunion;
  byte ebuf[sizeof(EEstruct)];
} eecharbuf;

// 256 kbits = 32K bytes for EEPROM
extEEPROM tcEEPROM(kbits_256, 1, 64, EEPROM_ADR);

/*=======================================================
// Define stuff for compatibility with translated
// Telescope Controller code (Originally in Forth)
// Commands available in the system */
#define SETIMECMD 0x1c
#define FNDOBJECT 0x1d
#define INITCMD 0x1e
#define FNDARB 0x1f
// Stay at current telescope position
#define STAYFUNC 0x22

// User variables
long IRkey; // Record latest raw IR key code, if seen
boolean lockbtninitflag;
byte COMMAND; //What the User tells us to do
byte screenCOMMAND;
boolean screenDecRefresh;
boolean TCIFLAG; // Set to true if Telescope Control interface has been set up
boolean AUTOSTARFLAG;  // Set to true if system can offer to select alignment star
boolean INITFLAG;  // Telescope mount initialized flag
boolean TIMEFLAG;  // Time initialized flag
boolean PFLAG;  // Precession flag
boolean RFLAG;  // True if Refraction calculations enabled
boolean DSTFLAG;  // Set to true if Daylight Savings time is in effect
int TZONE;  // System Time Zone offset from GMT

byte inputstrlen;

boolean NOLIM; //using Limits?
long RAAZ;  // Current encoder counts in RA/Azimuth ( 3 bytes each )
long DECAL;  // Current encoder counts in Declination/Altitude
double TRA;  // Target RA in hours
double TDEC;  // Target Declination in degrees
double previousTRA, previousTDEC;
double TAZIMUTH;
double TALTITUDE;
long LATEMP;
double FALTEMP;  // store floating # altitude
int GRYEAR;
byte GRMONTH, GRDAY;
long TIMER; //Double seconds time count
int rtcseconds, rtcmin, rtchours;
int hmsseconds, hmsmin, hmshours;
char siteid[70];
double FLONGITUDE, FLATITUDE;  // degrees
double FAZIMUTH, FALTITUDE;
double FRA, FDEC;
boolean ERRFLAG;  // error variable flag
double FHEIGHT;  // FP elevation in feet
double FTEMPF;  // Temperature in Farenheit for refraction
double FHUMID; // Percent relative humidity
double FPINHG;  // Barometric Pressure in Inches of Mercury
double FELAT;  // Equatorial Mount Latitude
double FMAGHDG; // Magnetic Compass heading
float busvolts; // Bus voltage as measured by INA219

const byte MONTHDAYS[] = { // Number of days in each month of the year
0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// need to make auto star select table

const int ISTARS[] = {
98  , /* Aldebaran */         492 , /* Vega */
113 , /* Rigel */             529 , /* Altair */
114 , /* Capella */           547 , /* Deneb */
140 , /* Betelgeuse */        595 , /* Markab */
162 , /* Sirius */            43  , /* Hamal */
190 , /* Procyon */           30  , /* Achernar */
196 , /* Pollux */            47  , /* Mira */
188 , /* Castor */            62  , /* Algol */
254 , /* Regulus */           155 , /* Canopus */
350 , /* Arcturus */          330 , /* Mizar */
422 , /* Antares */           592   /* Fomalhaut */
};

const byte NISTARS = 22; // number of stars in table

const char hitkey[] = "Hit Key to continue";

// define IR receive pin - for IR Remote Library routines
int RECV_PIN = IR_RECVpin;
decode_results results;
boolean irsetup, IRFLAG;
char irkeytab[20];

// Library specific variable defines
Uart Serial2 (&sercom2, 3, 2, SERCOM_RX_PAD_1, UART_TX_PAD_2); // For second UART port
Encoder RAAZenc(RAAZ_pinA, RAAZ_pinB); //Incremental Quadrature Encoder for RA/Aximuth
Encoder DECALenc(DECAL_pinA, DECAL_pinB); //Incremental Quadrature Encoder for Dec/Altitude
DobsonianControl myScope;
IRrecv irrecv(RECV_PIN); //IR detector - Used for TV Remote input
RTCZero rtczero; // Sparkfun Redboard Turbo internal Real Time Clock
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration
BME280 bme280; //Temperature, Barometric Pressure, Humidity Sensor
boolean BMEpresent;
SFE_UBLOX_GNSS ubloxGPS;
boolean GPSpresent; //U-blox based GPS
Adafruit_INA219 ina219; // Bus voltage monitor
boolean I2CEEPROMpresent, MagCompasspresent, RFD77402present;
boolean rockerTiltPresent, tubeTiltPresent;
SCL3300 rockerTilt, tubeTilt; // inclinometers
unsigned int distance = 0;
SiderealPlanets myAstro;
SiderealObjects myObjects;

//===============================================
// need to declare all functions - whatever
// TCVCIOFP
char KEY();
void EMIT(char n);
long GETnum(char* buf);
void LCDline1();
void LCDline2();
void LCDline3();
void LCDline4();
void LCDclear();
void LCDprint(int tmp);
void LCDprint(double tmp, int frac);
void LCDprint(char tmp);
void LCDprint(char* tmp);
void LCDprint(const char* tmp);
void SETINITFLAG();
void RESETINITFLAG();
boolean NOTINITquestion();
void SETIMEFLAG();
void RESETIMEFLAG();
boolean NOTIMESETquestion();
boolean INITCOMMANDquestion();
boolean TIMECOMMANDquestion();
void SETIRFLAG();
void RESETIRFLAG();
boolean IRquestion();
void SETTCIFLAG();
void RESETTCIFLAG();
boolean TCIquestion();
void SETPRECESSFLAG();
void RESETPRECESSFLAG();
boolean PRECESSquestion();
void SETREFRACTFLAG();
void RESETREFRACTFLAG();
boolean REFRACTquestion();
void SETERRFLAG();
void RESETERRFLAG();
int ACCEPT(char *buf, int limit);
boolean RDLBTN(int n);
void alarmMatch();
void TERMLINEUP();
void TERMclear();
void TERMcursor();
void TERMxy(int x, int y);
void TERMtextcolor( char buf );
void WAITASEC(int n);
void newdelay( long interval );
boolean CHKNUM();

// TCDERVIO
void printDegMinSecs(double n);
void printTime(double n);
void print2digitsUSB(int number);
boolean TRPLCMP( int n1, int n2, int n3, int n4 );
boolean GETYORN();
long GETDDECNUM();
int GETINUM();
int GETINUM(int n);
long CONVHMS(char *buf);
long INPUTHMS();
double GETFDECNUM();

// TCUSERIO
void oledprintData();
void print2digits(int number);
void printstatusscreen();
void SHWDT();
void printObject();
void updatestatusscreen();
void TERMALLOPTIONS();
void PRPLANET( int n );

// TCINIT
void SERCOM2_Handler();
void inithardware();
void RESETIME();
void STRTturboCLK();
void INITZONE();
void RESETIR();
void RESETTCI();
void tcintro();
void TC_main();
#endif
