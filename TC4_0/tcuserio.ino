/* Telescope Controller 4.00.00 - User I/O functions
// September 2022
// See MIT LICENSE.md file and ReadMe.md file for essential information
// Highly tailored to the Sparkfun Redboard Turbo or AdaFruit M4 Metro
// DO NOT ATTEMPT TO LOAD THIS ONTO A STANDARD UNO */

#include "tcheader.h"

void oledprintData() {
  if (eecharbuf.strunion.OLEDflag) {
    //oled.clear(PAGE);
    oled.setFontType(0);
    oled.setCursor(0, 0);
    if (BMEpresent) {
      oled.print(FTEMPF, 1);
      oled.print(" F ");
      oled.print(FHUMID, 0);
      oled.print("%");
      oled.print(FPINHG, 2);
      oled.print(" InHg");
    }
    if (getMagCompassPresent()) {
      oled.print(FMAGHDG, 1);
      oled.print(" Hdg \n");
    }
    // ...and time
    print2digits((int)myAstro.getLT());
    oled.print(":");
    DateTime now = rtczero.now();
    print2digits(now.minute());
    oled.print(":");
    print2digits(now.second());
    oled.display();
  }
}

void printstatusscreen() {
  LCDclear();
  if (eecharbuf.strunion.OLEDflag) {
    oled.begin();
    oled.clear(ALL);
    oled.display();
  }
  myAstro.setTimeZone(TZONE);
  if (DSTAUTOFLAG) DSTFLAG = myAstro.useAutoDST();
  TERMclear();
  TERMcursor();
  TERMtextcolor('g');
  TCterminal.print("Arduino SAMD Telescope Controller ");
  TCterminal.println(tcversion);
  TERMtextcolor('r');
  TCterminal.println("Site ID:"); // Default");
  TCterminal.print("GMT         GMT      Time       GPS: ");
  //if (!GPSpresent) TCterminal.print("Not ");
  TCterminal.println("Detected");
  TCterminal.println("Date        Time     Zone  DST  Latitude      Longitude       Elevation\n");
  //  01/01/2020 10:10:10 PST   -32* 32' 32"  110* 32' 32" W
  TCterminal.println("Local Time: 00:00:00   Local Siderial Time:");
  if (BMEpresent) {
    if (MotorDriverflag && eecharbuf.strunion.enableRealHwInit) {
		TCterminal.println("Temperature  Humidity  Pressure    Altitude");
    } else {
    TCterminal.println("Temperature  Humidity  Pressure    Altitude    Compass Hdg");
	}
    //  111.1* F     48.7%     29.92" Mg
    TERMtextcolor('w'); printBME280(); TCterminal.println(""); TERMtextcolor('r');
  } else if (getMagCompassPresent()) {
    TCterminal.println("                                               Compass Hdg\n");
  } else {
    TCterminal.println(" \n");
  }
  const char positionmsg1[] = "                     Right                    Encoder Counts %";
  const char positionmsg2[] = "Azimuth       Altitude      Ascension   Declination  Azimuth Altitude\n";
  TCterminal.print("Current");
  TCterminal.println(positionmsg1);
  TCterminal.println(positionmsg2);
  //270^ 40' 33"  89* 33' 20"   22h 33m 21s -22^ 22' 12'  80.1    50.3
  TCterminal.println("Current Object:\n");

  TCterminal.print("Target ");
  TCterminal.println(positionmsg1);
  // TCterminal.println("Azimuth       Altitude      Ascension   Declination  Azimuth Altitude\n");
  TCterminal.println(positionmsg2);
  //270^ 40' 33"  89* 33' 20"  22h 33m 21s -22^ 22' 12'  80.1    50.3
  TCterminal.println("Target Object:");
  TCterminal.println("Current Diffs from Target:"); //  V xxxx  > xxxx");
  //TCterminal.println("Maximum Ranges       Base Tilt     Tube Tilt      Power    Magnetic");
  //TCterminal.println("Azimuth   Altitude   X      Y      X      Y       Voltage  Variation");
  if (!(MotorDriverflag && eecharbuf.strunion.enableRealHwInit)) {
  TCterminal.println("Maximum Ranges       Base Tilt     Tube Tilt      Power    Current  Magnetic");
  TCterminal.println("Azimuth   Altitude   X      Y      X      Y       Voltage  (mA)     Variation");
  //16000000  16000000   100000    0.0  0.0    45.3 0.0    12.12
  } else {
  TCterminal.println("Maximum Ranges                                    Power    Current  Magnetic");
  TCterminal.println("Azimuth   Altitude                                Voltage  (mA)     Variation");
  }
  TERMALLOPTIONS();
  //TCterminal.println("Command:  0-8 Sun, Moon, Planet   . Star   - Non-Stellar (Scan)");
  //TCterminal.print("          9 Arbitrary   <Enter> Goto   <bckspc> Initialize");
  //TERMtextcolor('w');
  TERMxy(10, 2);
  //TCterminal.print(siteid); //Site ID
  TCterminal.println(eecharbuf.strunion.SiteID);
  TERMxy(0, 5); showDate(); //Print date
  //Time Zone
  TERMxy(22, 5);
  switch (TZONE) {
    case -11: TCterminal.print("SST"); break;
    case -10: TCterminal.print("HWI"); break;
    case -9:  TCterminal.print("AK"); break;
    case -8:  TCterminal.print("PST"); break;
    case -7:  TCterminal.print("MST"); break;
    case -6:  TCterminal.print("CST"); break;
    case -5:  TCterminal.print("EST"); break;
    case -4:  TCterminal.print("AST"); break;
    case -3:  TCterminal.print("ART"); break;
    case -1:  TCterminal.print("AZOT"); break;
    case 0:   TCterminal.print("UTC"); break;
    case 1:   TCterminal.print("CET"); break;
    case 2:   TCterminal.print("CAT"); break;
    case 3:   TCterminal.print("TRT"); break;
    case 4:   TCterminal.print("RET"); break;
    case 5:   TCterminal.print("YEKT"); break;
    case 6:   TCterminal.print("OMST"); break;
    case 7:   TCterminal.print("WIB"); break;
    case 9:   TCterminal.print("TLT"); break;
    case 10:  TCterminal.print("AET"); break;
    case 11:  TCterminal.print("NFT"); break;
    case 12:  TCterminal.print("NZST"); break;
    case 13:  TCterminal.print("TOT"); break;
  }
  TERMxy(28, 5); // Daylight Savings time flag
  if (DSTFLAG) {
    TCterminal.print("Yes");
  } else {
    TCterminal.print("No");
  }
  // Latitude, Longitude
  FLATITUDE = eecharbuf.strunion.FLATITUDE = ubloxGPS.getLatitude() / 10000000.;
  TERMxy(33, 5); printDegMinSecs(fabs(FLATITUDE));
  if (FLATITUDE > 0 ) {
    TCterminal.print("N ");
  } else {
    TCterminal.print("S ");
  }
  FLONGITUDE = eecharbuf.strunion.FLONGITUDE = ubloxGPS.getLongitude() / 10000000.;
  TERMxy(47, 5); printDegMinSecs(fabs(FLONGITUDE));
  if (FLONGITUDE > 0 ) {
    TCterminal.print("E ");
  } else {
    TCterminal.print("W ");
  }

  //Maximum Encoder Counts in Azimuth, Altitude
  TERMxy(0, 21); TCterminal.print(eecharbuf.strunion.RRAAZ);
  TERMxy(11, 21); TCterminal.print(eecharbuf.strunion.RDECAL);
  FAZIMUTH = -1.; // purposely bad values
  FALTITUDE = -91.;
  previousTRA = TRA + 1.;
  previousTDEC = TDEC + 1.;
  magVariation = -90.;
  screenDecRefresh = true;
  TAZIMUTH = -1.;
  TALTITUDE = -1.;
}

void showDate() {
  // Print date...
  print2digitsUSB(GRMONTH);
  TCterminal.print("/");
  print2digitsUSB(GRDAY);
  TCterminal.print("/");
  print2digitsUSB(GRYEAR);
}

void showDateLCD() {
  // Print date...
  print2digitsLCD(GRMONTH);
  LCDprint("/");
  print2digitsLCD(GRDAY);
  LCDprint("/");
  print2digitsLCD(GRYEAR);
}

void printObject() {
  //TCterminal.print("Identified Object: ");
  switch (myObjects.getIdentifiedObjectTable()) {
    case (1):
      TCterminal.print("Star "); break;
    case (2):
      TCterminal.print("NGC "); break;
    case (3):
      TCterminal.print("IC "); break;
    case (7):
      TCterminal.print("Other "); break;
  }
  TCterminal.print(myObjects.getIdentifiedObjectNumber());
  TCterminal.print(" ");
  if (myObjects.getIdentifiedObjectTable() == 1) {
    TCterminal.print(myObjects.printStarName(myObjects.getIdentifiedObjectNumber()));
  }
  if (myObjects.getAltIdentifiedObjectTable()) {
    //TCterminal.print("Alternate Object Identification: ");
    switch (myObjects.getAltIdentifiedObjectTable()) {
      case (4):
        TCterminal.print("Messier "); break;
      case (5):
        TCterminal.print("Caldwell "); break;
      case (6):
        TCterminal.print("Herschel 400 number "); break;
    }
    TCterminal.print(myObjects.getAltIdentifiedObjectNumber());
  } else {
    TCterminal.print("                        ");
  }
}

void printObjectLCD() {
  //TCterminal.print("Identified Object: ");
  switch (myObjects.getIdentifiedObjectTable()) {
    case (1):
      LCDprint("Star "); break;
    case (2):
      LCDprint("NGC "); break;
    case (3):
      LCDprint("IC "); break;
    case (7):
      LCDprint("Other "); break;
  }
  LCDprint(myObjects.getIdentifiedObjectNumber());
  LCDprint(" ");
  if (myObjects.getIdentifiedObjectTable() == 1) {
    LCDprint(myObjects.printStarName(myObjects.getIdentifiedObjectNumber()));
  }
  if (myObjects.getAltIdentifiedObjectTable()) {
    //TCterminal.print("Alternate Object Identification: ");
    switch (myObjects.getAltIdentifiedObjectTable()) {
      case (4):
        LCDprint("Mesr "); break;
      case (5):
        LCDprint("Cldwll "); break;
      case (6):
        LCDprint("Hrschl "); break;
    }
    LCDprint(myObjects.getAltIdentifiedObjectNumber());
  }
}

void printBME280() {
  double tmptemp, tmphumid, tmppinhg, tmpheight;
  tmptemp = bme280.readTempF();
  tmphumid = bme280.readFloatHumidity();
  tmppinhg = (bme280.readFloatPressure() * 0.0002953);
  tmpheight = bme280.readFloatAltitudeFeet();
  if (FTEMPF != tmptemp) {
    TERMxy(0, 8); // Temperature
    FTEMPF = tmptemp;
    TCterminal.print(FTEMPF);
    TCterminal.print(" F ");
  }
  if (FHUMID != tmphumid) {
    TERMxy(14, 8);
    FHUMID = tmphumid;
    TCterminal.print(FHUMID);
    TCterminal.print("% ");
  }
  if (FPINHG != tmppinhg) {
    FPINHG = tmppinhg;
    TERMxy(24, 8);
    TCterminal.print(FPINHG);
    TCterminal.print(" InHg ");
  }
  if (FHEIGHT != tmpheight) {
    FHEIGHT = tmpheight;
    TERMxy(36, 8);
    TCterminal.print(FHEIGHT);
    TCterminal.print(" ft  ");
  }
}

void printBME280LCD() {
  double tmptemp, tmphumid, tmppinhg, tmpheight;
  tmptemp = bme280.readTempF();
  tmphumid = bme280.readFloatHumidity();
  tmppinhg = (bme280.readFloatPressure() * 0.0002953);
  tmpheight = bme280.readFloatAltitudeFeet();
  
  {
    LCDline1();
    LCDprint("Temp: "); // Temperature
    FTEMPF = tmptemp;
    LCDprint(FTEMPF, 2);
    LCDprint(" F ");
  }
  {
    FPINHG = tmppinhg;
    LCDline2();
    LCDprint("Pres: ");
    LCDprint(FPINHG, 2);
    LCDprint(" InHg ");
  }
  {
    LCDline3();
    LCDprint("Humid: ");
    FHUMID = tmphumid;
    LCDprint(FHUMID, 2);
    LCDprint("% ");
  }
  {
    FHEIGHT = tmpheight;
    LCDline4();
    LCDprint("Alt: ");
    LCDprint(FHEIGHT, 2);
    LCDprint(" ft  ");
  }
}

void updatestatusscreen() {
  double ftmp, ftmp2, ftmplat, ftmplong, ftmpalt;
  int itmp;
  byte tmpminutes;
  boolean tmpflag, azimuthchanged, altitudechanged;
  float tmpMagVariation = magVariation;

  azimuthchanged = false;
  altitudechanged = false;
  ftmplat = FLATITUDE;
  FLATITUDE = eecharbuf.strunion.FLATITUDE = ubloxGPS.getLatitude() / 10000000.;
  ftmplong = FLONGITUDE;
  FLONGITUDE = eecharbuf.strunion.FLONGITUDE = ubloxGPS.getLongitude() / 10000000.;
  ftmpalt = FALTEMP;
  FALTEMP = eecharbuf.strunion.ELEVATION = ubloxGPS.getAltitude() * 0.00328084; //Elevation in feet

  // Update data on status screen
  itmp = (byte)ubloxGPS.getDay();
  if (eecharbuf.strunion.SerialTermflag) TCterminal.print("  ");
  tmpflag = DSTFLAG;
  if (GRDAY != itmp) {
    GRDAY = itmp;
    GRYEAR = (int)ubloxGPS.getYear();
    GRMONTH = (byte)ubloxGPS.getMonth();
    myAstro.setGMTdate(GRYEAR, GRMONTH, GRDAY);
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(0, 5); showDate(); //Print date
    }
    //tmpflag = myAstro.useAutoDST();
    //tmpMagVariation = magneticDeclination(FLATITUDE, FLONGITUDE, GRYEAR, GRMONTH, GRDAY);
  }
  tmpMagVariation = magneticDeclination(FLATITUDE, FLONGITUDE, GRYEAR, GRMONTH, GRDAY);
  if (DSTAUTOFLAG) tmpflag = myAstro.useAutoDST();
  DateTime now = rtczero.now();
  rtchours = (byte)now.hour();
  tmpminutes = rtcmin;
  rtcmin = (byte)now.minute();
  rtcseconds = now.second();

  // Print Time(s)
  myAstro.setGMTtime(rtchours, rtcmin, rtcseconds);
  if (eecharbuf.strunion.SerialTermflag) {
    TERMxy(13, 5); //printTime(myAstro.getGMT());
    print2digitsUSB(rtchours); TCterminal.print(":");
    print2digitsUSB(rtcmin); TCterminal.print(":");
    print2digitsUSB(rtcseconds); TCterminal.print(" ");
  }
  if (tmpflag != DSTFLAG) {
    DSTFLAG = eecharbuf.strunion.DSTFLAG = tmpflag;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(28, 5); // Daylight Savings time flag
      if (DSTFLAG) {
        TCterminal.print("Yes");
      } else {
        TCterminal.print("No");
      }
    }
  }

  // Latitude, Longitude
  if (fabs(ftmplat - FLATITUDE) > 0.001) {
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(33, 5); printDegMinSecs(fabs(ftmplat));
      if (ftmplat > 0 ) {
        TCterminal.print("N ");
      } else {
        TCterminal.print("S ");
      }
    }
    //FLATITUDE = ftmplat;
    myAstro.setLatLong(FLATITUDE, FLONGITUDE);
  }
  if (fabs(ftmplong - FLONGITUDE) > 0.001) {
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(47, 5); printDegMinSecs(fabs(ftmplong));
      if (ftmplong > 0 ) {
        TCterminal.print("E ");
      } else {
        TCterminal.print("W ");
      }
    }
    //FLONGITUDE = ftmplong;
    myAstro.setLatLong(FLATITUDE, FLONGITUDE);
  }
  //*
  // Elevation from GPS
  if (fabs(ftmpalt - FALTEMP) > 0.5) {
    myAstro.setElevationF(FALTEMP);
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(63, 5); TCterminal.print(FALTEMP);
    }
  }

  // LT, LST
  if (eecharbuf.strunion.SerialTermflag) {
    TERMxy(13, 6); printTime(myAstro.getLT());
    TERMxy(45, 6); printTime(myAstro.getLocalSiderealTime());
  }

  if (BMEpresent && (tmpminutes != rtcmin)) {
    // Only update once a minute, to reduce number of screen writes
    printBME280();
  }
  if (!(MotorDriverflag && eecharbuf.strunion.enableRealHwInit)) {
  if (getMagCompassPresent()) {
    ftmp = getMagCompassHeading();
    if (FMAGHDG != ftmp) {
      FMAGHDG = ftmp;
      if (eecharbuf.strunion.SerialTermflag) {
        TERMxy(48, 8); TCterminal.print(FMAGHDG, 1); TCterminal.print("  ");
      }
    }
  }
  if (MMC5983MAMagCompasspresent) {
    ftmp = getMMC5983MagCompassHeading();
    if (FMAGHDG != ftmp) {
      FMAGHDG = ftmp;
      if (eecharbuf.strunion.SerialTermflag) {
        TERMxy(48, 8); TCterminal.print(FMAGHDG, 1); TCterminal.print("  ");
      }
    }
  }
  }
  //*
  //Current position
  // print Azimuth, Altitude
  ftmp = getAzimuth();
  if (FAZIMUTH != ftmp) {
    FAZIMUTH = ftmp;
    azimuthchanged = true;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(0, 11); printDegMinSecs(FAZIMUTH);
    }
  }
  ftmp = getAltitude();
  if (FALTITUDE != ftmp) {
    FALTITUDE = ftmp;
    altitudechanged = true;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(15, 11); printDegMinSecs(FALTITUDE);
    }
  }
  
  myAstro.setAltAz(FALTITUDE, FAZIMUTH);
  // Refraction correction
  if (BMEpresent && eecharbuf.strunion.RFLAG && (azimuthchanged || altitudechanged)) {
    myAstro.doAntiRefractionF(FPINHG, FTEMPF);
  }
  myAstro.doAltAz2RAdec();
  //newdelay(10);
  // print Right Ascension, Declination
  ftmp = myAstro.getRAdec();
  if (FRA != ftmp) {
    FRA = ftmp;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(29, 11); //TCterminal.print(FRA);
      printDegMinSecs(FRA);
      TCterminal.print("  ");
    }
  }
  ftmp = myAstro.getDeclinationDec();
  if ((FDEC != ftmp) || screenDecRefresh) {
    FDEC = ftmp;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(41, 11); printDegMinSecs(FDEC);
    }
    //screenDecRefresh = false;
  }
  if (azimuthchanged) {
    float encoderpercent = (float)RAAZ * 100. / (float)RRAAZ;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(55, 11); TCterminal.print(encoderpercent);
    }
  }
  if (altitudechanged) {
    float encoderpercent = (float)DECAL * 100. / (float)RDECAL;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(63, 11); TCterminal.print(encoderpercent);
    }
  }
  
  myObjects.setRAdec(FRA, FDEC);
  myObjects.identifyObject();
  // print Object designation that is closest to this position
  if (eecharbuf.strunion.SerialTermflag) {
    TERMxy(17, 12); printObject();
  }

  //*
  // print target positions
  azimuthchanged = false;
  altitudechanged = false;
  if (COMMAND == STAYFUNC ) {
    TRA = FRA; TDEC = FDEC;
  }
  myAstro.setRAdec(TRA, TDEC);
  switch (COMMAND) {
    case 00: myAstro.doSun(); break;
    //case 01: myAstro.doMercury(); break;
    //case 02: myAstro.doVenus(); break;
    case 03: myAstro.doMoon(); myAstro.doLunarParallax(); break;
    //case 04: myAstro.doMars(); break;
    //case 05: myAstro.doJupiter(); break;
    //case 06: myAstro.doSaturn(); break;
    //case 07: myAstro.doUranus(); break;
    //case 0x08: myAstro.doNeptune(); break;
    case STAYFUNC: myAstro.setRAdec(FRA, FDEC); break;
  }
  myAstro.doRAdec2AltAz();
  // Refraction correction
  if (BMEpresent && eecharbuf.strunion.RFLAG) {
    myAstro.doRefractionF(FPINHG, FTEMPF);
  }
  ftmp = myAstro.getAzimuth();
  if (TAZIMUTH != ftmp) {
    TAZIMUTH = ftmp;
    azimuthchanged = true;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(0, 16); printDegMinSecs(TAZIMUTH);
    }
  }
  ftmp = myAstro.getAltitude();
  if (TALTITUDE != ftmp) {
    TALTITUDE = ftmp;
    altitudechanged = true;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(15, 16); printDegMinSecs(TALTITUDE);
    }
  }

  if (TRA != previousTRA) {
    previousTRA = TRA;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(29, 16); printDegMinSecs(TRA);
    }
  }
  if (TDEC != previousTDEC) {
    previousTDEC = TDEC;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(41, 16); printDegMinSecs(TDEC);
    }
  }
  if (azimuthchanged) {
    float encoderpercent = (float)TAZIMUTH / 3.600;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(55, 16); TCterminal.print(encoderpercent);
    }
  }
  if (altitudechanged) {
    float encoderpercent = (float)TALTITUDE / .900;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(63, 16); TCterminal.print(encoderpercent);
    }
  }
  if (eecharbuf.strunion.SerialTermflag) {
    myObjects.setRAdec(TRA, TRA);
    //myObjects.identifyObject();
    if (screenCOMMAND != COMMAND) {
      screenCOMMAND = COMMAND;
      TERMxy(17, 17);
      if (COMMAND <= 9 && COMMAND >= 0) {
        PRPLANET(COMMAND);
      } else {
        myObjects.identifyObject();
        // print Object designation that is closest to this position
        printObject();
      }
    }
  }

  if (!(MotorDriverflag && eecharbuf.strunion.enableRealHwInit)) {
  if (getRockerTiltPresent()) {
    if (rockerTilt.isConnected()) {
      //Get next block of data from sensor
      if (rockerTilt.available()) {
        if (eecharbuf.strunion.SerialTermflag) {
          TERMxy(22, 21);
          // print Base tilt
          TCterminal.print(rockerTilt.getTiltLevelOffsetAngleX()-eecharbuf.strunion.FtiltXrockeroff);
          TCterminal.print("  ");
          TERMxy(29, 21);
          TCterminal.print(rockerTilt.getTiltLevelOffsetAngleY()-eecharbuf.strunion.FtiltYrockeroff);
          TCterminal.print("  ");
        }
      } else rockerTilt.reset();
    } else rockerTilt.reset();
  }
  if (getTubeTiltPresent()) {
    if (tubeTilt.isConnected()) {
      //Get next block of data from sensor
      if (tubeTilt.available()) {
        if (eecharbuf.strunion.SerialTermflag) {
          TERMxy(36, 21);
          // print tube tilt
          TCterminal.print(tubeTilt.getTiltLevelOffsetAngleX());
          TCterminal.print("  ");
          TERMxy(43, 21);
          TCterminal.print(tubeTilt.getTiltLevelOffsetAngleY());
          TCterminal.print("  ");
        }
      } else tubeTilt.reset();
    } else tubeTilt.reset();
  }
  }
  if (eecharbuf.strunion.INA219flag && (tmpminutes != rtcmin)) {
    // Only update once a minute to reduce needed screen writes
    // Compute load voltage
    ftmp = ina219.getBusVoltage_V() + (ina219.getShuntVoltage_mV() / 1000.);
    ftmp2 = ina219.getCurrent_mA();
    if (busvolts != ftmp || current_mA != ftmp2) {
      busvolts = ftmp;
      current_mA = ftmp2;
      if (eecharbuf.strunion.SerialTermflag) {
        TERMxy(52, 21);
        TCterminal.print(busvolts, 2); // Voltage monitor
        TCterminal.print(" ");
        TERMxy(60, 21);
        TCterminal.print(current_mA, 2); // Current monitor
        TCterminal.print(" ");
      }
    }
  }
  if (magVariation != tmpMagVariation) {
    magVariation = tmpMagVariation;
    if (eecharbuf.strunion.SerialTermflag) {
      TERMxy(69, 21); TCterminal.print(magVariation);
      TCterminal.print("  ");
    }
	magVariationInAzimuthCounts = magVariation * ((double)RRAAZ) / 360.0;
    screenDecRefresh = false;
  }
}

void updatestatusLCD() {
  //LCD status screens
  if (LCDscreenNum == 0) {
    // Print date, time stuff on LCD
    LCDline1();
    print2digitsLCD(rtchours); LCDprint(":");
    print2digitsLCD(rtcmin); LCDprint(":");
    print2digitsLCD(rtcseconds); LCDprint(" ");
    showDateLCD();
    LCDline2();
    //printTimeLCD(myAstro.getLT());
    LCDprint("LST: ");
    printTimeLCD(myAstro.getLocalSiderealTime());
    LCDline3();
    LCDprint("Lat: ");
    printDegMinSecsLCD(fabs(FLATITUDE));
    if (FLATITUDE > 0. )
      LCDprint(" N");
    else
      LCDprint(" S");
    LCDline4();
    LCDprint("Long:");
    printDegMinSecsLCD(fabs(FLONGITUDE));
    if (FLONGITUDE > 0. )
      LCDprint(" E");
    else
      LCDprint(" W");
  } else if (LCDscreenNum == 1) {
    // Current Position
    LCDline1();
    LCDprint("Az/Al Current RA/Dec");
    LCDline2();
    printDegMinSecsLCD((FAZIMUTH));
    printDegMinSecsLCD((FRA));
    LCDline3();
    printDegMinSecsLCD((FALTITUDE));
    printDegMinSecsLCD((FDEC));
    LCDline4();
    myObjects.setRAdec(FRA, FDEC);
    myObjects.identifyObject();
    printObjectLCD();
  } else if (LCDscreenNum == 2) {
    // Target Position
    LCDline1();
    LCDprint("Az/Alt Target RA/Dec");
    LCDline2();
    printDegMinSecsLCD((TAZIMUTH));
    printDegMinSecsLCD((TRA));
    LCDline3();
    printDegMinSecsLCD((TALTITUDE));
    printDegMinSecsLCD((TDEC));
    LCDline4();
    myObjects.setRAdec(TRA, TDEC);
    myObjects.identifyObject();
    if (COMMAND <= 9 && COMMAND >= 0) {
      PRPLANETlcd(COMMAND);
    } else {
      myObjects.identifyObject();
      // print Object designation that is closest to this position
      printObjectLCD();
    }
  } else if (LCDscreenNum == 3) {
    LCDline1();
    LCDprint("Cmds: <Enter> Goto");
    LCDline2();
    LCDprint("0-8 Sun Moon Planet");
    LCDline3();
    LCDprint("9 Arbitrary  . Star");
    LCDline4();
    LCDprint("- Nebulae  <- Init");
  } else if (LCDscreenNum == 4) {
    if (BMEpresent) {
      printBME280LCD();
    } else {
      LCDscreenNum = 0;
    }
  }
}

void TERMALLOPTIONS() {
  TERMxy(0, 22);
  TERMtextcolor('r');
  TCterminal.println("Command:  0-8 Sun, Moon, Planet   . Star   - Non-Stellar");
  TCterminal.print(  "          9 Arbitrary   <Enter> Goto   <bckspc> Initialize");
  TERMtextcolor('w');
}

void PRPLANET( int n ) {
  // print planet name
  switch (n) {
    case 0: TCterminal.print("THE SUN  "); break;
    case 1: TCterminal.print("MERCURY  "); break;
    case 2: TCterminal.print("VENUS    "); break;
    case 3: TCterminal.print("THE MOON "); break;
    case 4: TCterminal.print("MARS     "); break;
    case 5: TCterminal.print("JUPITER  "); break;
    case 6: TCterminal.print("SATURN   "); break;
    case 7: TCterminal.print("URANUS   "); break;
    case 8: TCterminal.print("NEPTUNE  "); break;
    case 9: TCterminal.print("ARBITRARY"); break;
  }
  TCterminal.print("                        ");
}

void PRPLANETlcd( int n ) {
  // print planet name
  switch (n) {
    case 0: LCDprint("THE SUN  "); break;
    case 1: LCDprint("MERCURY  "); break;
    case 2: LCDprint("VENUS    "); break;
    case 3: LCDprint("THE MOON "); break;
    case 4: LCDprint("MARS     "); break;
    case 5: LCDprint("JUPITER  "); break;
    case 6: LCDprint("SATURN   "); break;
    case 7: LCDprint("URANUS   "); break;
    case 8: LCDprint("NEPTUNE  "); break;
    case 9: LCDprint("ARBITRARY"); break;
  }
  TCterminal.print("                        ");
}
