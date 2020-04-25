// Telescope Controller 4.0 - Header file

#ifndef TC4_0_H
#define TC4_0_H
const char tcversion[] = "4.00.00";
//==========================
// Arduino SAMD Board Pin Definitions
const int BLUE_LED = 13; // Blue "stat" LED on pin 13
const int RX_LED = PIN_LED_RXL; // RX LED on pin 25, use the predefined PIN_LED_RXL to make sure
const int TX_LED = PIN_LED_TXL; // TX LED on pin 26, use the predefined PIN_LED_TXL to make sure

// Define Encoder Pins - Must have this if nothing else
const int RAAZ_pinA = 15; // A1 - Azimuth Encoder
const int RAAZ_pinB = 16; // A2 
const int DECAL_pinA = 17; // A3 - Altitude Encoder
const int DECAL_pinB = 18; // A4
// SPI used for optional Murata SCA3300 inclinometer
// Use ICSP pins here
const int SPI_SS1 = 10; // D10 - Rocker Base inclinometer sensor select

// Pins used for optional Reference sensors
const int AZREFsensor = 4; /* D4 - Azimuth axis reference point */
const int HORIZONlim = 5; /* D5 - Altitude axis Horizon reference point */
//const int ZENITHlim = 6; /* D6 - Altitude axis Zenith point */
//const int EQUATORlim = 9; /* D9 - Equatorial mount Right Ascension axis Meridian Reference */
// IR Detector Pin - Option to use TV/DVD IR Remote as a handbox control
const int IR_RECVpin = 11; /* D11 - IR Remote sensor */

// Real Hardware button for forced re-init, position Lock, etc.
const int LOCKBTN = 19; /* A5 */
//The OLED library assumes a reset pin is necessary.
//The Qwiic OLED has RST hard-wired, so pick an IO pin that is not being used
const int PIN_RESET = 44; //This is for the Neo-Pixel LED, which isn't used

//==================================
// I2C Addresses used
// I2C Magnetic Compass (HMC6352 address 0x21) (HMC6343 address 0x19)
// I2C OLED display (IIC address 0x3c)
// I2C Voltage/Current sensor (IIC address 0x40)
// I2C GPS receiver module (IIC address 0x42)
// IIC address of EEPROM - 24LC256 or 24C512C-L
const int EEPROM_ADR = 0x50;
// Motor Driver config pattern "1000" on board for address 0x5D
const int MotorDriver_ADR = 0x5D; //Qwiic Motor Driver
// DS1307 I2C 5v RTC (IIC address 0x68)
const int DS1307_ADR = 0x68;
const int LCDi2c_ADR = 0x72; //Default for Sparkfun OpenLCD
// BME280 Temperature, Barometric Pressure, Relative Humidity Sensoe I2C address
const int BME280_ADR = 0x77;
//This OLED definition needed - The DC_JUMPER is the I2C Address Select jumper. 
//Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 1 

//====================================================
// Libraries!  This is why I use Arduino, seriously.
#include "wiring_private.h" // pinPeripheral() function for Serial2 UART
#include <Arduino.h>
#include <stdint.h>
#include <string.h>
#include <RTCZero.h> //SAMD Real Time Clock https://www.arduino.cc/en/Reference/RTC
#include <math.h> //Oh yeah.....
// This optional setting causes Encoder to use more optimized code
// It must be defined before Encoder.h is included.
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h> //Optical Quadrature Encoders https://www.pjrc.com/teensy/td_libs_Encoder.html
#include "pins_arduino.h"
#include <Wire.h>  // Include Wire if you're using I2C
#include "SCMD.h" // Serial Controlled Motor Driver https://github.com/sparkfun/Serial_Controlled_Motor_Driver
#include "SCMD_config.h" //Contains #defines for common SCMD register names and values
#include "extEEPROM.h" // I2C EEPROM https://github.com/PaoloP74/extEEPROM
#include <SerLCD.h> //Serial LCD library: http://librarymanager/All#SparkFun_SerLCD
#include <SFE_MicroOLED.h>  // SFE_MicroOLED library https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library
#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
#include <Adafruit_INA219.h> //Power/current/voltage monitor https://github.com/adafruit/Adafruit_INA219
#include "SparkFunBME280.h" //Temperature, Pressure, Humidity sensor https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
#include <HMC6352.h> // https://github.com/funflin/HMC6352-Arduino-Library
#include "SFE_HMC6343.h" // https://github.com/sparkfun/SparkFun_HMC6343_Arduino_Library
#include <IRremote.h> //Use TV/DVD IR remote https://github.com/z3t0/Arduino-IRremote
#include <SPI.h> //Need for Murata inclinometer sensor
#include <SCL3300.h> //Oooo, where did this come from?!  :-)
#include <SparkFunDS1307RTC.h> // https://github.com/sparkfun/SparkFun_DS1307_RTC_Arduino_Library

//=========================================
// EEPROM definitions
const int MAXUCOORD = 200; // maximum number of User Defined Coordinates
struct EEstruct {
  double dummy;
  double FLATITUDE;
  double FLONGITUDE;
  float DCEW;
  float DRHO;
  float DTEMPF; // default Farenheit temperature
  float DTZONE; // Default Time zone offset from GMT
  // IR decode Table - 20 chars match one of 20 long numbers
  long IRinput[32]; // Raw IR remote button numbers
  char IRchar[32]; // Corresponding ASCII char for each number
  // User defined coordinates
  long UCOORD[MAXUCOORD * 2]; // save space for user defined objects
  long DAZOFF; // Azimuth Offset
  long RRAAZ; //Default encoder range for RA/Azimuth
  long RDECAL; //Default encoder range for Declination/Altitude
  int BFLAGS; // Boot flags
  int OPTIONS; // One Button Telescope hardware options flags
  int ELEVATION; // default elevation
  int EEchk;
  boolean DSTFLAG; // Set to true if Daylight Savings time is in effect
  boolean EQUflag; // True for Equatorial mount, false for Alt-Az mount
  boolean EMOUNTFLAG;  // Set to true for Fork mount, false for German mount
  boolean NOLIM; //using Limits?
  boolean EQFLAG; //Cel EQ Hunt flag
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
  boolean RTC1307flag;
  char SiteID[40];
};

union EEarray {
  EEstruct strunion;
  byte ebuf[sizeof(EEstruct)];
} eecharbuf;

// 256 kbits = 32K bytes for EEPROM
extEEPROM tcEEPROM(kbits_256, 1, 64, EEPROM_ADR);

//=======================================================
// Object data tables
#include "TCobjects.h"
//=======================================================
// Define stuff for compatibility with translated
// Telescope Controller code (Originally in Forth)
#define FALSEFLAG 0
#define TRUEFLAG 1
const int BACKSPACE = 8;
const int ENTER = 0x0d;
const int NSTARS = 609; // Number of stars in table
const int NGCNUM = 7840; // Number of NGC objects
const int ICNUM = 5386; // Number of IC objects
// Offset to Hershel objects table from Messier table
const int HERSHELOFF = 438;

// Commands available in the system
#define SHWSTAR 0x0a
// Show non-stellar objects
#define SHWMESS 0x0b
#define SHWTIME 0x0c
// Show offset to target, identify object, and show current position
#define SHWHERE 0x0d
// Show initialization parameters
#define SHWPARM 0x0e
// Show arbitrary position
#define SHWARB 0x0f
#define FNDSUN 0x10
#define FNDMOON 0x13
// This was to find Pluto, but now it's just the SCAN function (?)
#define FNDPLTO 0x19
#define FNDSTAR 0x1a
// Find non-stellar object
#define FNDMESS 0x1b
#define SETIMECMD 0x1c
// Set to handbox mode - Probably make Motor Rate
#define SETOHNDBOX 0x1d
#define INITCMD 0x1e
#define FNDARB 0x1f
// Status display - Time, Temp, RH, Battery
#define STATUSDISP 0x20
// Scan function
#define SCANFUNC 0x21
// Stay at current telescope position
#define STAYFUNC 0x22
// Close telescope - Not currently used
#define CLSCOPE 0x2e


const double F2PI = 2.0 * M_PI;
const double FPI  = M_PI;
const double FPIdiv2 = M_PI_2;
const double FminusPIdiv2 = -M_PI_2;
const double FPIdiv4 = M_PI_4;
// 2PI/24HR is really 2PI/seconds-in-a-24-hour-day
const double F2PIdiv24HR = 7.272205217E-5;
const double F24HRdiv2PI = 13750.9871;
// 2PI/360 is really 2PI/seconds-in-360-degrees
const double F2PIdiv360 = 4.8481368111E-6;
const double F360div2PI = 206264.8062471;
const double F2to16 = 65536.0;
// #define F2to16minus1   65535.0
const double F2to15 = 32768.0;
const double F2to15minus1 = 32767.0;
const double F60 = 60.0;
const double F30 = 30.0;
const double F24 = 24.0;
const double F15 = 15.0;
const double F1_0 = 1.0;
const double F0_5 = 0.5;
const double F0 = 0.0;
const double Fminus1 = -1.0;
const double F3_28084 = 3.28084;
const double F86400 = 86400.0;
const double F324000 = 324000.0;
const double F648000 = 648000.0;
const double F1296000 = 1296000.0;
// SMP coefficients
const double F36525 = 36525.0;
const double F51544_5 = 51544.5;
// Precession coefficients
const double F51544 = 51544.0;
const double F2_668Eminus7 = 2.668E-7;
const double F6_112Eminus7 = 6.112E-7;
const double F2_655Eminus7 = 2.655E-7;
// Refraction Coefficients
const double F17 = 17.0;
const double F460 = 460.0;
#define F.087266463       0.087266463
const double F0_98829789 = 0.98829789;
#define F979.642664       979.642664
#define F14823.05         14823.05
#define Fminus0.53520501  -0.53520501
#define Fminus0.107041    -0.107041
#define F29.9318          29.9318
// Find GST coefficients
#define F24110.54841      24110.54841
#define F8640184.812866   8640184.812866
const double F0_093104 = 0.093104;
#define Fminus6.2Eminus6  -6.2E-6 

// Lunar Parallax correction coefficients
const double F60_40974 = 60.40974;
// #define F.996647         0.996647 
const double F1_5 = 1.5; // Constant for decoding mag
// Constant for 2 star initialization = 7.29211652e-5
const double FCONVK = 7.29211652e-5;

// User variables
long IRkey; // Record latest raw IR key code, if seen
boolean lockbtninitflag;
byte COMMAND; //What the User tells us to do
boolean GPSINITFLAG;  // Set to true if GPS was used to init system
boolean TCIFLAG; // Set to true if Telescope Control interface has been set up
boolean AUTOSTARFLAG;  // Set to true if system can offer to select alignment star
boolean MOUNTFLAG;  // Set if at NCP already
boolean EQUflag; // True for Equatorial mount, false for Alt-Az mount
// flag to set for German Equatorial Mount vs Fork mount
boolean EMOUNTFLAG;  // Set to true for Fork mount, false for German mount
boolean INITFLAG;  // Telescope mount initialized flag
boolean TIMEFLAG;  // Time initialized flag
boolean PFLAG;  // Precession flag
boolean RFLAG;  // True if Refraction calculations enabled
boolean DSTFLAG;  // Set to true if Daylight Savings time is in effect
int TZONE;  // System Time Zone offset from GMT
// Keeps track of what value to display - ie az/al/ra/dec
byte DISPLAYCOUNTER;
byte inputstrlen;
// The following SERies variables are used in the SMP routines
byte SERTERM;
byte SERSP;
// 2 bytes each
int SERISP;
int SERADDR;
boolean NOLIM; //using Limits?
boolean EQFLAG; //Cel EQ Hunt flag
// TURNAZ is the flag that keeps the telescope
// from moving more than 360 degrees in azimuth
// TURNAL does a similar thing in altitude
byte TURNAZ, TURNAL;
long MRAAZ;  // Maximum encoder counts in RA/Azimuth ( 3 bytes each )
long MDECAL;  // Maximum encoder counts in Declination/Altitude
long RAAZ;  // Current encoder counts in RA/Azimuth ( 3 bytes each )
long DECAL;  // Current encoder counts in Declination/Altitude
long TRAAZ;  // Target encoder counts in RA/Azimuth ( 3 bytes each )
long TDECAL;  // Target encoder counts in Declination/Altitude
long TRA;  // Target RA in arcseconds
long TDEC;  // Target Declination in arcseconds
long TAZ;  // raw actual target encoder counts - Azimuth
long TAL;  // Target Altitude encoder counts
long TMPAZOFF;  // temp az offset storage
long LATEMP;
double FALTEMP;  // store floating # altitude
int GBYEAR;
int GRYEAR;
byte GBMONTH;  // 1 byte )
byte GRMONTH;  // 1 byte )
byte GBDAY;  // 1 byte )
byte GRDAY;  // 1 byte )
long AZOFF;  // 3 bytes )
long TIMER; //Double seconds time count
long LT;
long LST;
long UT;
int rtcseconds, rtcmin, rtchours;
int hmsseconds, hmsmin, hmshours;
char siteid[70];
double FLONGITUDE;  // FP format each
double FLATITUDE;  // stored in radians
double FSLAT;
double FCLAT;
// These added for fast tracking computations
double FTDEC;
double FTSDEC;
double FSLstarSD;
double FCLstarCD;
double FRA;
double FDEC;
double FDAYS;
double FREFRACTCO;
double FSMALLT;
double FLARGET;
double FL;
double FU;
double FV;
double FW;
double SERFNUM;
double FRHO;  // variable for non-perpendicular axes
double FCEW;  // collimation error
boolean ERRFLAG;  // error variable flag
double FHEIGHT;  // FP elevation in feet
double FTEMPF;  // Temperature in Farenheit for refraction
double FHUMID; // Percent relative humidity
double FPINHG;  // Barometric Pressure in Inches of Mercury
double FELAT;  // Equatorial Mount Latitude
double FMAGHDG; // Magnetic Compass heading
double FESLAT;
double FECLAT;
// New USER vars for extra features
double FAUTOOFF;  // floating offset for autostar
byte TRGOBJTYPE;  // 1 byte
int TRGOBJNUM;  // 2 bytes
int TRGOBJADDR;  // 2 bytes
byte TMPOBJTYPE;  // 1 byte
int TMPOBJNUM;  // 2 bytes
long STARTIME;  // 4 bytes to store star observed time in
int ITSTARno;  // 2 bytes for # of init star
int ITGOTSTARFLAG;  // 2 bytes for flag - Need init star? 
double FTMP3RDV;  // Floating temp for 3RDV
int GPSDECODED;  // 2 bytes to keep track of vars set
double FY01TMP;  // Floating temp for Y01
double FAZOFF;  // Floating temp for Azimuth Offset
double FDETV;  // Floating temp DETV
int IDOBJTYPE;  // 2 byte Identify Object Type
int IDOBJADDR;  // 2 byte Identify Object Addr
int SCANOBJno;  // 2 byte Scan Obj#
boolean BESTFLAG;  // 1 byte flag if we will use Best table
int SCANTAB;  // 1 byte for scan table selected
int SMPDONE;  // 2 byte var to keep track of calculations
long SMPARRra[10];  //10*4 bytes for storing SMP coords calcs
long SMPARRdec[10];  //10*4 bytes for storing SMP coords calcs
float busvolts; // Bus voltage as measured by INA219
uint16_t spi_whoami; // Inclinometer ID

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
IRrecv irrecv(RECV_PIN); //IR detector - Used for TV Remote input
RTCZero rtczero; // Sparkfun Redboard Turbo internal Real Time Clock
SCMD i2cMotorDriver; //Serial Controlled Motor Driver
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration
BME280 bme280; //Temperature, Barometric Pressure, Humidity Sensor
boolean BMEpresent;
SFE_UBLOX_GPS ubloxGPS;
boolean GPSpresent; //U-blox based GPS
SFE_HMC6343 HMC6343; // Declare the compass object
Adafruit_INA219 ina219; // Bus voltage monitor
boolean I2CEEPROMpresent, MagCompasspresent, HMC6343MagCompasspresent;
//DS1307 rtc1307; <== defined as 'rtc' in library
boolean RTC1307present;
SCL3300 inclinometer;

//===============================================
// need to declare all functions - whatever
// TCVCIOFP
int FtoS(double f);
long FtoD(double f);
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
boolean USERRquestion();
boolean FORKquestion();
int ACCEPT(char *buf, int limit);
boolean HSIG();
boolean EQquestion();
void ZEROAZ(long Dzraaz);
void RDAZAL();
long ITAZAL(long Drraaz, long Drdecal, boolean eqflag);
boolean KYSTAT();
int GETZONE();
boolean RDLBTN(int n);
void SETIME(long dtime);
long RDTIME();
void alarmMatch();
void TERMLINEUP();
void TERMclear();
void TERMcursor();
void TERMxy(int x, int y);
void TERMtextcolor( char buf );
void WAITASEC(int n);
void newdelay( long interval );
boolean CHKNUM();
void Fdot(double f);
int ds1307_read( int address );
void ds1307_write( int address, int value );
/* EEPROM reads/writes done by structure now
byte CatCLK(int eeaddress);
int atCLK(int eeaddress);
void CstoreCLK(byte data, int eeAddress);
void storeCLK(int data, int eeAddress);
long threeBatCLK(int eeaddress);
void threeBstoreCLK(long data, int eeAddress);
*/

// TCDERVIO
void twoCR();
long GETIME( long* addr );
void TIME( long* addr );
void DATEFIX();
int UTYEAR();
byte UTMONTH();
byte UTDAY();
void DISTIME( long* n );
void FDISPLAYDMS( double f );
boolean TRPLCMP( int n1, int n2, int n3, int n4 );
boolean GETYORN();
boolean HUNTquestion( int n );
boolean DEFAULTquestion( int n );
boolean SKIPquestion( int n );
boolean INPUTquestion( int n );
long GETDDECNUM();
int GETINUM();
int GETINUM(int n);
long CONVHMS(char *buf);
long INPUTHMS();
double GETFDECNUM();
void readsensors();

// TCMATH
double RAAZtoF2PIRAD(long d);
double DECALtoFPIdiv2RAD( long d );
long F2PIRADtoRAAZ( double f );
long FPIdiv2RADtoDECAL( double f );
void T2toT3( long ra, long decal, long* Dra, long* Ddec );
void T3toT2( long Dra, long Ddec, long* ra, long* decal );
void atUCOORD( int coordnum, long* Dra, long* Ddec );
void storeUCOORD( long Dra, long Ddec, long coordnum );
void atSMPCOORD( int coordnum, long* Dra, long* Ddec );
void storeSMPCOORD( long Dra, long Ddec, int coordnum );
double FALplusRG( double f1, double f2 );
double FALminusRG( double f1, double f2 );
double INRNG( double f );
double SINRNG( double f );
double FdotUT();
long MJD();
double FMJD();
void POFFSET( long Dra, long Ddec, double fmjd, double* fradif, double* fdecdif );
void PRECESS( long Dra, long Ddec, long* Dpra, long* Dpdec );
void PRECESSTO2000( long Dra, long Ddec, int epoch, long* Dpra, long* Dpdec );
void PRECESSION( long Dra, long Ddec, int epoch, long* Dpra, long* Dpdec );
void PRECESSIONTAB( long* Dra, long* Ddec );
void ROFFSET( long Dal, double* fal, double* faldif );
long ANTIREFRACT( long Dral );
void REFRACTION( long Dal, long*  Dral, boolean *alflag );
void ERROROFF( double faz, double fal, double* rfaz, double* paz, double* rfal, double* pal );
void ERRORADJ( double faz, double fal, double* fnaz, double* fnal );
void ANTIERRORADJ( double fnaz, double fnal, double* faz, double* fal );
void TRKSETUP();
void TRKCON( double fha, double* faz, double* fdal );
void ACON( double fazha, double faldec, double *fhaaz, double *fdecal );
void ECON( double fazha, double faldec, double *fhaaz, double *fdecal );
double FHARADminus( double frad );
double RAtoFHARAD( long Dra );
long FHARADtoRA( double fharad );
void RAdivDECtoAZdivAL( long Dra, long Ddec, long* Daz, long* Dal );
void TRACKtoAZdivAL( long* Daz, long* Dal );
void AZdivALtoRAdivDEC( long Daz, long Dal, long* Dra, long* Ddec );
void EQC ( double* fha, double* fdec );
int MAKAZDIR( long Dtazha, long Dazha );

// TCUSERIO
void oledprintData();
void print2digits(int number);
void print2digitsUSB(int number);
void printstatusscreen();
void SHWDT();
void updatestatusscreen();
int GETSOURCE();
void SRCMSG();
void KEYMSG();
void TIMEMSG();
void PSAVEMSG();
void JCOORDMSG();
void TERMCOORDMSG();
void DEGMSG();
void LONGMSG();
void COUNTMSG();
void TZMSG();
void REDOMSG();
void BUTTONMSG();
void UCOORDMSG();
void TERMALLOPTIONS();
boolean TRACKINGCOMMANDquestion( int n );
void PRPLANET( int n );
void PRINTITEM( int n );


// TCINIT
void SERCOM2_Handler();
void inithardware();
void RESETIME();
long CORRECTIME ( long d );
void STRTturboCLK();
void DDATE();
void PUTIME( long Dtime );
void DTIME();
void SETRTC();
void STRHGT( double ft, double fel );
void DREFRACT();
void DERR();
void GETUTDATE();
boolean INITUTDATE();
void INITZONE();
void GETUT();
void GETLT();
void INITUTandLT (boolean flag);

void RESETIR();
void RESETTCI();

// TCTOP - Only need two defines here
void tcintro();
void TC_main();
#endif
