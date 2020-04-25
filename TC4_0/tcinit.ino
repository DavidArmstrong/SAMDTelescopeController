// TCINIT
// Telescope Controller 4.0 - Initialization functions
#include "tcheader.h"
void SERCOM2_Handler()
{
  Serial2.IrqHandler();
}

void inithardware() { //Set up all the hardware interfaces
  // Read Hardware button for forced re-init, position Lock, etc.
  pinMode(LOCKBTN, INPUT_PULLUP);
  lockbtninitflag = RDLBTN(); //Get button state, first thing
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  IRkey = 0L;
  // Brute force default initialization
  // We will replace the defaults if valid data is in an EEPROM
  eecharbuf.strunion.EEchk = 0; // Check for existence of EEPROM
  eecharbuf.strunion.DSTFLAG = false; // Daylight Savings used
  eecharbuf.strunion.EQUflag = false; // EQUflag
  eecharbuf.strunion.EMOUNTFLAG = false; // EMOUNTflag
  eecharbuf.strunion.NOLIM = false; // Using Limit/reference sensors?
  eecharbuf.strunion.EQFLAG = false; // Celestial equator hunt flag
  eecharbuf.strunion.ERRFLAG = false; // Errors flag
  eecharbuf.strunion.PFLAG = false; // Precession flag
  eecharbuf.strunion.RFLAG = false; // Refraction flag
  eecharbuf.strunion.MotorDriverflag = false; // Motor driver flag
  eecharbuf.strunion.OLEDflag = true; // OLED display used?
  eecharbuf.strunion.LCDpicflag = false; // PIC-based LCD on Serial used?
  eecharbuf.strunion.LCDi2cflag = false; // LCD on IIC?
  eecharbuf.strunion.INA219flag = false; // INA219 used?
  eecharbuf.strunion.RTC1307flag = false; // RTC1307 used?
  eecharbuf.strunion.IRSETUPflag = irsetup = false; // IR remote in use
  eecharbuf.strunion.TCIFLAG = TCIFLAG = false; // Telescope Control Interface set up
  eecharbuf.strunion.BFLAGS = 0; // Boot flags
  eecharbuf.strunion.ELEVATION = 5500; // Elevation in feet
  eecharbuf.strunion.DAZOFF = 0L; // Azimuth Offset
  eecharbuf.strunion.RRAAZ = 10000L; // Azimuth encoder range
  eecharbuf.strunion.RDECAL = 4000L; // Altitude encoder range
  eecharbuf.strunion.OPTIONS = 0; // Options flags
  eecharbuf.strunion.DCEW = 0.0;
  eecharbuf.strunion.DRHO = 0.0; 
  eecharbuf.strunion.DTEMPF = 68.0; // Default temp
  eecharbuf.strunion.DTZONE = -8.0; // Default Time Zone (PST)
  eecharbuf.strunion.FLATITUDE = 1.; // Latitude in radians
  eecharbuf.strunion.FLONGITUDE = -1.; // Longitude in radians, West is negative
  strncpy ( eecharbuf.strunion.SiteID, "Demonstration mode", sizeof("Demonstration mode") );
  // Set up default values for Demo mode
  TZONE = eecharbuf.strunion.DTZONE;
  DSTFLAG = eecharbuf.strunion.DSTFLAG;
  MRAAZ = eecharbuf.strunion.RRAAZ;
  MDECAL = eecharbuf.strunion.RDECAL;
  TMPAZOFF = eecharbuf.strunion.DAZOFF;

  // verify presence of I2C EEPROM
  byte i2cStat = tcEEPROM.begin();
  I2CEEPROMpresent = true;
  if ( i2cStat != 0 ) {
    I2CEEPROMpresent = false;
    //lockbtninitflag = true; //Force demo mode
  } else { // /*
    // Read in EEPROM values
    int testbyte = tcEEPROM.read(0L); // read first byte in EEPROM
    if (testbyte == 0x55) {
      // Good data there, so read in the rest of the array
      int eestat = tcEEPROM.read(2L, eecharbuf.ebuf, sizeof(EEstruct));
      if (eestat == 0) {
        TZONE = eecharbuf.strunion.DTZONE;
        DSTFLAG = eecharbuf.strunion.DSTFLAG;
        irsetup = eecharbuf.strunion.IRSETUPflag;
        TCIFLAG = eecharbuf.strunion.TCIFLAG;
        MRAAZ = eecharbuf.strunion.RRAAZ;
        MDECAL = eecharbuf.strunion.RDECAL;
        TMPAZOFF = eecharbuf.strunion.DAZOFF;
      }
    }
    eecharbuf.strunion.EEchk = 0x55; // */
  }
  
  if (eecharbuf.strunion.LCDi2cflag || eecharbuf.strunion.LCDpicflag || eecharbuf.strunion.OLEDflag) {
    LCDclear(); // Only clear if we are using LCD display
    newdelay(500);
    LCDline1();
    LCDprint("TC ");
    LCDprint(tcversion);
    LCDprint("    ");
  }
  if (I2CEEPROMpresent) TCterminal.println("I2C EEPROM detected.");

  if (eecharbuf.strunion.INA219flag) ina219.begin(); // Voltage monitor
  if (eecharbuf.strunion.OLEDflag) {
    oled.begin();
    oled.clear(ALL);
    oled.display();
  }

  byte bme280_id = bme280.begin(); //Returns ID of 0x60 if successful
  if (bme280_id != 0x60) { // Problem with BME280
    BMEpresent = false;
  } else { // BME280 online
    BMEpresent = true;
    TCterminal.println("BME280 Barometric Pressure and temperature sensor detected.");
  }

  if (inclinometer.begin(SPI_SS1)) TCterminal.println("Murata SCL3300 inclinometer detected.");

  // Check for the presence of a DS1307 RTC chip
  RTC1307present = false;
  rtc.begin(); // Call to initialize the library
  if (ds1307_read(8) != 0x55) {
    ds1307_write(8, 0x66);
    if (ds1307_read(8) == 0x66) {
      TCterminal.println("DS1307 RTC detected and set with default time.");
      rtc.autoTime(); // Set the clock to something reasonable
      ds1307_write(8, 0x55);
      RTC1307present = true;
    }
  } else if (ds1307_read(8) == 0x55) {
    TCterminal.println("DS1307 RTC detected.");
    RTC1307present = true;
  }

  newdelay(100);
  if ((ubloxGPS.begin() == false) || !I2CEEPROMpresent) {
	// We have to add in check for I2CEEPROMpresent, because if we don't
	// a bare Redboard Turbo will end up getting a false positive for GPS present check.
	// This means that to use GPS, the system MUST have an I2C EEPROM present.
    //Ublox GPS not detected at default I2C address
    GPSpresent = false;
    if (RTC1307present == true) {
      //Get time from DS1307 then
      GRYEAR = GBYEAR = rtc.year();
      GBMONTH = rtc.month();
      GBDAY = rtc.date();
      rtcseconds = rtc.second();
      rtchours = rtc.hour();
      rtcmin = rtc.minute();
      SETIMEFLAG(); // time is set correctly
    } else {
      // Assume Demo mode then
      GRYEAR = GBYEAR = 2020;
      GBMONTH = 1;
      GBDAY = 1;
      rtcseconds = 0;
      rtchours = rtcmin = 1;
      SETIMEFLAG(); // Fake it!
    }
    FALTEMP = (double)eecharbuf.strunion.ELEVATION; //Elevation in feet
    FLATITUDE = eecharbuf.strunion.FLATITUDE = 40.091125 * 3600. * F2PIdiv360;
    FLONGITUDE = eecharbuf.strunion.FLONGITUDE = -105.18512 * 3600. * FPI / 648000.;
    GPSINITFLAG = false;
  } else { // GPS online
    GPSpresent = true;
	  TCterminal.println("U-blox based GPS detected.");
	  TCterminal.println("Waiting to sync with GPS...");
    ubloxGPS.setAutoPVT(true); // non-blocking mode
    ubloxGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    ubloxGPS.saveConfiguration(); //Save the current settings to flash and BBR
	  // We gotta wait until satellite sync is active
	  while (((ubloxGPS.getLatitude() == 0) && (ubloxGPS.getLongitude() == 0)) || (ubloxGPS.getSIV() == 0)) {
	    newdelay(1000); //wait one second between checks - any faster just makes noise
	  }
    GRYEAR = GBYEAR = (int)ubloxGPS.getYear();
    GBMONTH = (byte)ubloxGPS.getMonth();
    GBDAY = (byte)ubloxGPS.getDay();
    rtchours = (byte)ubloxGPS.getHour();
    rtcmin = (byte)ubloxGPS.getMinute();
    rtcseconds = ubloxGPS.getSecond();
    FALTEMP = eecharbuf.strunion.ELEVATION = ubloxGPS.getAltitude() * 3.28084 / 1000.; //Elevation in feet
    FLATITUDE = eecharbuf.strunion.FLATITUDE = ubloxGPS.getLatitude() * 3600. * F2PIdiv360 / 10000000.;
    FLONGITUDE = eecharbuf.strunion.FLONGITUDE = (ubloxGPS.getLongitude() / 10000000.) * 3600. * FPI / 648000.;
    GPSINITFLAG = true;
    SETIMEFLAG(); // time is set correctly
  }
  long Dtime = rtcseconds + (rtcmin * 60L) + (rtchours * 3600L);
  STRTturboCLK();
  PUTIME( Dtime );
  
  bme280.settings.commInterface = I2C_MODE;
  bme280.settings.I2CAddress = BME280_ADR;
  bme280.settings.runMode = 3; //Forced mode
  bme280.settings.tStandby = 0;
  bme280.settings.filter = 4;
  bme280.settings.tempOverSample = 5;
  bme280.settings.pressOverSample = 5;
  bme280.settings.humidOverSample = 5;
  newdelay(500);
  if (eecharbuf.strunion.OLEDflag) {
    oled.clear(PAGE);
  }
  newdelay(500);
  irsetup = eecharbuf.strunion.IRSETUPflag; // Is IR table defined?
  irrecv.enableIRIn(); // Start the IR receiver
  
  HMC6352.Wake();
  newdelay(20);
  FMAGHDG = HMC6352.GetHeading();
  HMC6352.Sleep();
  HMC6343MagCompasspresent = false;
  if (FMAGHDG > 360.1) { //Old Magnetic Compass not detected at default I2C address
    MagCompasspresent = false;
    // Initialize the HMC6343 and verify its physical presence
    if (HMC6343.init()) {
      TCterminal.println("HMC6343 Magnetic Compass detected.");
      HMC6343MagCompasspresent = true;
    }
  } else { // Old Magntic Compass online
    MagCompasspresent = true;
    TCterminal.println("HMC6352 Magnetic Compass detected.");
  }

  if (eecharbuf.strunion.MotorDriverflag == true) {
    TCterminal.println("Motor driver setup.");
	i2cMotorDriver.settings.commInterface = I2C_MODE;
    i2cMotorDriver.settings.I2CAddress = MotorDriver_ADR;
    //i2cMotorDriver.settings.commInterface = I2C_MODE;
    //i2cMotorDriver.settings.I2CAddress = 0x5D; //config pattern is "1000" (default) 
    while ( i2cMotorDriver.begin() != 0xA9 ) //Wait until a valid ID word is returned
    {
      TCterminal.println( "ID mismatch, trying again" );
      newdelay(500);
    }
    TCterminal.println( "ID matches 0xA9" );
    //  Check to make sure the driver is done looking for slaves before beginning
    TCterminal.print("Waiting for enumeration...");
    while ( i2cMotorDriver.ready() == false );
    TCterminal.println("Done.");
  }
  SETIRFLAG(); // Fake it that IR is already set up
  SETTCIFLAG(); // Fake it that TC Options are already set up
}

// DERR GETUTDATE INITUTDATE INITZONE )
// GETUT GETLT INITUT+LT RESETIME SMPINIT )


// INITSCOPE ELAT INITELAT INITRANGE PCM NCPMSG )
// LATSPEED LATCOM INITLATITUDE AOCOM SETAO INITAZ )
// AUTOSTAR AIMMSG AIMATSTAR LONGCOM 3LONG HUNTERR INITERR )
// 3AIM3 MUCKAZ 3AIM 3RDV DETV GETV1 GETV2 GETV3 VID 3ANG )
// DUMQ FINDQ FINDR Y01 2LATAO 2HUNTAO 2STAR GETEMP )
// REFINIT PREINIT REALINIT NEWDEF INIT GET$ GETTNUM )
// TOSSCOMMA RDGNUM GPS BINIT )

long CORRECTIME ( long d ) {
  if (d < 0L) d += 86400L;
  if (86400L < d) d -= 86400L;
  return d;
}

void STRTturboCLK() {
  // Initialize registers, and set up RTC
  rtczero.begin(); // initialize RTC on Readboard Turbo
  rtczero.setDate(UTDAY(), UTMONTH(), (uint8_t)(UTYEAR() % 100)); //Default date set
  rtczero.setTime(rtchours, rtcmin, rtcseconds); //Default time set
  rtczero.setAlarmSeconds(59);
  rtczero.enableAlarm(rtczero.MATCH_SS); // We have a counter for once per minute
  rtczero.attachInterrupt(alarmMatch); // And an ISR to update that counter
}

void DDATE() {
  GBYEAR = 2000 + rtczero.getYear();
  GBMONTH = rtczero.getMonth();
  GBDAY = rtczero.getDay();
}

void PUTIME( long Dtime ) {
  UT = Dtime;
  long tmp;
  SETIME( 0L );
  // lt=gmt+tzone
  tmp = GETZONE() * 3600L; // TZONE to seconds first
  tmp += Dtime;
  LT = CORRECTIME(tmp);
}

void DTIME() {
  long tmp;
  // Inhibit updates ?
  tmp = rtczero.getHours() * 60L; // Get hours
  tmp = (tmp + rtczero.getMinutes()) * 60L; // Get minutes
  tmp += rtczero.getSeconds(); // Seconds
  PUTIME(tmp);
}

void SETRTC() { // set RTC here
  // Inhibit updates ?
  int days;
  byte flags;
  TIME(&UT, &days, &rtchours, &rtcmin, &rtcseconds); // sec/min/hrs
  eecharbuf.strunion.DTZONE = TZONE;
  if (RTC1307present == true) {
    rtc.setTime(rtcseconds, rtcmin, rtchours, 1, UTDAY(), UTMONTH(), (UTYEAR() % 100));
  }
if (DSTFLAG) {
  // used to subtract off extra hour before storing - now keep track of it
  //CstoreCLK(CatCLK(BFLAGS) | 16, BFLAGS); // set flag
  eecharbuf.strunion.DSTFLAG = true; // set flag
} else {
  //CstoreCLK(CatCLK(BFLAGS) & -17, BFLAGS); // reset flag
  eecharbuf.strunion.DSTFLAG = false; // reset flag
}
  STRTturboCLK();
  eecharbuf.strunion.BFLAGS |= 32; // set flag
  //CstoreCLK(CatCLK(BFLAGS) | 32, BFLAGS); // set flag
}

void STRHGT( double ft, double fel ) {
  FHEIGHT = fel;
  FPINHG = exp(fel / -27171.) * 29.9318; // result in inHg
  FREFRACTCO = FPINHG / ft;
  SETREFRACTFLAG();
}

void DREFRACT() { //Default refraction setup
  //signed byte tempf;
  //tempf = CatCLK(DTEMPF); //account for negative temps
  FTEMPF = (double)eecharbuf.strunion.DTEMPF;
  //STRHGT(FTEMPF + 460., (double)atCLK(ELEVATION));
  STRHGT(FTEMPF + 460., (double)eecharbuf.strunion.ELEVATION);
}

void DERR() {
// defaults are now in double radians, not signed arcseconds
FRHO = eecharbuf.strunion.DRHO;
FCEW = eecharbuf.strunion.DCEW;
}

void GETUTDATE() {
  int tmp = 0;
  TCterminal.println(" Input the year as in 2021");
  TCterminal.println(" or input only 2 digits, as in 19 for 2019.");
  TCterminal.println(" This is the year this instant in Greenwich, England.");
  do {
    TCterminal.print(" UTYEAR["); TCterminal.print(GBYEAR); TCterminal.print("]=");
    tmp = GETINUM();
    if (tmp == 0) tmp = GBYEAR;
    TCterminal.println("");
  } while (tmp < 0 || tmp > 2299);
  if (tmp < 999) tmp += 2000;
  GRYEAR = GBYEAR = tmp;
  TCterminal.println("");
  TCterminal.println(" Type GMT month number.  (i.e. 9 for September)");
  do {
    TCterminal.print(" UTMONTH["); TCterminal.print(GBMONTH); TCterminal.print("]=");
    tmp = GETINUM();
    if (tmp == 0) tmp = GBMONTH;
    TCterminal.println("");
  } while (tmp < 1 || tmp > 12);
  GBMONTH = tmp;
  TCterminal.println("");
  TCterminal.println(" Enter GMT day number.");
  do {
    TCterminal.print(" UTDAY["); TCterminal.print(GBDAY); TCterminal.print("]=");
    tmp = GETINUM();
    if (tmp == 0) tmp = GBDAY;
    TCterminal.println("");
  } while (tmp < 1 || tmp > 31);
  GBDAY = tmp;
  TCterminal.println("");
}

boolean INITUTDATE() {
  boolean flag;
  DDATE(); // Start by setting default date, based on GPS
  GETUTDATE(); // Modify if needed
  return true;
}

void INITZONE() {
  int tmp = 0;
  // Time Zone?
  // Start by setting default
  // TZONE = DTZONE; already done in tcintro()
  TCterminal.println(" Type time zone offset from England.");
  TCterminal.println(" i.e. EST=-5, CST=-6, MST=-7, PST=-8, etc.");
  TCterminal.println(" Don't include Daylight Savings Time here.");
  do {
    TCterminal.print(" TZONE=["); TCterminal.print(TZONE); TCterminal.print("]=");
    tmp = GETINUM(TZONE);
    TCterminal.println("");
  } while (tmp < -12 || tmp > 12);
  if (inputstrlen != 0)
    eecharbuf.strunion.DTZONE = TZONE = tmp;
  else
    eecharbuf.strunion.DTZONE = TZONE;
  TCterminal.println("");
  TCterminal.println(" Is Daylight Savings Time currently in effect?");
  TCterminal.print(" DST IN? ["); TCterminal.print(DSTFLAG); TCterminal.print("]=");
  DSTFLAG = GETYORN();
  eecharbuf.strunion.DSTFLAG = DSTFLAG;
  TCterminal.println("");
}

void GETUT() {
long d;
TIMEMSG();
TCterminal.print(" UT[");
DISTIME(&UT);
TCterminal.print("]=");
d = INPUTHMS(); TCterminal.println("");
if (inputstrlen != 0) PUTIME(d);
}

void GETLT() {
  long d;
  TIMEMSG();
  TCterminal.print(" LT[");
  DISTIME(&LT);
  TCterminal.print("]=");
  d = INPUTHMS(); TCterminal.println("");
  if (inputstrlen != 0) {
    d = d - (DSTFLAG?1:0) * 3600L;
    LT = CORRECTIME(d);
    SETIME(0L);
    // gmt=lt-tzone
    UT = CORRECTIME(LT - (GETZONE() * 3600L));
  }
}

void INITUTandLT (boolean flag) {
// Date has been updated; so Time must be updated!"
TCterminal.println(" Type 0 to input the Local time, or 1 to enter UT.");
TCterminal.print(" LT=0 UT=1 ?");
  if (GETYORN()) { //UT
    TCterminal.println("");
    GETUT();
  } else { // LT
    TCterminal.println("");
    GETLT();
  }
}

void RESETIME() {
  boolean flag;
  TERMclear();
  flag = INITUTDATE();
  INITZONE();
  INITUTandLT(flag);
//NOTINIT? 0= ( If we're here, refind LST )
//IF FINDLST<-LONG THEN
//  BCR ABCR
//  AB." Should current time parameters be saved to the RTC RAM?"
//CR ." SET RTC?" GETYORN
//IF
  SETRTC(); //This was done in SETIME, called above
//THEN
  SETIMEFLAG();
}
/*
: SMPINIT ( - )
( Modified to open the telescope while calculating, if selected )
( OPTIONS @CLK OPENBIT AND ( )
( IF ( )
(  2CR ." OPEN SCOPE" ( )
(  PIA1DRIVE C@ OPENBIT NOT AND ( Set open bit to low )
(  DUP PIA1DRIVE C! ( Save this )
(  PIA1DATA 1+ C! ( Start motor )
  ( TRUEFLAG ( Do calc regardless )
(  SMPCALCBIT OPTIONS @CLK AND ( )
( ELSE ( )
  ( Query user to do initial calculation of all SMP objects )
(  TERMIF ( )
(    2CR ." Initialize Sun, Moon and Planet coordinates?" ( )
(  THEN ( )
(  CR ." INIT SMP?" GETYORN ( )
( THEN ( )
( IF ( )
  10 0 DO
    I PRINTITEM ( n - n )
    SMPCALC ( n - Dra/Ddec )
    2DROP 2DROP
  LOOP
( THEN ( ) ;

: INITSCOPE ( Drraaz/Drdecal - )
( TERMIF ( )
  BCR ABCR
  AB." The telescope will move to the limits, "
  AB." if they exist."
  ABCR
  AB." Stand clear while moving!"
  ABCR ABCR
  AB." If no limits, use the Handbox to move "
  AB." the telescope,"
  ABCR
  AB." and the Lock button to lock in these positions:"
  ABCR ABCR
  AB." AZIMUTH STRT = Starting point to compute range"
  ABCR
   B." RA MERIDIAN = RA Starting Point"
  ABCR
  ( break here for 255 char limit )
  AB." AZIMUTH END = Same as start after once around CLOCKWISE"
  ABCR
  AB." HORIZON = Telescope level"
  ABCR
  AB." ZENITH = Telescope vertical"
  ABCR
  AB." POLE = for declination"
  ABCR
  AB." EQUATOR = for declination"
  ABCR ABCR
( to terminal, if used. )
( THEN ( )
BEGIN ( )
  CR ." READY TO GO?" GETYORN ( - flag )
UNTIL ( )
CR
2SWAP 2DUP ( - Drdecal/Drraaz/Drraaz )
( Adjust RRAAZ if not zero )
D0= 0=
IF 1. D- THEN 2SWAP ( - Drraaz/Drdecal )
EQ? IF
  ( For Equatorial mounts RDECAL is one less than max )
  2DUP ( Is it zero? )
  D0= 0=
  IF 1. D- THEN
  ELATSRC C@ HUNT?
ELSE
  0
THEN
( Call the ML routine to acutally do the initialization )
ITAZAL ( Drraaz/Drdecal/EHuntflag - Dmraaz/Dmdecal/Deqlatitude )
0 RDAZAL ( - 0/Dazimuth/Daltitude )
MTRSON ( azdir/Dtraaz/Dtdecal - ) ( stop scope here )
LATEMP L! ( Temporarily store Equatorial latitude )
( EQ? IF 2* 2* THEN ) ( 90 deg -> 360 deg ) ( does this internally )
( Adjust as appropriate to keep lsb accurate )
EQ? IF 1. D+ THEN MDECAL L!
1. D+ MRAAZ L!
TRUEFLAG R# ! ;

: ELAT ( felatitude - )
FDUP FELAT F!
FDUP FSIN FESLAT F!
FCOS FECLAT F! ;

: INITELAT ( - )
EQ? IF
  ( TERMIF ( )
    SRCMSG B." Equatorial Mount Latitude?"
  ( THEN ( )
  CR ." ELAT SRC?"
  GETSOURCE ( - n )
  DUP ELATSRC C!
  DUP INPUT?
  IF
    CR ." ELA=" INPUTHMS ( - d )
    2DUP D0< IF DMINUS THEN
    D->F F15. F/ F2PI/24HR F*
    ELAT ( equatorial latitude )
  THEN
  DEFAULT?
  IF
    DLAT 3B@CLK
    D->F F2PI/360 F*
    ELAT
  THEN
THEN ;

: INITRANGE ( - )
INITELAT ( - )
FALSEFLAG R# !
( TERMIF ( )
  SRCMSG
  B." Ranges of encoder counts in"
  ABCR
  EQ? IF
    B." Right Ascension and Declination?"
  ELSE
    B." Azimuth and Altitude?"
  THEN
( THEN ( )
CR ." RANGE SRC?"
GETSOURCE ( - n )
DUP INPUT?
IF
  ( TERMIF ( )
    BCR ABCR
    AB." Enter number of counts given by one revolution of"
    ABCR AB." the encoders in each axis."
  ( THEN ( )
  EQ? IF
    CR ." RA RNG=" GETDDECNUM
    CR ." DE RNG="
  ELSE
    CR ." AZ RNG=" GETDDECNUM
    CR ." AL RNG="
  THEN
  GETDDECNUM
(  BEGIN ( )
(    CR ." READY TO GO?" GETYORN ( - flag )
(  UNTIL ( )
  INITSCOPE ( Drraaz/Drdecal - )
THEN
DUP HUNT?
IF
  0. 0. ( let ITAZAL or INOLIM hunt for ranges )
  INITSCOPE ( Drraaz/Drdecal - )
THEN
DUP DEFAULT?
IF
  RRAAZ 3B@CLK RDECAL 3B@CLK
  INITSCOPE ( Drraaz/Drdecal - )
THEN
SKIP?
IF
  ( TERMIF ( )
    BCR ABCR
    AB." Move telescope to Limit sensors?"
    ABCR  AB." (Or if no limits, to reestablish"
    EQ? IF
      BCR
      AB." RA Meridian and Declination Celestial Pole Position.)"
    ELSE
      B."  altitude horizon.)"
    THEN
  ( THEN ( )
  CR ." TO LIMITS?" GETYORN
  IF
    MRAAZ L@ MDECAL L@
    INITSCOPE ( Drraaz/Drdecal - )
  THEN
THEN
EQ? ELATSRC C@ HUNT? AND
IF ( init equatorial latititude )
  LATEMP L@ ( retrieve measured eqlat value )
  D->F MDECAL L@ D->F F/
  F2PI F* ( measured angle in radians )
  FPI/2 FSWAP F- ( convert to latitude )
  ELAT ( felatitude - )
THEN ;

( : PCM ( - )
( CHKNUM IF DROP THEN ( get rid of possible spurious input )
( 2CR ." HIT CR " ( )
( TERMIF ( )
(  B." to transfer to PC:" ( )
( THEN ( )
( KEY CR ; ( )

: NCPMSG ( - )
( TERMIF ( )
  BCR ABCR
  AB." Use Handbox to aim at North or South Celestial Pole."
  ABCR
  AB." Press the Lock button when position is centered "
  AB." in the eyepiece."
  ABCR
( THEN ( ) ;

: LATSPEED ( - )
( Speed up computations using Latitude once it is known. )
FLATITUDE F@ FDUP
FSIN FSLAT F!
FCOS FCLAT F! ;

: LATCOM ( - n )
( TERMIF ( )
  SRCMSG
  B." Latitude?"
( THEN ( )
CR ." LAT SRC?" GETSOURCE
DUP INPUT?
IF
  ( TERMIF ( )
    BCR ABCR
    AB." Input present Latitude in -deg.mn.sc format."
    ABCR AB." If in the Southern hemisphere, make it negative."
  ( THEN ( )
  CR ." LATITUDE=" CR INPUTHMS ( - d in seconds )
  D->F FPI/2
  F* F324000. F/
  FLATITUDE F!
THEN
DUP DEFAULT?
IF
  DLAT 3B@CLK D->F F2PI/360 F*
  FLATITUDE F!
THEN ;

: INITLATITUDE ( - )
FALSEFLAG MOUNTFLAG C!
LATCOM
HUNT?
IF
  NCPMSG
  CR ." AIM AT NCP" HANDBOX ( - Daz/Dal )
  ANTIREFRACT DECAL->FPI/2RAD
  ( TERMIF ( )
    BCR ABCR
    AB." In Southern hemisphere?"
  ( THEN ( )
  CR ." SOUTHERN?" GETYORN
  IF F-1. F* THEN
  FLATITUDE F! ( - az )
  ( by coincidence, this IS azoff, so... )
  TMPAZOFF L! ( store here temporarily, was in FL )
  TRUEFLAG MOUNTFLAG C!
THEN
LATSPEED ;

: AOCOM ( - flag )
( TERMIF ( )
  SRCMSG
  B." Azimuth Offset from North?"
  ABCR
  AB." (If no Azimuth sensor exists, choose Hunt.)"
( THEN ( )
CR ." AZ OFF SRC?" GETSOURCE ( - n )
DUP DEFAULT?
IF DAZOFF 3B@CLK AZOFF L! THEN
DUP INPUT?
IF
  ( TERMIF ( )
    BCR ABCR
    AB." Enter angle separating Azimuth sensor "
    AB." from North."
    ABCR AB." Use -deg.mn.sc format."
  ( THEN ( )
  CR ." AO=" INPUTHMS ( - d )
  ( # seconds in 360 deg. )
  AZOFF L!
THEN ;

: SETAO ( - )
EQ? 0= IF
  ( Not for equatorial mounts )
  AZOFF L@ ( now convert to raw encoder counts )
  D->F MRAAZ L@ D->F F* F1296000. F/ F->D
  ZEROAZ
THEN ;

: INITAZ ( - )
( Initialize Azimuth Offset value. )
AOCOM ( - flag )
HUNT?
IF
  MOUNTFLAG C@ ( true, if already have NCP )
  IF
    TMPAZOFF L@ ( get from temporary holding place )
    AZOFF L!
  ELSE ( if not, then get it )
    NCPMSG
    CR ." AIM AT NCP"
    TERMDISPLAY? 0=
    IF WAITASEC WAITASEC THEN
    ( Leave message on LEDs for a second )
    HANDBOX ( - Daz/Dal )
    ANTIREFRACT
    2DROP AZOFF L!
  THEN
THEN
( Don't skip this, 'cause INIT only calls this )
( routine anyway when necessary. )
SETAO ;

: AUTOSTAR ( n - )
( n selects # alignment star needs, i.e. 1 or 2 )
0. TRGOBJNUM ! ( init target star number )
TRGOBJTYPE C! ( init target type = star = 0 )
FPI FAUTOOFF F! ( init object offset )
1 =
IF ( find closest star to 45 degree altitude )
  STRTAB ( - addr )
  NISTARS 0 ( # stars in table )
  DO
    DUP I ISTARS 1- 5 * + ( - tabaddr/strptr )
    DUP TRGOBJADDR !
    TAB->RA/DEC ( - tabaddr/Dra/Ddec )
    RA/DEC->AZ/AL ( - tabaddr/Dra/Ddec/Daz/Dal )
    2DUP D0<
    IF ( below horizon? )
      2DROP
    ELSE
      DECAL->FPI/2RAD ( - Dra/Ddec/Daz/fal )
      FPI/4 F- FABS FDUP FAUTOOFF F@ F< ( Compare object offset )
      IF
        I TRGOBJNUM ! ( save object # )
        FAUTOOFF F!
      ELSE
        FDROP
      THEN
    THEN
    FDROP FDROP ( - tabaddr )
  LOOP
  TRGOBJNUM @ ISTARS DUP CR ." 1 STAR=" . WAITASEC
  DUP TRGOBJNUM ! ( save object number to display later )
  0 TRGOBJTYPE C! ( stars are type 0 object )
  1- 5 * + ( - addr )
  ( now save the address for possible later ID )
  DUP TRGOBJADDR !
  TAB->RA/DEC ( - Dra/Ddec )
  RA/DEC->TARG ( Dra/Ddec - )
  DISPOFF ( star # left in TRGOBJNUM )
ELSE ( second star )
  GETCURRENTAZAL ( - Daz/Dal )
  STRTAB ( - Daz/Dal/tabaddr )
  NISTARS 0 DO
    DUP I ISTARS 1- 5 * + ( - Daz/Dal/tabaddr/strptr )
    DUP TRGOBJADDR !
    TAB->RA/DEC ( - Dcaz/Dcal/tabadr/Dra/Ddec )
    RA/DEC->AZ/AL ( - Dcaz/Dcal/tabadr/Dra/Ddec/Daz/Dal )
    2DUP D0=
    IF ( if up )
      DECAL->FPI/2RAD
      ( - Dcaz/Dcal/tabadr/Dra/Ddec/Daz/fal )
      ( fal has to be ~45deg=PI/2 elev, faz-az+-PI )
      FPI/4 F@ F- FABS ( - faldiff )
      5 PICK 5 PICK ( - faldiff/Dtaz )
      RAAZ->F2PIRAD FDUP
( - Dcaz/Dcal/tabaddr/Dra/Ddec/Daz/faldiff/ftaz/ftaz )
      20 PICK 20 PICK RAAZ->F2PIRAD FSWAP FOVER
      ( - faldiff/ftaz/fcaz/ftaz/fcaz )
      FPI/2 F+ F- FABS ( - faldiff/ftaz/fcaz/fazdif1 )
      FROT FROT FPI/2 F- F- FABS
      ( - faldiff/fazdif1/fazdif2 )
      FOVER FOVER F<
      IF FDROP ELSE FSWAP FDROP THEN
      F+ ( - fdif )
      FDUP FAUTOOFF F@ F<
      IF
        I TRGOBJNUM ! ( save star # )
        FAUTOOFF F! ( save fdif )
      ELSE
        FDROP
      THEN
      ( - Dcaz/Dcal/tabaddr/Dra/Ddec/Daz )
      FDROP FDROP ( - Dcaz/Dcal/tabaddr )
    ELSE
      FDROP FDROP
      2DROP ( get rid of 4 double #'s on stack )
    THEN
  LOOP
  >R 2DROP 2DROP R> ( - tabaddr )
  TRGOBJNUM @ ISTARS DUP CR ." 2 STAR=" . WAITASEC
  DUP TRGOBJNUM ! ( save object number to display later )
  0 TRGOBJTYPE C! ( stars are type 0 object )
  1- 5 * + ( - addr )
  ( now save the address for possible later ID )
  DUP TRGOBJADDR !
  TAB->RA/DEC
  RA/DEC->TARG ( Dra/Ddec - )
  DISPOFF ( - )
THEN ; ( target star # left in TRGOBJNUM )

: AIMMSG ( - )
( TERMIF ( )
  BCR ABCR
  AB." Use the Handbox to aim the telescope at a "
  AB." reference star.  Press the"
  ABCR
  AB." Lock button when star is centered in the "
  AB." eyepiece.  If prompted"
  ABCR
  AB." for a star number, enter one representing "
  AB." that star in the catalog."
  ABCR
( THEN ( )
CR ." AIM AT STAR"
( Leave message on LEDs for a second )
TERMDISPLAY? 0=
IF WAITASEC WAITASEC THEN ;

: AIMATSTAR ( - az/al/ra/dec )
AUTOSTARFLAG C@
IF
  ( TERMIF ( )
    BCR ABCR
    AB." Use AutoStar selection?"
  ( THEN ( )
  CR ." AUTOSTAR?" GETYORN
ELSE
  FALSEFLAG
THEN
( Do we do AUTOSTAR function? )
IF ( Yes, do it )
  ( TERMIF ( )
    BCR ABCR
    AB." Zero the offset coordinates shown below, "
    AB." then press Enter."
  ( THEN ( )
  CR ." AUTOSTAR..."
  ITSTAR# @ 0=
  IF ( First star )
    1 AUTOSTAR ( n - )
  ELSE ( Second star )
    2 AUTOSTAR ( n - )
  THEN
  AIMMSG ( - )
  HANDBOX ( - Daz/Dal )
  RDTIME ( - Daz/Dal/dtime )
  STARTIME 2! ( store time temporarily )
  ANTIREFRACT ( - Daz/Dal' )
  TRGOBJNUM @ ( - star# )
  STARCOORDS ( n - Dra/Ddec/mag )
ELSE
  ( Standard AimAtStar routine then )
  AIMMSG ( - )
  HANDBOX ( - Daz/Dal )
  RDTIME ( - Daz/Dal/dtime )
    STARTIME 2! ( store time temporarily )
  ANTIREFRACT ( - Daz/Dal' )
  GETSTAR  ( - Daz/Dal'/Dra/Ddec/mag )
THEN
DROP ( don't need mag )
2DUP TDEC L!
2OVER TRA L! TRKSETUP
3 2PICK TAL L!
4 2PICK TAZ L! ( Save it for possible double use later )
1 ITSTAR# +! ; ( Increment count of stars aimed at )

: LONGCOM ( - flag )
( TERMIF ( )
  SRCMSG B." Longitude?"
( THEN ( )
CR ." LONG SRC?" GETSOURCE ( flag - flag/n )
DUP INPUT? ( - flag1/n/flag2 )
IF ( - flag1/n )
  ( TERMIF ( )
    BCR ABCR
    AB." Enter the Longitude in -deg.mn.sc format."
    ABCR
    AB." Enter West Longitude as a negative number."
  ( THEN ( )
  LONGMSG
  INPUTHMS ( - d )
  D->F FPI F* 
  F648000. F/
  FLONGITUDE F!
THEN
DUP DEFAULT? ( - flag1/n/flag2 )
IF
  DLONG 3B@CLK D->F F2PI/360 F*
  FLONGITUDE F!
THEN ;

: 3LONG ( flag - )
HUNT? ( - flag )
IF
  ITGOTSTARFLAG @ 0=
  IF ( Use zero to say Hunt a star )
   AIMATSTAR ( Daz/Dal - Daz/Dal/Dra/Ddec )
  ELSE
   TAZ L@
   ( fix up az taken before azimuth offset was computed )
   AZOFF L@ ( - Daz/Dao )
   D- ( - Dnewtaz )
   2DUP TAZ L! ( protective programming )
   TAL L@ TRA L@ TDEC L@
  THEN
  FINDLST ( Daz/Dal/Dra/Ddec - )
  FINDLONG ( - )
ELSE
   FINDLST<-LONG
THEN ;

: HUNTERR ( - )
( Routine to hunt for error angles )
;

: INITERR ( - )
( Initialize telescope error constants )
( TERMIF ( )
  SRCMSG B." Telescope Error Constants?"
( THEN ( )
CR ." ERRORS SRC?" GETSOURCE ( - n )
DUP INPUT?
IF
  ( TERMIF ( )
    BCR ABCR
    AB." Use -deg.mn.sc format."
    ABCR ABCR
    AB." Note: Values to be saved as Defaults must be "
    AB." between +-9 deg."
    ( 2CR ) ABCR ABCR
    AB." RHO is difference in non-perpendicularity of axes.  CEW is"
    ABCR
AB." angle difference between optical and mechanical telescope axes."
  ( THEN ( )
  CR ." RHO=" INPUTHMS D->F F2PI/360 F* FRHO F!
  CR ." CEW=" INPUTHMS D->F F2PI/360 F* FCEW F!
  SETERRFLAG
THEN
DUP DEFAULT?
IF
  DERR
  SETERRFLAG
THEN
DUP HUNT?
IF
  ( TERMIF ( )
    BCR ABCR
    AB." This Program is not set up to Hunt error angles!"
    ABCR AB." This requires an external PC program!"
  ( THEN ( )
THEN
SKIP?
IF
  USERR? IF
    ( TERMIF ( )
      BCR ABCR
      AB." Correct coordinates for mount errors?"
    ( THEN ( )
    CR ." USE ERRORS?" GETYORN
    IF SETERRFLAG ELSE RESETERRFLAG THEN
  THEN
THEN ;

: 3AIM3 ( fra/Ddec/Daz/Dal - xy1/xy2/xy3/Daz/Dal )
>2R >2R >2R ( - fra )
FDUP FSIN FSWAP FCOS ( - fsra/fcra ) ( - fsaz/fcaz )
2R DECAL->FPI/2RAD FCOS FSWAP ( - fsra/fcdec/fcra )
FOVER F* ( - fsra/fcdec/x1 )
FSWAP FROT ( - x1/fcdec/fsra )
F* ( - x1/x2 )
2R> DECAL->FPI/2RAD FSIN ( - x1/x2/x3 )
2R> 2R> ;

: MUCKAZ ( Daz/Dal/Dra/Ddec - DMaz/Dal/Dra/Ddec )
>2R >2R >2R >2R F2PI 2R> RAAZ->F2PIRAD F-
F2PIRAD->RAAZ 2R> 2R> 2R> ;

: 3AIM ( - x1/x2/x3/y1/y2/y3 )
( Generate row of X and Y matrix )
AIMATSTAR ( - Daz/Dal/Dra/Ddec ) MUCKAZ
2ROT >2R 2ROT >2R >2R ( - Dra )
RAAZ->F2PIRAD FCONVK STARTIME 2@ D->F F* F-
( Take observed star time into account )
2R> 2R> 2R> 3AIM3 >2R ( - Daz )
RAAZ->F2PIRAD 2R> 0. 0. ( The 0. are dummy vars )
3AIM3 2DROP 2DROP
( - y1/y2/y3 ) ;

: 3RDV ( x11/x21/x31/f/f/f/x12/x22/x32/f/f/f - ... )
( ... - x11/x21/x31/f/f/f/x12/x22/x32/f/f/f/x13/x23/x33 )
( Generate third row of either X or Y matrix )
11 FPICK 5 FPICK F* 11 FPICK 7 FPICK F* F- ( - x13 )
11 FPICK 8 FPICK F* 14 FPICK 7 FPICK F* F- ( - x13/x23 )
14 FPICK 8 FPICK F* 14 FPICK 10 FPICK F* F- ( x13/x23/x33 )
( normalize values )
3 0 DO 3 FPICK FDUP F* LOOP F+ F+ FSQRT
( - x13/x23/x33/fa )
FDUP FTMP3RDV F! F/ ( store fa in FTMP3RDV temporary var )
FROT FTMP3RDV F@ F/ ( - x23/nx33/nx13 )
FROT FTMP3RDV F@ F/ ( - nx33/nx13/nx23 )
FROT ( - nx13/nx23/nx33 ) ;

: DETV ( v11/v21/v31 )
  ( /v12/v22/v32 )
  ( /v13/v23/v33 )
  ( - determinant of X )
9 FPICK 6 FPICK F* 2 FPICK F*
7 FPICK 4 FPICK F* 9 FPICK F* F+
4 FPICK 6 FPICK F* 10 FPICK F* F+
4 FPICK 7 FPICK F* 9 FPICK F* F-
10 FPICK 6 FPICK F* 4 FPICK F* F-
7 FPICK 10 FPICK F* 3 FPICK F* F- ;

: GETV1  3 0 DO 21 FPICK LOOP ;

: GETV2  3 0 DO 18 FPICK LOOP ;

: GETV3  3 0 DO 15 FPICK LOOP ;

: VID ( I - v1/v2/v3)
F0. F0. F1. ;

: 3ANG ( r1/r2/r3 - r1/r2/r3/f/h )
3 FPICK FDUP F*
3 FPICK FDUP F* F+ FSQRT ( - r1/r2/r3/c )
FDUP F0. F= IF
  FOVER F0< IF FPI/2 F-1. F* ELSE FPI/2 THEN
ELSE FOVER FSWAP F/ FATN THEN
( - r13/r23/r33/h )
4 FPICK F0. F= IF
  3 FPICK F0< IF F2PI FPI/2 F- ELSE FPI/2 THEN
ELSE
  4 FPICK F0< IF
    3 FPICK 5 FPICK F/ FATN FPI F+
  ELSE
    3 FPICK 5 FPICK F/ FATN
  THEN
THEN
FSWAP ;

: DUMQ  4 1 DO F0. LOOP ; ( Dummy Q matrix onto stack )

: FINDQ
4 1 DO ( M )
  5 I - 1 DO F0. LOOP ( Dummy rest of Q on stack )
  ( Put V onto stack )
  I 1 = IF
    VID GETV2 GETV3
  ELSE I 2 = IF
      GETV1 VID GETV3
    ELSE
      GETV1 GETV2 VID
    THEN
  THEN
  DETV ( - determinant )
  FDETV F@ F/
  14 I - 1 DO FSWAP FDROP LOOP
LOOP ; ( - [X/Y]/Q )

: FINDR
4 1 DO ( I )
  4 1 DO ( L )
    23 I 5 * - FPICK 3 J + FPICK F*
  LOOP
  F+ F+
LOOP ;

: Y01 ( ff/fh - y1/y2/y3 )
FY01TMP F! ( - ff )
FDUP FSIN FSWAP FCOS ( - fsf/fcf )
FY01TMP F@ FCOS FSWAP ( - fsf/fch/fcf )
FOVER F* ( - fsf/fch/y1 )
FSWAP FROT ( - y1/fch/fsf )
F* ( - y1/y2 )
FY01TMP F@ FSIN ; ( - y1/y2/y3 )

: 2LATAO ( - flatitude )
( fao stored in FAZOFF )
3AIM 3AIM 3RDV 3RDV ( Make X and Y matricies )
( - x11/x21/x31/y11/y21/y31 )
(  /x12/x22/x32/y12/y22/y32 )
(  /x13/x23/x33/y13/y23/y33 )
( Transform Matrix )
DUMQ
( Now copy X to top to find determinant )
GETV1 GETV2 GETV3 DETV FDETV F!
12 NFDROP ( throw away Q and V )
( Now really compute Q )
FINDQ
( R = Y * Q )
FINDR
( Now find Altitude and Azimuth of RA = don't care, Dec = 90 degrees )
( Result is Latitude and Azimuth Offset )
3ANG ( - ff/fh )
Y01 ( ff/fh - y1/y2/y3 )
3ANG ( - ff/fh )
>2R >R ( save off flatitude )
( Now fix up Computed Azimuth before saving )
F2PI FSWAP F-
INRNG ( make sure it is in a valid range )
FAZOFF F! ( Azimuth Offset )
( - x9/y9/q3/r3/y3 )
27 NFDROP ( Clean up stack )
R> 2R> ( restore flatitude ) ;

: 2HUNTAO ( - )
( still need to do this first to set LST )
FINDLST<-LONG ( - ) ( *1, #2 ) ( $5, $6, $7, $8 )
AIMATSTAR ( - Daz/Dal/Dra/Ddec )
( change ra to hour angle=LST-ra )
>2R ( save off dec )
RAAZ->F2PIRAD ( - fra )
( seconds offset of star obs is in STARTIME )
LST STARTIME 2@ ( - fra/addr/d )
2DUP 365 M/MOD ROT DROP ( see word GETIME in TCDERVIO )
D+ ROT 2@ D+ ( - fra/dlst )
D->F ( lst in seconds, now convert to radians )
F2PI/24HR F*
FSWAP ( - flst/fra )
F- ( - fha )
FDUP F0< IF F2PI F+ THEN ( correct if negative )
F2PIRAD->RAAZ ( - ha )
2R> ( - Daz/Dal/Dha/Ddec ) ( restore dec )
4 2ROLL 4 2ROLL ( cheat so as to use the FINDH routine )
( Now compute Ao )
( Ao = Aencoder - ArcCos[[Sin dec - Sin lat * Sin alt]/Cos lat * Cos alt]] )
FINDH ( Dha/Ddec/Daz/Dal - fazo/Daz ) ( should get right quadrant )
RAAZ->F2PIRAD FSWAP F-
INRNG ( make sure it is in a valid range )
F2PIRAD->RAAZ AZOFF L! SETAO ;

: 2STAR ( The other initialization routine )
LATCOM ( - n )
HUNT?
IF ( Hunt Lat )
  ( Do 2 star initialization here )
  2LATAO ( - flatitude )
  FLATITUDE F! LATSPEED ( Latitude )
  R# @ IF ( Only find Ao if at limit. )
    AOCOM HUNT? IF FAZOFF F@ F2PIRAD->RAAZ AZOFF L! THEN
    SETAO
  THEN
  ( FALSEFLAG ITGOTSTARFLAG ! ( Already have aimed at a star )
  LONGCOM ( - flag )
  3LONG ( flag - ) ( Now find Logitude by long known method )
( ----------------------------------- )
ELSE ( Lat known )
  LATSPEED
  ( TRUEFLAG ) 0 ITGOTSTARFLAG ! ( assume we need to aim at star )
  R# @ IF ( Only find Ao if at limit. )
    AOCOM HUNT?
    IF ( Lat known, Hunt Ao ) ( *1, #1 )
      LONGCOM HUNT? ( - flag )
      IF
        ( Hunt both Long and Ao with 2star method )
        2LATAO ( - flatitude )
        FDROP ( already have latitude, so don't need this )
        FAZOFF F@ F2PIRAD->RAAZ AZOFF L! SETAO
        TRUEFLAG ( Yes, we are HUNTing Longitude )
        ( FALSEFLAG ITGOTSTARFLAG ! )
        ( Already have aimed at a star for 3LONG )
        3LONG ( flag - ) ( *1, #2 ) ( $5, $6, $7, $8 )
      ELSE ( Longitude known, find Ao )
        TRUEFLAG AUTOSTARFLAG C! ( Allow AutoStar selection )
        2HUNTAO
      THEN
    ELSE ( Know Ao and it is set )
      LONGCOM ( - flag )
      3LONG ( flag - ) ( *1, #2 ) ( $5, $6, $7, $8 )
    THEN
  ELSE ( skipping Ao initialization, assume okay )
    LONGCOM ( - flag )
    3LONG ( flag - ) ( *1, #2 ) ( $5, $6, $7, $8 )
  THEN
THEN ;

: GETEMP ( - )
( TERMIF ( )
  BCR ABCR
  AB." Input Temperature in degrees Farenheit."
( THEN ( )
CR ." TEMPF=" GETFDECNUM ( - f )
FDUP FTEMPF F! ; ( Store for later reference )

: REFINIT ( - )
( TERMIF ( )
  SRCMSG B." Refraction Parameters?"
( THEN ( )
CR ." REFRACT SRC?" GETSOURCE ( - n )
DUP INPUT? OVER HUNT? OR
IF
  GETEMP ( - )
  F460. F+ ( - ft )
  ( TERMIF ( )
    BCR ABCR
    AB." Input Barometric Pressure as inches of mercury."
  ( THEN ( )
  CR ." PINHG=" GETFDECNUM ( - ft/finHg )
  FDUP FPINHG F! ( Store for later reference )
  FDUP F0. F= ( - ft/finHg/flag )
  IF
    FDROP ( - ft )
    TERMIF
      BCR ABCR
      AB." Enter Elevation as feet above sea level."
      TRUEFLAG
    ELSE
      GPSINITFLAG C@ 0=
    THEN
    IF
      CR ." EL=" GETFDECNUM ( - ft/fel )
      ( Check for default value retrieval )
      FDUP F0. F= IF FDROP ELEVATION @CLK S->F THEN
      ( And store elevation for later retrieval )
    ELSE
      FHEIGHT F@
    THEN
    STRHGT ( ft/fel - )
  ELSE
    F0. FHEIGHT F!
    FSWAP F/ FREFRACTCO F! SETREFRACTFLAG
  THEN
THEN
DUP DEFAULT? IF DREFRACT THEN
SKIP? IF
  NOTINIT? 0= IF
    ( TERMIF ( )
      BCR ABCR
      AB." Account for Refraction?"
    ( THEN ( )
    CR ." REFRACTION?" GETYORN ( - flag )
    IF
      SETREFRACTFLAG
    ELSE
      RESETREFRACTFLAG
    THEN
  ELSE
    RESETREFRACTFLAG
  THEN
THEN ;

: PREINIT ( - )
INITERR ( Account for mount errors while initializing, if desired. )
EQ? IF
  ( TERMIF ( )
    BCR ABCR
    AB." Is the telescope on an Equatorial Fork mount?"
    ABCR
    AB." (If not, a German Equatorial mount will be assumed.)"
  ( THEN ( )
  CR ." FORK?" GETYORN
  EMOUNTFLAG C!
THEN
INITRANGE
( TERMIF ( )
  BCR ABCR
  AB." Account for Precession?"
( THEN ( )
CR ." PRECESSION?" GETYORN
IF SETPRECESSFLAG ELSE RESETPRECESSFLAG THEN
REFINIT ;

: REALINIT ( flag - )
IF
  2STAR
ELSE ( Standard NCP Initialization )
  INITLATITUDE ( - )
  R# @ IF INITAZ THEN ( - ) ( Only adjust if at limits )
  ( TRUEFLAG) 0 ITGOTSTARFLAG !
  ( Hunt for star to find longitude )
  LONGCOM ( - flag )
  3LONG ( flag - ) ( Find Longitude by long known method )
THEN ;

: NEWDEF (  - ) ( Set defaults )
( TERMIF ( )
  BCR ABCR
  AB." Should the current telescope parameters become "
  AB." the new defaults?"
( THEN ( )
CR ." NEW DEFAULT?" GETYORN ( - n )
( current values are to be the new defaults, so save them )
IF
  MRAAZ L@ RRAAZ 3B!CLK
  MDECAL L@ RDECAL 3B!CLK
  REFRACT? IF
    FTEMPF F@ F->S DTEMPF C!CLK
    FHEIGHT F@ F->S ELEVATION !CLK
  THEN
  FLATITUDE F@ F360/2PI F* F->D DLAT 3B!CLK
  FLONGITUDE F@ F360/2PI F* F->D DLONG 3B!CLK
  AZOFF L@ DAZOFF 3B!CLK
  USERR? IF
    FCEW F@ F360/2PI F* F->D DROP DCEW !CLK
    FRHO F@ F360/2PI F* F->D DROP DRHO !CLK
  THEN
  BFLAGS C@CLK 64 OR ( set flag )
  DUP BFLAGS C!CLK
  96 AND 96 = ( Both time and init done? )
  IF
    BFLAGS C@CLK
    127 AND ( clear flag for init )
    PRECESS? IF 8 OR ELSE -9 AND THEN
    REFRACT? IF 4 OR ELSE -5 AND THEN
    USERR?   IF 2 OR ELSE -3 AND THEN
    EMOUNTFLAG C@ IF 1 OR ELSE -2 AND THEN
    BFLAGS C!CLK
  THEN
THEN
TRUEFLAG AUTOSTARFLAG C! ( Allow AutoStar selection )
SETINITFLAG ;

: INIT ( - )
PREINIT ( - )
EQ? IF ( don't allow NCP initialization for Equatorial mounts )
  TRUEFLAG REALINIT
ELSE
  ( TERMIF ( )
    BCR ABCR
    AB." Use 2-star initialization?"
    ABCR
    AB." (If not, the program will do Celestial Pole initialization.)"
    ABCR
  ( THEN ( )
  CR ." 2 STAR?" GETYORN REALINIT
THEN
( Set defaults )
NEWDEF ;

: GET$ ( - d )
( First, take 2 seconds to check for $ char )
RDTIME 3. D+ ( - Dseconds to check until )
BEGIN
  TERM1? ( - n ) ( char or zero )
  DUP IF ( check if non-zero )
    36 ( $ char ) =
  THEN ( - flag )
  >R 2DUP RDTIME D= R> OR
UNTIL ;

: GETTNUM ( n - n )
( in -> # of chars )
0 SWAP 0
DO
  10 *
  KEY1 48 - +
LOOP ;

: TOSSCOMMA ( - )
BEGIN KEY1 44 = ( comma? ) UNTIL ;

: RDGNUM ( n - f/nkey )
GETTNUM ( degrees ) 60 * ( to minutes )
2 GETTNUM + ( minutes ) 60 M* ( - Dseconds )
KEY DROP ( Throw period )
3 GETTNUM ( - d/nfracmin )
6 100 */ 
/* ignore this starting comment line....
0 D+ D->F ( - fseconds)
TOSSCOMMA
KEY1 ;

: GPS ( - flag )
( Only do if GPS module is sending data to SCI1 )
  GET$ ( - d )
  ( either we got a $, or timed out )
  RDTIME D= 0= ( Not timed out )
  IF ( decode )
    NOTIMESET? IF INITZONE THEN
    ( Assume refraction )
    GETEMP SETREFRACTFLAG
    CR ." GPS..."
    0 GPSDECODED ! ( Temp to keep track of data decoded )
    GET$ 2DROP
    ( Here we check to see if GPS is sending valid data )
    ( - )
    BEGIN ( loop until lat is there )
      FALSEFLAG ( flag to keep looping )
      KEY1 ( get char )
      71 = ( G? )
      IF
        KEY1 80 = ( P? )
        IF
          KEY1 71 = ( G? )
          IF
            KEY1 71 = ( G? ) AND
            KEY1 65 = ( A? ) AND
            IF
              ( GPGGA word )
              TOSSCOMMA
              TOSSCOMMA ( skip time )
              KEY1 ( - n ) ( check Latitude )
              44 = 0= ( not comma? )
              IF DROP TRUEFLAG THEN
            THEN ( end of $GPGGA )
          THEN
        THEN ( P )
      THEN
      BEGIN KEY1 36 = ( $? ) UNTIL
    UNTIL
    ( and then we return to getting that data )
    BEGIN ( loop to get data )
      KEY1 ( get char )
      DUP 71 = ( G? )
      IF
        DROP KEY1 80 = ( P? )
        IF
          KEY1 DUP 71 = ( G? )
          IF
            DROP KEY1 DUP 71 = ( G? ) AND
            KEY1 65 = ( A? ) AND
            IF
              ( GPGGA word )
              DROP TOSSCOMMA
              ( Get time )
              2 GETTNUM ( - hours ) 60 *
              2 GETTNUM + 60 M* ( - d )
              2 GETTNUM 0 D+ ( - DsecondsGMT )
              PUTIME ( d - )
              TOSSCOMMA
              2 RDGNUM ( - f/n ) ( get Latitude )
              TOSSCOMMA
              3 RDGNUM ( - f/n ) ( get Longitude )
              TOSSCOMMA TOSSCOMMA ( skip fix quality )
              TOSSCOMMA ( # satellites )
              TOSSCOMMA ( Horizontal dilution )
              GETFDECNUM ( - felevation in meters )
              F3.28084 F* ( - felevation in feet )
              FTEMPF F@ F460. F+ ( - ft )
              FSWAP
              STRHGT ( ft/fel - )
              ( set it up regardless )
              1 GPSDECODED @ OR
              DUP 3 AND 3 = IF SETRTC SETIMEFLAG THEN    
              GPSDECODED ! ( update flag )
              ( Now do Longitutde/Latitude stuff )
              87 = ( W? ) IF F-1. F* THEN
              F2PI/360 F* FLONGITUDE F!
              83 = ( S? ) IF F-1. F* THEN
              F2PI/360 F* FDUP ELAT ( felat - )
              FLATITUDE F! LATSPEED
            THEN ( end of $GPGGA )
          ELSE
            ( check for $GP_RMC_ for date )
            82 = ( R? )
            KEY1 77 = ( M? ) AND
            KEY1 67 = ( C? ) AND
            IF ( $GPRMC )
              9 0 DO TOSSCOMMA LOOP ( skip extra stuff )
              2 GETTNUM GBDAY C!
              2 GETTNUM GBMONTH C!
              2 GETTNUM 2000 + GBYEAR !
              2 GPSDECODED @ OR
              DUP 3 AND 3 = IF SETRTC SETIMEFLAG THEN   
              GPSDECODED ! ( update flag )
            THEN
          THEN ( R )
        THEN ( P )
(      ELSE ( H )
(        72 = ( H? ) ( check for $HCHDG )
(        KEY1 67 = AND ( C? )
(        KEY1 72 = AND ( H? )
(        KEY1 68 = AND ( D? )
(        KEY1 71 = AND ( G? )
(        TOSSCOMMA ( )
(        IF ( HCHDG - compass heading )
(          GPSDECODED @ 4 AND 0= ( )
(          IF ( )
(            GETFDECNUM ( - f is magnetic heading )
(            FV F! ( store temporarily )
(            GPSDECODED @ 4 OR GPSDECODED ! ( )
(          ELSE ( get variation )
(            F@ 8 AND 0= ( )
(            IF ( )
(              TOSSCOMMA ( skip mag heading )
(              TOSSCOMMA ( skip mag dev )
(              TOSSCOMMA ( skip mag dev sign )
(              GETFDECNUM ( get variation )
(              FU F! ( )
              ( to get TRUE, add 'E' variation to reading )
(              GPSDECODED @ 8 OR GPSDECODED F! )
( Leave AZOFF alone for now )
(            ELSE ( get variation sign )
(              3 0 DO TOSSCOMMA LOOP ( )
(              KEY1 87 = ( W? )
(              IF F-1. ELSE F1. THEN  ( )
(              FU F@ F* ( - variation )
(              FV F@ F+ ( - true heading )
(              3600 S->F F* ( convert to seconds )
(              FDUP F0< IF F1296000. F+ THEN ( )
(              F->D AZOFF L! ( )
(              GPSDECODED @ 16 OR GPSDECODED ! ( set flag )
(            THEN ( )
(          THEN ( )
(        THEN ( )
      THEN
      ( check for $ and Flags == 31/3 )
      BEGIN KEY1 36 = ( $? ) UNTIL
      GPSDECODED @
      ( 31 ) 3 = ( everything has been set? )
    UNTIL
    FINDLST<-LONG SETAO
    TRUEFLAG  ( GPS initialized system )
  ELSE
    FALSEFLAG
  THEN
DUP GPSINITFLAG C! ;
//* /
void BINIT ( ) { //Initialization on bootup
BFLAGS C@CLK 224 AND 96 = ( Is bit pattern 011 for top 3 bits? )
( And not already initialized as well? )
NOTIMESET? NOTINIT? AND AND
IF
  DTZONE C@CLK TZONE C! ( Restore time zone )
  BFLAGS C@CLK DUP 16 AND ( DST )
  DUP DSTFLAG C!
  IF GETZONE 1+ TZONE C! THEN
  DUP 4 AND
  IF
    DREFRACT SETREFRACTFLAG
  ELSE
    ( set up a dummy values )
    F60. FTEMPF F!
    F30. FPINHG F!
    RESETREFRACTFLAG
  THEN
  DUP 2 AND
  IF SETERRFLAG ELSE RESETERRFLAG THEN
  DUP 1 AND EMOUNTFLAG C! ( Fork mount? )
  8 AND
  IF SETPRECESSFLAG ELSE RESETPRECESSFLAG THEN
  DDATE DTIME ( Restore date and time )
  STRTturboCLK
  SETIMEFLAG
  DERR ( Restore error constants )
  DLAT 3B@CLK D->F F2PI/360 F* FDUP ELAT
  FLATITUDE F! ( Set Latitude )
  LATSPEED
  DLONG 3B@CLK D->F F2PI/360 F*
  FLONGITUDE F! ( Set Longitude )
  0 ELATSRC C! ( Fake out equatorial source )
  ( check for GPS setting of this stuff, which overrides )
  GPS ( - flag )
  GPSOFF
  SMPINIT ( allow user to init SMP coordinates )
  OPENCOVER ( - )
  RRAAZ 3B@CLK RDECAL 3B@CLK
  INITSCOPE ( Set default ranges )
  ( Scope at limits, one way or another )
  0 ITGOTSTARFLAG ! ( assume we need to aim at star )
  TRUEFLAG AUTOSTARFLAG C! ( Allow AutoStar selection )
  AOCOM HUNT?
  IF ( Lat known, Hunt Ao ) ( *1, #1 )
    2HUNTAO
  ELSE ( Know Ao and it is set )
    FINDLST<-LONG
  THEN
  ( Save parameters if GPS was used )
  IF SETRTC NEWDEF THEN
  SETINITFLAG
ELSE
  ( Use GPS to determine AutoStar call )
  NOTINIT?
  IF
    GPS ( - flag )
    GPSOFF
    IF
      SMPINIT ( allow user to init SMP coordinates )
      OPENCOVER ( - )
      PREINIT ( - )
      ( Scope at limits, one way or another )
      0 ITGOTSTARFLAG ! ( assume we need to aim at star )
      TRUEFLAG AUTOSTARFLAG C! ( Allow AutoStar selection )
      AOCOM HUNT?
      IF ( Lat known, Hunt Ao ) ( *1, #1 )
        2HUNTAO
      ELSE ( Know Ao and it is set )
        FINDLST<-LONG
      THEN
      SETRTC NEWDEF ( - )
      SETINITFLAG
    ELSE
      FALSEFLAG AUTOSTARFLAG C! ( Disable AutoStar selection )
    THEN
  THEN
THEN
}
// */
long initIRkey(long previous) {
  // Get IR input three identical times to proceed
  //char n, tabn;
  long x1, x2, x3, xn;
  x1 = x2 = 0L; x3 = 1L;
  do {
    if (irrecv.decode(&results)) {
      // Return an IR input char if it has been repeated three times in a row
      xn = results.value;
      if (xn != 0xffffffffL) {
        TCterminal.println(xn, HEX);
        if (x2 != 0 && x3 == 1) {
          if (x1 == xn)
            x3 = x1;
          else
            x1 = x2 = 0;
        }

        if (x1 != 0 && x2 == 0) {
          if (x1 == xn)
            x2 = x1;
          else
            x1 = 0;
        }
        if (x1 == 0 ) x1 = xn;
        if (x1 == previous) x1 = 0L;
      }
      irrecv.resume(); // Receive the next value
    }
  } while (x1 != x2 || x2 != x3);
  return x1;
}

void RESETIR() {
  // IR remote use setup
  TCterminal.println(" ");
  TCterminal.println(" Need IR keys for: 0-9 - . <cr> <backspc> Right Left Up Down Lock");
  TCterminal.println(" Enable IR remote?");
  if (GETYORN()) { // Init IR decode array
    TCterminal.println("\n Press each key three times");
    TCterminal.print("\r 0"); eecharbuf.strunion.IRinput[0] = initIRkey(1L); eecharbuf.strunion.IRchar[0] = '0';
    TCterminal.print("\r 1"); eecharbuf.strunion.IRinput[1] = initIRkey(eecharbuf.strunion.IRinput[0]); eecharbuf.strunion.IRchar[1] = '1';
    TCterminal.print("\r 2"); eecharbuf.strunion.IRinput[2] = initIRkey(eecharbuf.strunion.IRinput[1]); eecharbuf.strunion.IRchar[2] = '2';
    TCterminal.print("\r 3"); eecharbuf.strunion.IRinput[3] = initIRkey(eecharbuf.strunion.IRinput[2]); eecharbuf.strunion.IRchar[3] = '3';
    TCterminal.print("\r 4"); eecharbuf.strunion.IRinput[4] = initIRkey(eecharbuf.strunion.IRinput[3]); eecharbuf.strunion.IRchar[4] = '4';
    TCterminal.print("\r 5"); eecharbuf.strunion.IRinput[5] = initIRkey(eecharbuf.strunion.IRinput[4]); eecharbuf.strunion.IRchar[5] = '5';
    TCterminal.print("\r 6"); eecharbuf.strunion.IRinput[6] = initIRkey(eecharbuf.strunion.IRinput[5]); eecharbuf.strunion.IRchar[6] = '6';
    TCterminal.print("\r 7"); eecharbuf.strunion.IRinput[7] = initIRkey(eecharbuf.strunion.IRinput[6]); eecharbuf.strunion.IRchar[7] = '7';
    TCterminal.print("\r 8"); eecharbuf.strunion.IRinput[8] = initIRkey(eecharbuf.strunion.IRinput[7]); eecharbuf.strunion.IRchar[8] = '8';
    TCterminal.print("\r 9"); eecharbuf.strunion.IRinput[9] = initIRkey(eecharbuf.strunion.IRinput[8]); eecharbuf.strunion.IRchar[9] = '9';
    TCterminal.print("\r -"); eecharbuf.strunion.IRinput[10] = initIRkey(eecharbuf.strunion.IRinput[9]); eecharbuf.strunion.IRchar[10] = '-';
    TCterminal.print("\r ."); eecharbuf.strunion.IRinput[11] = initIRkey(eecharbuf.strunion.IRinput[10]); eecharbuf.strunion.IRchar[11] = '.';
    TCterminal.print("\r <cr>"); eecharbuf.strunion.IRinput[12] = initIRkey(eecharbuf.strunion.IRinput[11]); eecharbuf.strunion.IRchar[12] = 0x0d;
    TCterminal.print("\r <backspc>"); eecharbuf.strunion.IRinput[13] = initIRkey(eecharbuf.strunion.IRinput[12]); eecharbuf.strunion.IRchar[13] = 0x08;
    TCterminal.print("\r Right    "); eecharbuf.strunion.IRinput[14] = initIRkey(eecharbuf.strunion.IRinput[13]); eecharbuf.strunion.IRchar[14] = 0x3e;
    TCterminal.print("\r Left "); eecharbuf.strunion.IRinput[15] = initIRkey(eecharbuf.strunion.IRinput[14]); eecharbuf.strunion.IRchar[15] = 0x3c;
    TCterminal.print("\r Up  "); eecharbuf.strunion.IRinput[16] = initIRkey(eecharbuf.strunion.IRinput[15]); eecharbuf.strunion.IRchar[16] = 0x5e;
    TCterminal.print("\r Down"); eecharbuf.strunion.IRinput[17] = initIRkey(eecharbuf.strunion.IRinput[16]); eecharbuf.strunion.IRchar[17] = 0x76;
    TCterminal.print("\r Lock"); eecharbuf.strunion.IRinput[18] = initIRkey(eecharbuf.strunion.IRinput[17]); eecharbuf.strunion.IRchar[18] = 0x0d;
    eecharbuf.strunion.IRSETUPflag = irsetup = true;
  } else {
    eecharbuf.strunion.IRSETUPflag = irsetup = false;
  }
  SETIRFLAG();
}

void OptionStateMsg(boolean flag) {
  //Print out Option state
  if (flag) {
    TERMtextcolor('g');
    TCterminal.println("Enabled");
  } else {
    TERMtextcolor('r');
    TCterminal.println("Disabled");
  }
  TERMtextcolor('w');
}

void RESETTCI() {
  // Set up basic telescope interface parameters
  // i.e. Ranges, etc.
  char choice = 'A';
  
  do {
    TCterminal.println("\n Select Option to Enable/Disable");
    TCterminal.print(" 1) OLED Display: ");    OptionStateMsg(eecharbuf.strunion.OLEDflag);
    TCterminal.print(" 2) PIC LCD Display: ");    OptionStateMsg(eecharbuf.strunion.LCDpicflag);
    newdelay(10);
    TCterminal.print(" 3) I2C LCD Display: ");    OptionStateMsg(eecharbuf.strunion.LCDi2cflag);
    TCterminal.print(" 4) INA219 Voltage Monitor: ");    OptionStateMsg(eecharbuf.strunion.INA219flag);
    TCterminal.print(" 5) Motor Driver: ");    OptionStateMsg(eecharbuf.strunion.MotorDriverflag);
    newdelay(10);
    TCterminal.print(" 6) Limit Switches: ");    OptionStateMsg(eecharbuf.strunion.NOLIM);
    TCterminal.print(" 7) Precession: ");    OptionStateMsg(eecharbuf.strunion.PFLAG);
    TCterminal.print(" 8) Refraction: ");    OptionStateMsg(eecharbuf.strunion.RFLAG);
    newdelay(10);
    TCterminal.print(" 9) Site ID: ");
    TERMtextcolor('g');   TCterminal.println(eecharbuf.strunion.SiteID);    TERMtextcolor('w');
    TCterminal.println(" 0) Done - Return to Main Display");
    choice = KEY();
    if (choice == '1') {
      eecharbuf.strunion.OLEDflag = !(eecharbuf.strunion.OLEDflag);
      if (eecharbuf.strunion.OLEDflag) {
        oled.begin();
        oled.clear(ALL);
        oled.display();
        newdelay(500);
        oled.clear(PAGE);
      }
    }
    if (choice == '2') eecharbuf.strunion.LCDpicflag = !(eecharbuf.strunion.LCDpicflag);
    if (choice == '3') eecharbuf.strunion.LCDi2cflag = !(eecharbuf.strunion.LCDi2cflag);
    if (choice == '4') {
      eecharbuf.strunion.INA219flag = !(eecharbuf.strunion.INA219flag);
      if (eecharbuf.strunion.INA219flag) ina219.begin(); // Voltage monitor
    }
    if (choice == '5') eecharbuf.strunion.MotorDriverflag = !(eecharbuf.strunion.MotorDriverflag);
    if (choice == '6') eecharbuf.strunion.NOLIM = !(eecharbuf.strunion.NOLIM);
    if (choice == '7') eecharbuf.strunion.PFLAG = !(eecharbuf.strunion.PFLAG);
    if (choice == '8') eecharbuf.strunion.RFLAG = !(eecharbuf.strunion.RFLAG);
    if (choice == '9') {
      // Redefine Site ID
      TCterminal.print("\n Enter new Site ID: ");
      inputstrlen = ACCEPT(eecharbuf.strunion.SiteID, 39); //Get 39 chars max
      TCterminal.println(" ");
    }
  } while (choice != '0');
  SETTCIFLAG();
}
