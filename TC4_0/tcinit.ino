// TCINIT
// Telescope Controller 4.0 - Initialization and High level functionality
#include "tcheader.h"

// We have to define this function to use the SAMD alternate Serial2 pins
//void SERCOM2_Handler() {
//  Serial2.IrqHandler();
//}

void inithardware() { //Set up all the hardware interfaces
  // Read Hardware button for forced re-init, position Lock, etc.
  pinMode(LOCKBTN, INPUT_PULLUP);
  //  lockbtninitflag = RDLBTN(); //Get button state, first thing
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  IRkey = 0L;
  // Brute force default initialization
  // We will replace the defaults if valid data is in an EEPROM
  eecharbuf.strunion.EEchk = 0; // Check for existence of EEPROM
  eecharbuf.strunion.DSTFLAG = false; // Daylight Savings used
  eecharbuf.strunion.NOLIM = false; // Using Limit/reference sensors?
  eecharbuf.strunion.ERRFLAG = false; // Errors flag
  eecharbuf.strunion.PFLAG = false; // Precession flag
  eecharbuf.strunion.RFLAG = false; // Refraction flag
  eecharbuf.strunion.MotorDriverflag = false; // Motor driver flag
  eecharbuf.strunion.SerialTermflag = true; // Serial port Terminal display
  eecharbuf.strunion.OLEDflag = false; // OLED display used?
  eecharbuf.strunion.LCDpicflag = false; // PIC-based LCD on Serial used?
  eecharbuf.strunion.LCDi2cflag = false; // LCD on IIC?
  eecharbuf.strunion.INA219flag = false; // INA219 used?
  eecharbuf.strunion.IRSETUPflag = irsetup = false; // IR remote in use
  eecharbuf.strunion.TCIFLAG = TCIFLAG = false; // Telescope Control Interface set up
  eecharbuf.strunion.ELEVATION = 5500; // Elevation in feet
  eecharbuf.strunion.RRAAZ = 10000L; // Azimuth encoder range
  eecharbuf.strunion.RDECAL = 4000L; // Altitude encoder range
  eecharbuf.strunion.DTEMPF = 68.0; // Default temp
  eecharbuf.strunion.DTZONE = -8.0; // Default Time Zone (PST)
  eecharbuf.strunion.FLATITUDE = 1.; // Latitude in radians
  eecharbuf.strunion.FLONGITUDE = -1.; // Longitude in radians, West is negative
  strncpy ( eecharbuf.strunion.SiteID, "Demonstration mode", sizeof("Demonstration mode") );
  // Set up default values for Demo mode
  TZONE = eecharbuf.strunion.DTZONE;
  DSTFLAG = eecharbuf.strunion.DSTFLAG;
  LCDbrightness = 0x9D;

  // verify presence of I2C EEPROM
  byte i2cStat = tcEEPROM.begin(extEEPROM::twiClock100kHz);
  I2CEEPROMpresent = true;
  if ( i2cStat != 0 ) {
    I2CEEPROMpresent = false;
    //lockbtninitflag = true; //Force demo mode
  } else { // /*
    // Read in EEPROM values
    int testbyte = tcEEPROM.read(0L); // read first byte in EEPROM
    if (testbyte == EEcheckByte) {
      // Good data there, so read in the rest of the array
      int eestat = tcEEPROM.read(2L, eecharbuf.ebuf, sizeof(EEstruct));
      if (eestat == 0) {
        LCDline4(); LCDprint("EEPROM read error ");
        TZONE = eecharbuf.strunion.DTZONE;
        DSTFLAG = eecharbuf.strunion.DSTFLAG;
        irsetup = eecharbuf.strunion.IRSETUPflag;
        TCIFLAG = eecharbuf.strunion.TCIFLAG;
      }
    }
    eecharbuf.strunion.EEchk = EEcheckByte; // */
  }

  if (eecharbuf.strunion.LCDi2cflag || eecharbuf.strunion.LCDpicflag || eecharbuf.strunion.OLEDflag) {
    LCDclear(); // Only clear if we are using LCD display
    newdelay(500);
    LCDline1();
    LCDprint("TC ");
    LCDprint(tcversion);
    LCDprint("    ");
  }
  if (I2CEEPROMpresent) {
    TCterminal.println("I2C EEPROM detected.");
  } else {
    TCterminal.println("I2C EEPROM *NOT* detected.");
  }

  if (eecharbuf.strunion.INA219flag) {
    ina219.begin(); // Voltage monitor
    // To use a slightly lower 32V, 1A range (higher precision on amps):
    //ina219.setCalibration_32V_1A();
    // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
    ina219.setCalibration_16V_400mA();
    busvolts = 0.;
    current_mA = 0.;
  }
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
    TCterminal.println("BME280 Humidity, Barometric Pressure and Temperature sensor detected.");
  }

  if (getDistanceSensorPresent()) {
    TCterminal.println("RFD77402 Distance Sensor detected.");
  }

  newdelay(100);
  //ubloxGPS.enableDebugging(); 
  if ( (ubloxGPS.begin() == false) || !I2CEEPROMpresent ) {
    //* We have to add in check for I2CEEPROMpresent, because if we don't
    // a bare Redboard Turbo will end up getting a false positive for GPS present check.
    // This means that to use GPS, the system MUST have an I2C EEPROM present.
    // Ublox GPS not detected at default I2C address
    GPSpresent = false;
    // Assume Demo mode then
    GRYEAR = 2020;
    GRMONTH = 1;
    GRDAY = 1;
    rtcseconds = 0;
    rtchours = rtcmin = 1;
    SETIMEFLAG(); // Fake it!

    FALTEMP = (double)eecharbuf.strunion.ELEVATION; //Elevation in feet
    FLATITUDE = eecharbuf.strunion.FLATITUDE = 40.091125;
    FLONGITUDE = eecharbuf.strunion.FLONGITUDE = -105.18512;
    // */
    TERMtextcolor('r');
    TCterminal.println("U-blox based GPS NOT detected!");
    // TCterminal.println("Fix this first and Reboot!");
    // while (1) ;
  } else { // GPS online
    GPSpresent = true;
    TCterminal.println("U-blox based GPS detected.");
    TCterminal.println("Waiting to sync with GPS...");
    //ubloxGPS.setAutoPVT(true); // non-blocking mode
    ubloxGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    ubloxGPS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
    // We gotta wait until satellite sync is active
    while (((ubloxGPS.getLatitude() == 0) && (ubloxGPS.getLongitude() == 0)) || (ubloxGPS.getSIV() == 0)) {
      newdelay(1000); //wait one second between checks - any faster just makes noise
      TCterminal.print(".");
    }
    GRYEAR = (int)ubloxGPS.getYear();
    GRMONTH = (byte)ubloxGPS.getMonth();
    GRDAY = (byte)ubloxGPS.getDay();
    rtchours = (byte)ubloxGPS.getHour();
    rtcmin = (byte)ubloxGPS.getMinute();
    rtcseconds = ubloxGPS.getSecond();
    FALTEMP = eecharbuf.strunion.ELEVATION = ubloxGPS.getAltitude() * 0.00328084; //Elevation in feet
    FLATITUDE = eecharbuf.strunion.FLATITUDE = ubloxGPS.getLatitude() / 10000000.;
    FLONGITUDE = eecharbuf.strunion.FLONGITUDE = ubloxGPS.getLongitude() / 10000000.;
    SETIMEFLAG(); // time is set correctly
  }
  //long Dtime = rtcseconds + (rtcmin * 60L) + (rtchours * 3600L);
  STRTturboCLK();
  myAstro.setLatLong(FLATITUDE, FLONGITUDE);
  myAstro.setTimeZone(TZONE);
  myAstro.setGMTdate(GRYEAR, GRMONTH, GRDAY);
  myAstro.setGMTtime(rtchours, rtcmin, rtcseconds);
  DSTFLAG = myAstro.useAutoDST();
  myAstro.getGMTsiderealTime();

  bme280.settings.commInterface = I2C_MODE;
  bme280.settings.I2CAddress = BME280_ADR;
  bme280.settings.runMode = 3; //Forced mode
  bme280.settings.tStandby = 0;
  bme280.settings.filter = 4;
  bme280.settings.tempOverSample = 5;
  bme280.settings.pressOverSample = 5;
  bme280.settings.humidOverSample = 5;
  //newdelay(500);
  if (eecharbuf.strunion.OLEDflag) {
    oled.clear(PAGE);
  }
  newdelay(500);
  irsetup = eecharbuf.strunion.IRSETUPflag; // Is IR table defined?
  //irrecv.enableIRIn(); // Start the IR receiver

  //myScope.begin(eecharbuf.strunion.MotorDriverflag, SPI_SS1, SPI_SS2, AZREFsensor, HORIZONlim, 0);
  rockerTiltPresent = false;
  tubeTiltPresent = false;
  if (rockerTilt.begin(SPI_SS1)) {
    rockerTiltPresent = true;
  }
  if (tubeTilt.begin(SPI_SS2)) {
    tubeTiltPresent = true;
  }

  if (getRockerTiltPresent()) {
    TCterminal.println("Rocker Murata SCL3300 inclinometer detected.");
  }
  if (getTubeTiltPresent()) {
    TCterminal.println("Tube Murata SCL3300 inclinometer detected.");
  }

  myDeclination.begin(); // Initialize magnetic variation calculations
  HMC6352.Wake();
  newdelay(20);
  FMAGHDG = HMC6352.GetHeading();
  HMC6352.Sleep();
  HMC6343MagCompasspresent = false;
  if (FMAGHDG > 360.1) { //Old Magnetic Compass not detected at default I2C address
    MagCompasspresent = false;
    // Initialize the HMC6343 and verify its physical presence
    if (dobHMC6343.init()) {
      TCterminal.println("HMC6343 Magnetic Compass detected.");
      HMC6343MagCompasspresent = true;
    }
  } else { // Old Magntic Compass online
    MagCompasspresent = true;
    TCterminal.println("HMC6352 Magnetic Compass detected.");
  }

  //MotorDriverflag = eecharbuf.strunion.MotorDriverflag;
  MotorDriverflag = false;
  if (MotorDriverflag == true) {
    //TCterminal.println("Motor driver setup.");
    i2cMotorDriver.settings.commInterface = I2C_MODE;
    i2cMotorDriver.settings.I2CAddress = MotorDriver_ADR;
    while ( i2cMotorDriver.begin() != 0xA9 ) //Wait until a valid ID word is returned
    {
      //TCterminal.println( "ID mismatch, trying again" );
      newdelay(500);
    }
    //TCterminal.println( "ID matches 0xA9" );
    //  Check to make sure the driver is done looking for slaves before beginning
    //TCterminal.print("Waiting for enumeration...");
    while ( i2cMotorDriver.ready() == false );
    //TCterminal.println("Done.");
  }

  SETIRFLAG(); // Fake it that IR is already set up
  SETTCIFLAG(); // Fake it that TC Options are already set up
  SETdisplayFLAG(); //Assume display is already set up
}

void STRTturboCLK() {
  // Initialize registers, and set up RTC
  rtczero.begin(); // initialize RTC on Readboard Turbo
  //rtczero.setDate(GRDAY, GRMONTH, (uint8_t)(GRYEAR % 100)); //Default date set
  //rtczero.setTime(rtchours, rtcmin, rtcseconds); //Default time set
  DateTime now = DateTime(GRYEAR, GRMONTH, GRDAY, rtchours, rtcmin, rtcseconds);
  rtczero.adjust(now);
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
    LCDclear(); LCDline1(); LCDprint("TZONE=["); LCDprint(TZONE); LCDprint("]=");
    tmp = GETINUM(TZONE);
    TCterminal.println("");
  } while (tmp < -12 || tmp > 12);
  if (inputstrlen != 0) {
    eecharbuf.strunion.DTZONE = TZONE = tmp;
  } else {
    eecharbuf.strunion.DTZONE = TZONE;
  }
  TCterminal.println("");
  TCterminal.println(" Is Daylight Savings Time currently in effect?");
  TCterminal.print(" DST IN? ["); TCterminal.print(DSTFLAG); TCterminal.print("]=");
  LCDclear(); LCDline1(); LCDprint("DST IN? ["); LCDprint(DSTFLAG); LCDprint("]=");
  DSTFLAG = GETYORN();
  eecharbuf.strunion.DSTFLAG = DSTFLAG;
  TCterminal.println("");
}

void RESETIME() {
  TERMclear(); 
  INITZONE();
  SETIMEFLAG();
}

void INIT() {
  // Initialize Telescope Mount
  // 1. Find Azimuth Reference
  // 2. Get Azimuth Range
  // 3. Check Azimuth Level
  // 4. Get True North location in Azimuth, if possible
  // - Update Azimuth zero location
  // 5. Get Horizon location in Altitude
  // 6. Get Range in Altitude by finding Zenith
  // 7. Check with referencwe star
}

long initIRkey(long previous) {
  // Get IR input three identical times to proceed
  //char n, tabn;
  long x1, x2, x3, xn;
  x1 = x2 = 0L; x3 = 1L;/*
  do {
    // Used to use: irrecv.decode(&results)
    if (irrecv.decode()) {
      // Return an IR input char if it has been repeated three times in a row
      //xn = results.value;
	  xn = irrecv.results.value;
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
  */
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
    TCterminal.print("\r Right    "); eecharbuf.strunion.IRinput[14] = initIRkey(eecharbuf.strunion.IRinput[13]); eecharbuf.strunion.IRchar[14] = 0x3e; // > greater than
    TCterminal.print("\r Left "); eecharbuf.strunion.IRinput[15] = initIRkey(eecharbuf.strunion.IRinput[14]); eecharbuf.strunion.IRchar[15] = 0x3c; // < less than
    TCterminal.print("\r Up  "); eecharbuf.strunion.IRinput[16] = initIRkey(eecharbuf.strunion.IRinput[15]); eecharbuf.strunion.IRchar[16] = 0x5e; // ^ Caret
    TCterminal.print("\r Down"); eecharbuf.strunion.IRinput[17] = initIRkey(eecharbuf.strunion.IRinput[16]); eecharbuf.strunion.IRchar[17] = 0x76; // v lowercase letter V
    TCterminal.print("\r Lock"); eecharbuf.strunion.IRinput[18] = initIRkey(eecharbuf.strunion.IRinput[17]); eecharbuf.strunion.IRchar[18] = 0x0d; // CR
    TCterminal.print("\r LCD next Screen"); eecharbuf.strunion.IRinput[19] = initIRkey(eecharbuf.strunion.IRinput[18]); eecharbuf.strunion.IRchar[19] = 'n';
    TCterminal.print("\r LCD Brighter"); eecharbuf.strunion.IRinput[20] = initIRkey(eecharbuf.strunion.IRinput[19]); eecharbuf.strunion.IRchar[20] = '+';
    TCterminal.print("\r LCD Dimmer"); eecharbuf.strunion.IRinput[21] = initIRkey(eecharbuf.strunion.IRinput[20]); eecharbuf.strunion.IRchar[21] = '=';
    TCterminal.print("\r Motors Faster"); eecharbuf.strunion.IRinput[22] = initIRkey(eecharbuf.strunion.IRinput[21]); eecharbuf.strunion.IRchar[22] = '>';
    TCterminal.print("\r Motors Slowe"); eecharbuf.strunion.IRinput[23] = initIRkey(eecharbuf.strunion.IRinput[22]); eecharbuf.strunion.IRchar[23] = '<';
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

void LCDOptionStateMsg(boolean flag) {
  //Print out Option state
  if (flag) {
    LCDprint("E");
  } else {
    LCDprint("D");
  }
}

void RESETTCI() {
  // Set up basic telescope interface parameters
  // i.e. Ranges, etc.
  char choice = 'A';

  do {
    TCterminal.println("\n Select Option to Enable/Disable");
    TCterminal.print(" 1) INA219 Voltage Monitor: ");    OptionStateMsg(eecharbuf.strunion.INA219flag);
    TCterminal.print(" 2) Motor Driver: ");    OptionStateMsg(eecharbuf.strunion.MotorDriverflag);
    TCterminal.print(" 3) Limit Switches: ");    OptionStateMsg(eecharbuf.strunion.NOLIM);
    TCterminal.print(" 4) Precession: ");    OptionStateMsg(eecharbuf.strunion.PFLAG);
    TCterminal.print(" 5) Refraction: ");    OptionStateMsg(eecharbuf.strunion.RFLAG);
    TCterminal.print(" 6) Site ID: ");
    TERMtextcolor('g');   TCterminal.println(eecharbuf.strunion.SiteID);    TERMtextcolor('w');
    TCterminal.println(" 0) Done - Return to Main Display");
    LCDclear();
    LCDline1(); LCDprint("1 Volt:"); LCDOptionStateMsg(eecharbuf.strunion.INA219flag);
    LCDprint(" 2 MTR:"); LCDOptionStateMsg(eecharbuf.strunion.MotorDriverflag);
    LCDline2(); LCDprint("3 Lim:"); LCDOptionStateMsg(eecharbuf.strunion.NOLIM);
    LCDprint(" 4 Prec:"); LCDOptionStateMsg(eecharbuf.strunion.PFLAG);
    LCDline3(); LCDprint("5 Ref:"); LCDOptionStateMsg(eecharbuf.strunion.RFLAG);
    LCDprint(" 0 Done"); 
    LCDline4(); LCDprint("6:");
    LCDprint(eecharbuf.strunion.SiteID); 
    choice = KEY();
    if (choice == '1') {
      eecharbuf.strunion.INA219flag = !(eecharbuf.strunion.INA219flag);
      if (eecharbuf.strunion.INA219flag) ina219.begin(); // Voltage monitor
    }
    if (choice == '2') eecharbuf.strunion.MotorDriverflag = !(eecharbuf.strunion.MotorDriverflag);
    if (choice == '3') eecharbuf.strunion.NOLIM = !(eecharbuf.strunion.NOLIM);
    if (choice == '4') eecharbuf.strunion.PFLAG = !(eecharbuf.strunion.PFLAG);
    if (choice == '5') eecharbuf.strunion.RFLAG = !(eecharbuf.strunion.RFLAG);
    if (choice == '6') {
      // Redefine Site ID
      TCterminal.print("\n Enter new Site ID: ");
      LCDclear();
      LCDline1(); LCDprint("Enter new Site ID:"); 
      LCDline2();
      inputstrlen = ACCEPT(eecharbuf.strunion.SiteID, 39); //Get 39 chars max
      TCterminal.println(" ");
    }
  } while (choice != '0');
  SETTCIFLAG();
}

void RESETdisplay() {
  // Set up Display options
  char choice = 'A';

  do {
    TCterminal.println("\n Select Option to Enable/Disable");
    TCterminal.print(" 1) OLED Display: ");    OptionStateMsg(eecharbuf.strunion.OLEDflag);
    TCterminal.print(" 2) PIC LCD Display: ");    OptionStateMsg(eecharbuf.strunion.LCDpicflag);
    TCterminal.print(" 3) I2C LCD Display: ");    OptionStateMsg(eecharbuf.strunion.LCDi2cflag);
    TCterminal.print(" 4) Serial Terminal Display: ");    OptionStateMsg(eecharbuf.strunion.SerialTermflag);
    TCterminal.println(" 0) Done - Return to Main Display");
    LCDclear();
    LCDline1(); LCDprint("0 Done 1 OLED:"); LCDOptionStateMsg(eecharbuf.strunion.OLEDflag);
    LCDline2(); LCDprint("2 PIC LCD:"); LCDOptionStateMsg(eecharbuf.strunion.LCDpicflag);
    if (eecharbuf.strunion.LCDpicflag) {
      LCDprint(" +-");
    }
    LCDline3(); LCDprint("3 I2C LCD:"); LCDOptionStateMsg(eecharbuf.strunion.LCDi2cflag);
    
    LCDline4(); LCDprint("4 Serial:"); LCDOptionStateMsg(eecharbuf.strunion.SerialTermflag);
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
    if ((choice == '-') && eecharbuf.strunion.LCDpicflag) {
      LCDbrightness--;
      if (LCDbrightness < 0x80) LCDbrightness = 0x80;
      TC_LCD.write(0x7C); //Send command character
      TC_LCD.write(LCDbrightness);
      newdelay(500);
    } else if ((choice == '+') && eecharbuf.strunion.LCDpicflag) {
      LCDbrightness++;
      if (LCDbrightness > 0x9D) LCDbrightness = 0x9D;
      TC_LCD.write(0x7C); //Send command character
      TC_LCD.write(LCDbrightness);
      newdelay(500);
    }
    if (choice == '4') {
      eecharbuf.strunion.SerialTermflag = !(eecharbuf.strunion.SerialTermflag);
    }
  } while ((choice != '0') || !(eecharbuf.strunion.SerialTermflag || eecharbuf.strunion.LCDpicflag || eecharbuf.strunion.LCDi2cflag));
  SETdisplayFLAG();
}

void topEEPROMwrite() {
  if ( tcEEPROM.write(2L, eecharbuf.ebuf, sizeof(EEstruct)) != 0 ) {
    //there was a problem
    TCterminal.println("EEPROM write error.");
    TCterminal.println(hitkey);
    LCDline4(); LCDprint("EEPROM write error ");
    KEY();
  }
  byte tmpbyte = EEcheckByte;
  tcEEPROM.write(0L, &tmpbyte, 1);
  printstatusscreen();
}

void doCommand() {
  // Process command key press from user
  int tmp = 0;
  long int Ltmp = 0L;
  screenCOMMAND = STAYFUNC;
  char num = KEY();
  if (num == 0x08) {
    // Initialization menu
    TERMclear();
    TCterminal.println(" Initialization menu");
    TCterminal.println(" 1 Time");
    TCterminal.println(" 2 Orientation");
    TCterminal.println(" 3 IR decode");
    TCterminal.println(" 4 TC Options");
    TCterminal.println(" 5 Display");
    LCDclear();
    LCDline1(); LCDprint("1 Time  2 Mount");
    LCDline2(); LCDprint("3 IR  4 Options");
    LCDline3(); LCDprint("5 Display");
    num = KEY();
    // Reinit on Backspace
    if (num == '1') RESETIMEFLAG();
    if (num == '2') RESETINITFLAG();
    if (num == '3') RESETIRFLAG();
    if (num == '4') RESETTCIFLAG();
    if (num == '5') RESETdisplayFLAG();
    if ((num < '1') || (num > '5')) {
      printstatusscreen();
      LCDclear();
    }
  } else if (num < '9' && num >= '0') {
    // Sun, Moon, or Planet
    COMMAND = num - '0';
    switch (COMMAND) {
      case 0: myAstro.doSun(); break;
      case 1: myAstro.doMercury(); break;
      case 2: myAstro.doVenus(); break;
      case 3: myAstro.doMoon(); myAstro.doLunarParallax(); break;
      case 4: myAstro.doMars(); break;
      case 5: myAstro.doJupiter(); break;
      case 6: myAstro.doSaturn(); break;
      case 7: myAstro.doUranus(); break;
      case 8: myAstro.doNeptune(); break;
    }
    TRA = myAstro.getRAdec();
    TDEC = myAstro.getDeclinationDec();
  } else if ( num == '9') {
    // Arbitrary
    Ltmp = -1L;
    TERMclear();

    TCterminal.println(" Input in hour.minute.second format:");
    do {
      LCDclear();  LCDline1(); LCDprint("RA=");
      TCterminal.print(" Right Ascension = ");
      Ltmp = INPUTHMS(); TCterminal.println("");
    } while (Ltmp < 0 || Ltmp > (24L * 3600L));
    if (tmp != -1L) {
      TRA = (double)Ltmp / 3600.; // Convert RA to hours
    }
    TCterminal.println(" ");
    TCterminal.println(" Input in degree.minute.second format:");
    Ltmp = -90L * 3600L;
    do {
      TCterminal.print(" Declination = ");
      LCDline1(); LCDprint("Dec=");
      Ltmp = INPUTHMS();
      TCterminal.println("");
    } while (Ltmp < (-90L * 3600L) || Ltmp > (90L * 3600L));
    if (tmp != -1L) {
      TDEC = (double)Ltmp / 3600.; // Convert Declination to hours
      COMMAND = '9' - '0';
    }
    printstatusscreen();
  } else if ( num == '.') {
    // Star
    TERMclear();
    LCDclear();  LCDline1();
    int j = 0;
    int k = (int)myObjects.printStarName(1);
    for (int i = 0; i <= myObjects.NSTARS; i++) {
      if ((int)myObjects.printStarName(i) != k) {
        TCterminal.print(i);
        TCterminal.print(" ");
        TCterminal.print(myObjects.printStarName(i));
        TCterminal.print("   ");
        j++;
      }
      if (j > 4) {
        j = 0;
        TCterminal.println("");
      }
    }
    TCterminal.println("");
    TCterminal.print(" Input Star number between 1 and ");
    TCterminal.println(myObjects.NSTARS);
    do {
      LCDclear();  LCDline1();  LCDprint("Star=");
      TCterminal.print(" Star Number = ");
      tmp = GETINUM(0);
      TCterminal.println("");
    } while (tmp <= 0 || tmp > myObjects.NSTARS);
    if (tmp != 0) {
      myObjects.selectStarTable(tmp);
      TRA = myObjects.getRAdec();
      TDEC = myObjects.getDeclinationDec();
      COMMAND = FNDARB;
    }
    printstatusscreen();
  } else if (num == '-') {
    // Non-stella object menu
    TERMclear();
    TCterminal.println(" Non-stellar Object menu");
    TCterminal.println(" 1 NGC");
    TCterminal.println(" 2 IC");
    TCterminal.println(" 3 Other Object table");
    TCterminal.println(" 4 Messier");
    TCterminal.println(" 5 Caldwell");
    TCterminal.println(" 6 Hershel 400");
    TCterminal.println(" 7 Return to Display Screen");
    LCDclear();  LCDline1(); LCDprint("1 NGC 2 IC  3 Other");
    LCDline2(); LCDprint("4 Messr  5 Caldwell");
    LCDline3(); LCDprint("6 Herschel 400");
    char num1 = KEY();

    if (num1 == '1') {
      do {
        LCDclear();  LCDline1(); LCDprint("NGC=");
        TCterminal.print(" NGC Number = ");
        tmp = GETDDECNUM();
        TCterminal.println("");
      } while (tmp < 0 || tmp > myObjects.NGCNUM);
      if (tmp != 0) {
        myObjects.selectNGCTable(tmp);
        TRA = myObjects.getRAdec();
        TDEC = myObjects.getDeclinationDec();
        COMMAND = FNDOBJECT;
      }
    }
    if (num1 == '2') {
      do {
        TCterminal.print(" IC Number = ");
        LCDclear();  LCDline1(); LCDprint("IC=");
        tmp = GETDDECNUM();
        TCterminal.println("");
      } while (tmp < 0 || tmp > myObjects.ICNUM);
      if (tmp != 0) {
        myObjects.selectICTable(tmp);
        TRA = myObjects.getRAdec();
        TDEC = myObjects.getDeclinationDec();
        COMMAND = FNDOBJECT;
      }
    }
    if (num1 == '3') {
      do {
        TCterminal.print(" Other Object Table Number = ");
        LCDclear();  LCDline1(); LCDprint("Other=");
        tmp = GETDDECNUM();
        TCterminal.println("");
      } while (tmp < 0 || tmp > 32000);
      if (tmp != 0) {
        if (myObjects.selectOtherObjectsTable(tmp)) {
          TRA = myObjects.getRAdec();
          TDEC = myObjects.getDeclinationDec();
          COMMAND = FNDOBJECT;
        }
      }
    }
    if (num1 == '4') {
      do {
        TCterminal.print(" Messier Number = ");
        LCDclear();  LCDline1(); LCDprint("Messier=");
        tmp = GETDDECNUM();
        TCterminal.println("");
      } while (tmp < 0 || tmp > 110);
      if (tmp != 0) {
        myObjects.selectMessierTable(tmp);
        TRA = myObjects.getRAdec();
        TDEC = myObjects.getDeclinationDec();
        COMMAND = FNDOBJECT;
      }
    }
    if (num1 == '5') {
      do {
        TCterminal.print(" Caldwell Number = ");
        LCDclear();  LCDline1(); LCDprint("Caldwell=");
        tmp = GETDDECNUM();
        TCterminal.println("");
      } while (tmp < 0 || tmp > 109);
      if (tmp != 0) {
        myObjects.selectCaldwellTable(tmp);
        TRA = myObjects.getRAdec();
        TDEC = myObjects.getDeclinationDec();
        COMMAND = FNDOBJECT;
      }
    }
    if (num1 == '6') {
      do {
        TCterminal.print(" Herschel 400 Number = ");
        LCDclear();  LCDline1(); LCDprint("Hershel=");
        tmp = GETDDECNUM();
        TCterminal.println("");
      } while (tmp < 0 || tmp > 400);
      if (tmp != 0) {
        myObjects.selectHershel400Table(tmp);
        TRA = myObjects.getRAdec();
        TDEC = myObjects.getDeclinationDec();
        COMMAND = FNDOBJECT;
      }
    }
    printstatusscreen();
  } else if (num == 'n') {
    LCDclear();
    LCDscreenNum++;
    if (LCDscreenNum > 3) LCDscreenNum = 0;
  }
}

// Main Telescope Controller Routines
void tcintro() { // Startup screen for user
  // Set up I2C and serial ports
  
  Serial.begin(115200); //May use VT100 compatible terminal emulator - Full Menus
  //while (!Serial); //Wait for user to open terminal
  newdelay(2000);
  Serial.println(" u-blox compatibility mode");
  Wire.begin(); // I2C init
  
  Serial1.begin(9600); // use serial 4x20 LCD display - Easy to mount onto telescope
  
  //pinPeripheral(2, PIO_SERCOM); // Define Pins for Serial2 UART port
  //pinPeripheral(3, PIO_SERCOM_ALT);
  //Serial2.begin(9600); //Or put VT100 compatible terminal emulator - Full Menus here
  
  newdelay(1600); // Give time for SerialUSB to init
  TERMclear(); //Clear terminal screen and home cursor
  TERMtextcolor('g');
  TCterminal.println("\nArduino SAMD Telescope Controller");
  TCterminal.println("(c) 2022, MIT License");
  TERMtextcolor('r');
  TCterminal.println("\nThis software provided as-is without warranty of any kind.");
  TCterminal.println("The User assumes all responsibility for use of this");
  TCterminal.println("program and all related documentation files.");
  TERMtextcolor('w');
  TCterminal.print("\nInitializing system for ");
  TCterminal.print("Alt-Azimuth");
  TCterminal.println(" mounted telescopes.");
  TCterminal.print("\nVersion ");
  TCterminal.println(tcversion);
  TCterminal.println("");

  // This was the first half of the topmost Forth word TC - used for initialization
  TERMtextcolor('c');
  inithardware(); //See what we have out there connected, and set it up

  /* Site ID string copy to sized buffer (overflow safe): */
  strncpy ( siteid, eecharbuf.strunion.SiteID, sizeof(eecharbuf.strunion.SiteID) );

  //Handle Lock btn status on Reset
  if (lockbtninitflag) { //Lock button pressed on reset?
    TCterminal.println("\nLock button pressed on reset.  Forcing reinitialization.");
    LCDline2();
    LCDprint("TC SYSTEM RESET");
    RESETIMEFLAG(); RESETINITFLAG(); // Actual re-initialization done in TC_main()
    WAITASEC(3); // Let user know what we've done
  }
  // make it so it doesn't assume errors on start
  if (NOTIMESETquestion() && !INITFLAG) {
    RESETERRFLAG();
  }

  WAITASEC(2);
  LCDline3();
  LCDprint(hitkey);
  LCDscreenNum = 3;
  TERMtextcolor('w');
  TCterminal.println(hitkey);
  KEY();
  TCterminal.println("    ");
  printstatusscreen();
  COMMAND = STAYFUNC; // Dummy command value to start
  screenCOMMAND = COMMAND;
}

void TC_main() {
  // This is the second half of the old Forth word TC
  // We do telescope initialization here to allow for re-initialization by user selection
  boolean callscreenredraw = false;
  if (NOTIMESETquestion()) {
    RESETIME();
    topEEPROMwrite();
    callscreenredraw = true;
  }
  if (NOTINITquestion()) {
    //INIT(); // Initialize telescope
    //topEEPROMwrite();
    callscreenredraw = true;
  }
  if (IRquestion()) {
    RESETIR(); // Initialize IR remote key decoding
    topEEPROMwrite();
    callscreenredraw = true;
  }
  if (TCIquestion()) {
    RESETTCI(); // Initialize telescope options
    topEEPROMwrite();
    callscreenredraw = true;
  }
  if (Displayquestion()) {
    RESETdisplay(); // Initialize display options
    topEEPROMwrite();
    callscreenredraw = true;
  }
  // Main processing loop
  updatestatusscreen(); // Update Status screen
  updatestatusLCD();
  oledprintData(); //Show it on a quick display, if present
  if (CHKNUM()) {
    doCommand();
  }
}
