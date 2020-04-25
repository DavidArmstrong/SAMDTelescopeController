// TCUSERIO
// Telescope Controller 4.0 - User I/O functions
#include "tcheader.h"

void oledprintData() {
  if(eecharbuf.strunion.OLEDflag) {
    //oled.clear(PAGE);
    oled.setFontType(0);
    oled.setCursor(0,0);
    if (BMEpresent) {
      oled.print(FTEMPF, 1);
      oled.print(" F ");
      oled.print(FHUMID, 0);
      oled.print("%");
      oled.print(FPINHG, 2);
      oled.print(" InHg");
    }
    if (MagCompasspresent) {
      oled.print(FMAGHDG, 1);
      oled.print(" Hdg \n");
    }
    // ...and time
    print2digits(rtczero.getHours());
    oled.print(":");
    print2digits(rtczero.getMinutes());
    oled.print(":");
    print2digits(rtczero.getSeconds());
    oled.display();
  }
}

void print2digits(int number) {
  if (number < 10) {
    oled.print("0"); // print a 0 before if the number is < than 10
  }
  oled.print(number);
}

void print2digitsUSB(int number) {
  if (number < 10) {
    TCterminal.print("0");
  }
  TCterminal.print(number);
}

void printstatusscreen() {
  TERMclear();
  TERMcursor();
  TERMtextcolor('g');
  TCterminal.print("Arduino SAMD Telescope Controller ");
  TCterminal.println(tcversion);
  TERMtextcolor('r');
  TCterminal.println("Site ID:"); // Default");
  TCterminal.print("GMT         GMT      Time       GPS: ");
  if (!GPSpresent) TCterminal.print("Not ");
  TCterminal.println("Detected");
  TCterminal.println("Date        Time     Zone  DST  Latitude      Longitude       Elevation\n");
  //  01/01/2020 10:10:10 PST   -32* 32' 32"  110* 32' 32" W
  TCterminal.println("Local Time: 00:00:00   Local Siderial Time:");
  if (BMEpresent) {
    TCterminal.println("Temperature  Humidity  Pressure    Altitude    Compass Hdg\n");
  //  111.1* F     48.7%     29.92" Mg
  } else if (MagCompasspresent) {
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
  TCterminal.println("Maximum Ranges       Azimuth   Base Tilt     Tube Tilt   Power");
  TCterminal.println("Azimuth   Altitude   Offset    X      Y      X    Y      Voltage\n");
//16000000  16000000   100000    0.0  0.0    45.3 0.0    12.12
  TERMALLOPTIONS();
  //TCterminal.println("Command:  0-8 Sun, Moon, Planet   . Star   - Non-Stellar (Scan)");
  //TCterminal.print("          9 Arbitrary   <Enter> Goto   <bckspc> Initialize");
  //TERMtextcolor('w');
  TERMxy(10,2);
  //TCterminal.print(siteid); //Site ID
  TCterminal.println(eecharbuf.strunion.SiteID);
  //Time Zone
  //int tztmp = TZONE - (DSTFLAG?1:0);
  TERMxy(22,5);
  switch (TZONE) {
    case -10: TCterminal.print("HWI"); break;
    case -9:  TCterminal.print("AK"); break;
    case -8:  TCterminal.print("PST"); break;
    case -7:  TCterminal.print("MST"); break;
    case -6:  TCterminal.print("CST"); break;
    case -5:  TCterminal.print("EST"); break;
    case -4:  TCterminal.print("AST"); break;
    case 0:   TCterminal.print("UTC"); break;
  }
  TERMxy(28,5); // Daylight Savings time flag
  if (DSTFLAG) {
    TCterminal.print("Yes");
  } else {
    TCterminal.print("No");
  }
  //Maximum Encoder Counts in Azimuth, Altitude, and Azimuth offset from True North
  TERMxy(0,21); TCterminal.print(eecharbuf.strunion.RRAAZ);
  TERMxy(11,21); TCterminal.print(eecharbuf.strunion.RDECAL);
  TERMxy(22,21); TCterminal.print(TMPAZOFF);
  // Latitude, Longitude
  double ftmp;
  ftmp = FLATITUDE;
  TERMxy(33,5); FDISPLAYDMS(fabs(ftmp));
  if (ftmp > 0 ) {
    TCterminal.print(" N");
  } else {
    TCterminal.print(" S");
  }
  
  ftmp = FLONGITUDE;
  TERMxy(47,5); FDISPLAYDMS(fabs(ftmp));
  if (ftmp > 0 ) {
    TCterminal.print(" E");
  } else {
    TCterminal.print(" W");
  }
  if (GPSpresent) { // Elevation from GPS
    TERMxy(63,5); TCterminal.print(FALTEMP);
  }
}

void SHWDT() {
  // Print date...
  print2digitsUSB(UTDAY());
  TCterminal.print("/");
  print2digitsUSB(UTMONTH());
  TCterminal.print("/");
  print2digitsUSB(UTYEAR() % 100);
}

void updatestatusscreen() {
  // Update data on status screen
  TCterminal.print("  ");
  TERMxy(0,5);
  SHWDT();
  // Print Time
  TERMxy(13,5); DISTIME(&UT);
  //if (GPSpresent) { // Elevation from GPS
    //TERMxy(63,5); TCterminal.print(FALTEMP);
  //}
  // LT, LST
  TERMxy(13,6); DISTIME(&LT);
  TERMxy(13,6);
  if (BMEpresent) {
    TERMxy(0,8);  // Temperature
    //FTEMPF = bme280.readTempF();
    TCterminal.print(FTEMPF);
    TCterminal.print(" F ");
    TERMxy(14,8);
    //FHUMID = bme280.readFloatHumidity();
    TCterminal.print(FHUMID);
    TCterminal.print("% ");
    //FPINHG = (bme280.readFloatPressure() * 0.0002953);
    TERMxy(24,8);
    TCterminal.print(FPINHG);
    TCterminal.print(" InHg ");
    //FHEIGHT = bme280.readFloatAltitudeFeet();
    TERMxy(36,8);
    TCterminal.print(FHEIGHT);
    TCterminal.print(" ft  ");
  }
  if (MagCompasspresent) {
    TERMxy(48,8);
    TCterminal.print(FMAGHDG, 1); TCterminal.print("  ");
  }
  if (inclinometer.isConnected()) {
    //Get next block of data from sensor
    if (inclinometer.available()) {
      TERMxy(32,21);
      TCterminal.print(inclinometer.getTiltLevelOffsetAngleX());
      TCterminal.print("  ");
      TERMxy(39,21);
      TCterminal.print(inclinometer.getTiltLevelOffsetAngleY());
      TCterminal.print("  ");
    }
  }
  if (eecharbuf.strunion.INA219flag) {
    TERMxy(58,21);
    TCterminal.print(busvolts, 2); // Voltage monitor
    TCterminal.print(" ");
  }
  newdelay(20);
  /*
  BEGIN
  GETCURRENTAZAL ( - Daz/Dal )
  ANTIREFRACT ( Daz/Dal - Daz/Dal )
  AZ/AL->RA/DEC ( - Dra/Ddec/Daz/Dal )
  REFRACTION ( adds alflag )
  DISPLAYSTKCOORDS ( - Dra/Ddec/Daz/Dal/alflag )
  3 NFDROP ( Efficiently clean 9 stack values )
  TRUEFLAG UPDATEDCOUNTER ( Pflag - flag )
UNTIL ;
/*
 * Display current position
 */
// GETCURRENTAZAL();
}

// TAB->RA/DEC )
// BTAB->RA/DEC SAVNSNUM MKTABADR GETMESS GETCALD )
// CHKHRSHL GETHRSHL GETNGC GETIC TABLADR )
// GETMESSNGC SAVEUCOORD? )
// GETARB STARCOORDS PRNTSTAR PICKSTAR GETSTAR )
// DISPLAYSTKCOORDS UPDATEDISPLAY HANDBOX )
// RA/DEC->TARG STAYONAZAL STAYHERE NOTRACK TRACK )
// STARLAB SHWOBJ MSRLAB HRSHLAB LABELOBJ DISPOFF )

int GETSOURCE() {
TCterminal.println("\nEnter one of the bracketed characters:");
TCterminal.println("<I>nput  <H>unt  <D>efault  <S>kip");
int n;
boolean flag;
do {
  n = KEY();
  flag = HUNTquestion(n) || SKIPquestion(n) || DEFAULTquestion(n) || INPUTquestion(n);
} while (!flag);
EMIT( n ); //echo good input
return n;
}

void SRCMSG() {
TCterminal.print("\nSource for ");
}

void KEYMSG() {
TCterminal.println("\nUse <cr> key to quit.");
}

void TIMEMSG() {
TCterminal.println("\nInput time as: hr.mn.sc");
}

void PSAVEMSG() {
TCterminal.print("Use 'P' to save position.");
}

void JCOORDMSG() {
TCterminal.println("\nThe coordinates are displayed in the following formats:");
TCterminal.println("\nRA=hr.mn.sc   Right Ascension");
TCterminal.println("DE=-dg.mn.sc  Declination");
TCterminal.println("AZ=deg.mn.sc  Azimuth");
TCterminal.println("AL=-dg.mn.sc  Altitude\n");
}

void TERMCOORDMSG() {
  JCOORDMSG();
  PSAVEMSG();
}

void DEGMSG() {
TCterminal.print("in: -deg.mn.sc format.");
}

void LONGMSG() {
const char* tmp = "LONGITUDE=";
TCterminal.println("");
TCterminal.print(tmp);
LCDline1();
LCDprint(tmp);
}

void COUNTMSG() {
TCterminal.println("\nMaximum number of encoder counts in ");
}

void TZMSG() {
TCterminal.println("\nTime Zone offset in hours from UT is:");
}

void REDOMSG() {
TCterminal.println("Verify resetting the ");
}

void BUTTONMSG() {
TCterminal.println("\nPress the Lock button to lock onto the coordinates the telescope is");
TCterminal.println("pointing to.  Hold it down for at least one second!");
}

void UCOORDMSG() {
TCterminal.println("\nEnter a User Coordinate number between 1 and ");
TCterminal.print(MAXUCOORD);
TCterminal.print(":");
}

void TERMALLOPTIONS() {
//TCterminal.print("What is the character of the option you want? ");
  TERMxy(0,22);
  TERMtextcolor('r');
  TCterminal.println("Command:  0-8 Sun, Moon, Planet   . Star   - Non-Stellar (Scan)");
  TCterminal.print("          9 Arbitrary   <Enter> Goto   <bckspc> Initialize");
  TERMtextcolor('w');
//LCDline1();
//LCDprint("OPTION? ");
}

boolean TRACKINGCOMMANDquestion( int n ) {
  boolean flag;
if (SETOHNDBOX == n) {
  return true; // using Handbox now
} else {
  if (TIMECOMMANDquestion()) {
    REDOMSG();
    TCterminal.print("Time? (Y/N) ");
  }
  if (INITCOMMANDquestion()) {
    REDOMSG();
    TCterminal.print("Initialization Parameters? (Y/N) ");
  }
  if (TIMECOMMANDquestion() || INITCOMMANDquestion()) {
    TCterminal.print("SURE?");
    flag = GETYORN();
    if (flag) {
      if (TIMECOMMANDquestion()) {
        RESETIMEFLAG();
      } else {
        RESETINITFLAG();
      }
    }
    return flag;
  } else {
    // And now, let's check for the new scan function )
    if ((n > SHWARB) || (n == FNDPLTO)) {
    // We now make this the SCAN function permanently
      // SCANFUNC 
      return true;
    } else
      return false;
  }
}
}

void PRPLANET( int n ) {
  // print planet name
  switch (n) {
case 00: TCterminal.println("THE SUN"); LCDprint("THE SUN"); break;
case 01: TCterminal.println("MERCURY"); LCDprint("MERCURY"); break;
case 02: TCterminal.println("VENUS");   LCDprint("VENUS"); break;
case 03: TCterminal.println("THE MOON"); LCDprint("THE MOON"); break;
case 04: TCterminal.println("MARS");    LCDprint("MARS"); break;
case 05: TCterminal.println("JUPITER"); LCDprint("JUPITER"); break;
case 06: TCterminal.println("SATURN");  LCDprint("SATURN"); break;
case 07: TCterminal.println("URANUS");  LCDprint("URANUS"); break;
case 0x08: TCterminal.println("NEPTUNE"); LCDprint("NEPTUNE"); break;
  }
}

void PRINTITEM( int n ) {
  TCterminal.println("\nComputing position of: ");
  PRPLANET(n);
}
/*
DECIMAL
: TAB->RA/DEC ( addr - Dra/Ddec )
DUP 2+ SWAP @ ( get ra ) ( - addr+2/ra )
SWAP @ ( get dec )
T2->T3 ;

: BTAB->RA/DEC ( addr - Dra/Ddec )
DUP 2+ SWAP T@ ( get ra ) ( - addr+2/ra )
SWAP T@ ( get dec )
T2->T3 ;

: SAVNSNUM ( n - n )
DUP SCANOBJ# ! ( save for scan function )
DUP TMPOBJNUM ! ; ( save for display )

: MKTABADR ( ngcic_num - ngcicaddr )
( object #s are NGC, IC, or Other objects only )
( account for IC numbers )
DUP NGCNUM >
IF ( either IC or Other objects )
  10000 -
  DUP ICNUM >
  IF ( Other objects )
    10000 + ( Add back in offset to stay compatable )
    ( Compute starting address to look at )
    NGCNUM ICNUM + 4 * NGCTAB + ( n - n/addr )
    BEGIN ( look for object in table )
      2DUP ( Efficient way to do OVER OVER )
      T@ = ( - n/addr/flag )
      OVER T@ -1 = OR ( -1=$FFFF at end of table )
      SWAP 6 + SWAP ( 6 bytes are used for each object in Other table )
    UNTIL
    DUP 6 - T@ -1 = ( -1=end of table )
    IF
      2DROP 0 ( end of table + not found )
    ELSE
      SWAP DROP ( - address )
      4 - ( back up to ra/dec )
    THEN
  ELSE ( Compute address of IC object )
    NGCNUM +
    1- 4 * NGCTAB + ( - icadddr )
  THEN
ELSE ( Just a regular NGC object )
  1- 4 * NGCTAB + ( - ngcaddr )
THEN ;

: GETMESS ( - ngcicaddr )
CR ." MESSIER=" GETINUM
DUP DUP 111 < SWAP ( - n/flag/n )
0 > AND ( - n/flag )
IF ( Messier # )
  3 TMPOBJTYPE C! ( save for display )
  SAVNSNUM
  1- 2* ( offset, since only 2 bytes are used per object )
  MSRTAB ( table address )
  + ( Add offset to table base )
  DUP IDOBJADDR ! ( store addr for target ID later )
  @ ( get NGC number from table )
  MKTABADR ( object_num - table_addr )
ELSE
  DROP 0 SAVNSNUM
THEN ;

: GETCALD ( - ngcicaddr )
( Caldwell # )
CR ." CALD=" GETINUM
DUP DUP 110 < SWAP
0 > AND ( - n/flag )
IF ( Caldwell # )
  3 TMPOBJTYPE C! ( save for display )
  SAVNSNUM
  1- 2* ( offset, since only 2 bytes are used per object )
  220 + ( additional offset to Caldwell #'s )
  MSRTAB ( table address )
  + ( Add offset to table base )
  DUP IDOBJADDR ! ( store addr for target ID later )
  @ ( get NGC number from table )
  MKTABADR ( object_num - table_addr )
ELSE
  DROP 0 SAVNSNUM
THEN ;

: CHKHRSHL ( hershel# - ngc# )
( 110 Messier + 109 Caldwell object * 2 bytes = 438 Bytes )
HERSHELOFF ( offset to Hershel #'s )
MSRTAB ( table address )
+ ( Add offset to table base )
DUP 1600 + SWAP ( - hershel/tablimit/tabstart )
DO
  I @ OVER =
  IF
    I 2+
    DUP IDOBJADDR ! ( store addr for target ID later )
    @ ( got NGC # )
    SWAP DROP LEAVE
  THEN
4 ( skip 4 bytes to next Hershel # )
+LOOP ;

: GETHRSHL ( - ngcaddr )
( Hershel # )
CR ." HRSHL=" GETFDECNUM ( GETINUM )
( Modified this to search for wierd Hershel # )
FDUP FINT FDUP F->S >R ( save base Hershel # to return stack )
F- 10 S->F F* F->S ( now we have the extension # )
( now shift 11 bits pattern as stored in memory )
2048 * R> OR ( and combine with base Hershel # )
( Search for this turkey in the Hershel table )
SAVNSNUM ( n - n )
4 TMPOBJTYPE C! ( save for display )
CHKHRSHL ( hershel# - ngc# )
DUP SCANOBJ# @ =
IF ( Hershel # didn't change, so not found )
  DROP 0 SAVNSNUM
ELSE ( NGC # will never = Hershel # format )
  MKTABADR ( object_num - table_addr )
THEN ;

: GETNGC ( - ngcaddr )
CR ." NGC=" GETINUM
DUP DUP NGCNUM 1+ < SWAP
0 > AND
IF
  SAVNSNUM
  1 TMPOBJTYPE C! ( save for display )
  MKTABADR ( object_num - table_addr )
  DUP IDOBJADDR ! ( store addr for target ID later )
ELSE
  DROP 0 SAVNSNUM
THEN ;

: GETIC ( - icaddr )
CR ." IC=" GETINUM
DUP DUP ICNUM 1+ < SWAP
0 > AND OVER
19999 > OR
IF
  DUP 10000 < IF 10000 + THEN
  SAVNSNUM
  2 TMPOBJTYPE C! ( save for display )
  MKTABADR ( object_num - table_addr )
  DUP IDOBJADDR ! ( store addr for target ID later )
ELSE
  DROP 0 SAVNSNUM
THEN ;

: TABLADR ( n - n/ngcicaddr )
BESTFLAG ! ( record if we will use the Best table )
( TERMIF ( )
  BCR ABCR
  AB." Select Object Table:"
  ABCR AB." 2 - Messier"
  ABCR AB." 3 - Caldwell"
  ABCR AB." 4 - Hershel"
  ABCR AB." 5 - NGC"
  ABCR AB." 6 - IC and Other"
  BESTFLAG @ IF
    BCR AB." 7 - Best Objects"
  THEN
  2CR
( THEN ( )
0. ( double zeros on stack )
BEGIN
  SWAP DROP TERMLINEUP ( - n )
  CR ." TABLE?"
  DUP 2 = IF CR TERMLINEUP ." MESSIER..." THEN
  DUP 3 = IF CR TERMLINEUP ." CALD...." THEN
  DUP 4 = IF CR TERMLINEUP ." HERSHEL..." THEN
  DUP 5 = IF CR TERMLINEUP ." NGC....." THEN
  DUP 6 = IF CR TERMLINEUP ." IC......" THEN
  BESTFLAG @ IF
  DUP 7 = IF CR TERMLINEUP ." BEST...." THEN ( only for scan )
  THEN
  DUP DUP ( - n/n/n )
  BESTFLAG @ IF 8 ELSE 7 THEN ( - n/n/n/7 or 8 )
  < SWAP ( - n/flag/n )
  1 > AND ( - n/flag )
  KEY 48 - SWAP ( - n/new_n/flag )
  IF ( valid, so get CR )
    ( - n/new_n )
    ( usually Enter is 13 decimal, but have to mod here )
    DUP -35 = ( enter? ) ( - n/new_n/flag )
  ELSE
    FALSEFLAG ( otherwise, repeat loop )
  THEN
UNTIL
DROP ( - n )
DUP SCANTAB C!
DUP 2 = IF GETMESS >R THEN
DUP 3 = IF GETCALD >R THEN
DUP 4 = IF GETHRSHL >R THEN
DUP 5 = IF GETNGC >R THEN
DUP 6 = IF GETIC >R THEN
DUP 7 = IF
  1 SCANOBJ# ! ( start always with first best object )
  BESTAB >R
THEN
R> ; ( - n/ngcicaddr )

: GETMESSNGC ( - Dra/Ddec )
( stay tracking current target if NGC number not in catalog )
FALSEFLAG TABLADR ( n - n/ngcicaddr ) ( don't use Best table )
SWAP DROP ( - ngcicaddr )
-DUP IF ( valid )
  BTAB->RA/DEC
ELSE ( not valid )
  TRGOBJTYPE C@ TMPOBJTYPE C!
  TRGOBJNUM  @ TMPOBJNUM !
  TRGOBJADDR @ IDOBJADDR !
  TRA L@ TDEC L@
THEN
PRECESSIONTAB ;

: SAVEUCOORD? ( Dra/Ddec - )
( TERMIF ( )
  BCR ABCR
  AB." Save to a User Defined Object Variable."
  UCOORDMSG
( THEN ( )
CR ." SAVE=" GETINUM
DUP ( - n/n )
DUP 0> SWAP MAXUCOORD 1+ < AND
IF
  !UCOORD ( Dra/Ddec/ucoord# - )
ELSE
  2DROP FDROP
THEN ;

: GETARB ( - Dra/Ddec )
( TERMIF ( )
  BCR ABCR
  AB." Enter Arbitary coordinate position."
  ABCR AB." Enter these as: hr.mn.sc and -deg.mn.sc"
  ABCR AB." Or at the RA= prompt enter P<num> for a user coordinate."
  ABCR AB." If entering a User Defined Coordinate,"
  UCOORDMSG
( THEN ( )
CR ." RA=" ( INPUTHMS ) ( - d )
( Okay, new part to get RA or UCOORD. )
PAD 12 ACCEPT ( addr/n - )
PAD DUP 1+ C@ 80 = ( P char? )
IF
  1+ NUMBER ( - d )
  DROP ( d - n )
  DUP MAXUCOORD 1+ <
  OVER 0 <> AND
  IF ( only accept between 1 to 22/8 inclusive )
    TERMIF
      DUP 2CR ." Retrieving coordinates for User Object # " .
    THEN
    @UCOORD ( n - Dra/Ddec )
  ELSE
    ( TERMIF ( )
      BCR ABCR
      AB." ERROR getting User Object #" . ( n - )
      ABCR AB." Showing current coordinates instead."
    ( THEN ( )
    ." OBJECT ERROR" WAITASEC CR
    GETCURRENTAZAL ( - Daz/Dal )
    AZ/AL->RA/DEC 2DROP 2DROP ( - Dra/Ddec )
  THEN
ELSE
  CONVHMS ( - d ) ( and continue on )
  ( Dra = d * 15 ) ( Convert Hour angle to degrees )
  15 UD* ( - Dra )
  CR ." DE=" INPUTHMS ( - d )
  ( - Dra/Ddec )
  PRECESS?
  IF
    ( TERMIF ( )
      BCR ABCR
      AB." Enter epoch year of these coordinates as an integer."
    ( THEN ( )
    CR ." EPOCH=" GETINUM
    -DUP
    IF PRECESSION THEN
  THEN
THEN ;

: STARCOORDS ( n - Dra/Ddec/mag )
1- 5 * ( offset to star in table )
STRTAB ( table address )
+ ( absolute address )
( now save the address for possible later ID )
DUP IDOBJADDR !
DUP >R ( save addr to get mag )
TAB->RA/DEC
PRECESSIONTAB ( Dra/Ddec - Dra/Ddec )
R> 4 + C@ ; ( get mag )

: PRNTSTAR ( n - n )
( output star name if it exists )
STRNAMES ( - n/addr )
BEGIN ( Loop thru star names )
    DUP @ ( - n/addr/star# )
    DUP -1 <>
WHILE
    SWAP 1+ ( - n/star#/addr+1) 
    BEGIN ( print name )
      1+ DUP C@ ( - n/star#/addr+n/char )
      DUP 255 <>
    WHILE
      2OVER = ( - n/star#/addr+n/char/flag )
      IF EMIT ELSE DROP THEN
    REPEAT ( - n/star#/addr+n )
    DROP SWAP 3 PICK ( - n/addr+n/star#/n )
    = IF WAITASEC WAITASEC CR THEN ( - n/addr+n )
    1+ ( point to next star # )
REPEAT
2DROP ( - n )
;

: PICKSTAR ( - n )
( Allow user to input either star # or select star name )
PAD 12 ACCEPT ( addr/n - )
PAD DUP 1+ C@ 80 = ( P char? )
IF
  ( User wants to pick from a star name )
  DROP  ( don't need PAD value )
  CR 0 STRNAMES ( - addr )
  BEGIN ( Loop thru star names )
    SWAP DROP ( delete previous star number )
    DUP @ ( - addr/star# )
    DUP -1 =
    IF 2DROP STRNAMES DUP @ THEN ( go to start of table if needed )
    SWAP 1+ CR
    BEGIN ( print name )
      1+ DUP C@ DUP 255 <>
    WHILE
      EMIT
    REPEAT
    DROP ( - star#/addr )
    1+ ( point to next star # )
    KEY ( wait for decision from user )
    ( Allow for Abort )
    DUP 65 = ( A for abort )
    IF DROP SWAP DROP CR ." STAR=" GETINUM SWAP ENTER THEN
    ENTER =
  UNTIL
  DROP ( - star# )
ELSE
  GET# DROP ( - n ) ( and continue on )
THEN
;

: GETSTAR ( - Dra/Ddec/mag )
  AB." Enter star number from catalog of stars."
  ABCR AB." Or P to pick from list of star names."
0
BEGIN
  DROP
  CR ." STAR="
  ( GETINUM ( get star or planet number )
  PICKSTAR ( - n ) ( new version to get # )
  DUP ( - n/n )
  DUP TMPOBJNUM ! ( save object number to display later )
  0 TMPOBJTYPE C! ( stars are type 0 object )
  NOTINIT? IF -10 ELSE -1 THEN ( allow alignment w/ planets )
  ( - n/n/-10or-1 )
  > ( accept 0 for sun )
  ( - n/flag )
  OVER NSTARS 1+ < AND ( make sure it's in range )
  ( - n/flag )
  OVER -3 = 0= AND ( Don't accept -3 for the moon! )
UNTIL ( - n )
DUP 1 <
IF
  DUP -7 <
  IF
    DROP GETARB ( n - Dra/Ddec )
    6 TRGOBJTYPE C! ( store invalid object type # )
  ELSE
    DUP ABS ( Absolute value of negative # )
    PRINTITEM ( n - n )
    TRGOBJNUM ! ( save planet # for later disp )
    5 TRGOBJTYPE C! ( planets are type 5 )
    GETSMPOSITION ( n - Dra/Ddec )
  THEN
  -1 ( put dummy solar magnitude onto stack )
ELSE
  STARCOORDS ( n - Dra/Ddec/mag )
THEN ;

: DISPLAYSTKCOORDS
( Dra/Ddec/Daz/Dal/alflag - Dra/Ddec/Daz/Dal/alflag )
TERMLINEUP TERMLINEUP TERMLINEUP TERMLINEUP
DISPLAYCOUNTER C@ DUP
3 = TERMDISPLAY? OR
IF ( ra )
  5 2PICK ( - Dra )
  RAAZ->F2PIRAD ( - f in rad )
  F15. F/ ( account for hours display instead of deg )
  TERMCR ." RA=" FDISPLAYDMS 2SP
THEN
DUP 4 = TERMDISPLAY? OR
IF ( dec )
  4 2PICK DECAL->FPI/2RAD
  TERMCR ." DE=" FDISPLAYDMS 2SP
THEN
DUP 2 = TERMDISPLAY? OR
IF ( az )
  3 2PICK RAAZ->F2PIRAD
  TERMCR ." AZ=" FDISPLAYDMS 2SP
THEN
DUP 1 = TERMDISPLAY? OR
IF ( al )
  2OVER ( - Dal )
  DECAL->FPI/2RAD ( Dal - frad )
  TERMCR ." AL=" FDISPLAYDMS 2SP
THEN DROP ;

: UPDATEDISPLAY ( - )
GETCURRENTAZAL ( - Daz/Dal )
ANTIREFRACT
AZ/AL->RA/DEC
REFRACTION ( reinstalled so we can see actual altitude )
DISPLAYSTKCOORDS
3 NFDROP ( Efficient way to clear 9 stack values )
TERM? -DUP
IF
  BACKSPACE =
  IF
    DISPLAYCOUNTER C@ 1+
    DUP 5 =
    IF DROP 1 THEN
    DISPLAYCOUNTER C!
  THEN
THEN ;

: HANDBOX ( - Daz/Dal ) ( absolute positioning )
( TERMIF ( )
  BCR ABCR
  AB." Use the Handbox to move the telescope around the sky.  X and Y"
  ABCR AB." buttons control telescope axes."
  BUTTONMSG
  JCOORDMSG
  2CR 2CR
( THEN ( )
1 DISPLAYCOUNTER C!
BEGIN
  NOTINIT?
  IF
    ( show az/al or ra/dec when initializing )
    TERMLINEUP TERMLINEUP
    GETCURRENTAZAL ( - Daz/Dal )
    FALSEFLAG ( - Daz/Dal/alflag )
    DISPLAYCOUNTER C@ DUP ( - Daz/Dal/alflag/dc/dc )
    1 = TERMDISPLAY? OR
    IF ( az ) ( - Daz/Dal/alflag/dc )
      3 2PICK RAAZ->F2PIRAD ( - Daz/Dal/alflag/dc/frad )
      TERMCR ." AZ=" FDISPLAYDMS 2SP
    THEN
    2 = TERMDISPLAY? OR
    IF ( al ) ( - Daz/Dal/alflag )
      0 2OVER DECAL->FPI/2RAD
      TERMCR ." AL=" FDISPLAYDMS 2SP
      DROP ( get rid of dummy zero on stack )
    THEN
    2DROP FDROP ( Daz/Dal/alflag - )
    TERM? -DUP
    IF
      BACKSPACE =
      IF
        DISPLAYCOUNTER C@ 1+
        DUP 3 = IF DROP 1 THEN
        DISPLAYCOUNTER C!
      THEN
    THEN
  ELSE ( normal display )
    UPDATEDISPLAY
  THEN
  TRUEFLAG ( set Handbox mode )
  RDLBTN ( - flag )
UNTIL
0 RDAZAL MTRSON ( Current position is target )
FALSEFLAG RDLBTN DROP ( Disable Handbox )
GETCURRENTAZAL ;

: RA/DEC->TARG ( Dra/Ddec - )
( object coordinates in 16 bit form )
TDEC L!
TRA L! TRKSETUP ;

: STAYONAZAL ( Daz/Dal - )
ANTIREFRACT ( Daz/Dal - Daz/Dal )
( correct for being too close to horizon )
DUP 10 < IF DROP 10 THEN
AZ/AL->RA/DEC ( 2DROP 2DROP ) ( - Dra/Ddec/Daz/Dal )
GOTOAZAL ( Daz/Dal - )
RA/DEC->TARG ( Dra/Ddec - )
FNDARB COMMAND C! ;

: STAYHERE ( - )
FALSEFLAG RDLBTN DROP ( Disable Handbox )
GETCURRENTAZAL ( - Daz/Dal ) ( absolute positioning )
STAYONAZAL ( Daz/Dal - )
0 RDAZAL MTRSON ; ( Current position is target )

: NOTRACK ( - )
STAYHERE
STAYFUNC COMMAND C! ;

HEX
: TRACK ( - ) ( do routine of COMMAND )
HSIG IF ( special handbox mode to reposition scope )
  BEGIN
    TRUEFLAG ( set Handbox mode )
    RDLBTN ( - flag )
  UNTIL
  STAYHERE ( - ) ( Track this position )
THEN
COMMAND C@ ( - n )
TIMECOMMAND? SWAP ( n - flag/n )
INITCOMMAND? ROT OR ( - n/flag )
IF
  DROP ( do nothing for resetime or reinit command )
ELSE
  DROP
  COMMAND C@ STAYFUNC <>
  IF ( Gotta move! )
    TRACK->AZ/AL ( - Daz/Dal )
    REFRACTION ( adds alflag )
    IF ( - Daz/Dal )
      ( TERMIF ( )
        BCR ABCR
    AB." Tracking error!  The Target position is below the horizon."
        ABCR AB." Tracking stops here." ABCR
      ( THEN ( )
      CR ." NOT UP!"
      NOTRACK ( - ) ( Stop moving )
      WAITASEC
      2DROP 2DROP ( Daz/Dal - )
    ELSE
      GOTOAZAL ( Daz/Dal - )
    THEN
  THEN
THEN ;

DECIMAL
: STARLAB ( star# - )
( STRTAB - 5 / 1+ ( - star# )
  PRNTSTAR ( n - n )
  ." STAR " U.
;

: SHWOBJ ( ngcic# - )
( get ngcic# from addr by: ADDR NGCTAB - 4 / 1+ )
DUP NGCNUM >
IF
  DUP 20000 <
  IF
    10000 -
    ." IC"
  ELSE
    ." O"
  THEN
ELSE
  ." NGC"
THEN
U. ;

: MSRLAB ( msr#addr - )
DUP MSRTAB - 2/ 1+ ( - msr#adr/m# )
DUP 111 <
IF
  ." M" U.
ELSE
  110 - ." C" U.
THEN
@ ( get ngc/ic # )
SHWOBJ ( ngcic# - ) ;

: HRSHLAB ( hershel#addr+2 - )
DUP TERMIF ." H" THEN 2- @ ( - hershel#addr+2/hershel# )
DUP 2048 / ( Don't need 11 least sig bits for cat # )
SWAP 2047 ( $07FF ) AND 3 .R ." ." .
@ ( get ngc/ic # )
." N" TERMIF ." GC" THEN 4 .R ( ngcic# - ) ;

: LABELOBJ ( ngcicother# -  )
( now print it out )
IDOBJTYPE @ DUP ( Retrieve object type number )
0= IF ( star )
  IDOBJADDR @ ( - star_addr )
  STRTAB - 5 / 1+ ( - star# )
  STARLAB
THEN
DUP 3 = IF ( MSR/CLD )
  IDOBJADDR @ ( - msr#addr )
  MSRLAB ( msr#addr - )
THEN
DUP 1 = OVER 2 = OR IF ( NGC/IC/Other )
  OVER ( - ngcicother#/objtype/ngcic# )
  SHWOBJ ( ngcicother# - )
THEN
DUP 4 = IF ( Hershel )
  IDOBJADDR @ ( - hershel#addr+2 )
  HRSHLAB ( hershel#addr+2 - )
THEN
5 = ( Planet then... )
IF
  TRGOBJNUM @ ( - planet# )
  PRPLANET ( n - n )
  2SP
  DROP ( don't need planet # now )
THEN
DROP ( don't need ngcicother# anymore )
2SP TERMCR ;

: DISPOFF ( - )
( print offsets of encoders from target position in case of no motors )
( Updated for large counts )
( TERMIF ( )
  BCR ABCR
  AB." Here are the current telescope position offsets from"
  ABCR AB." the target position.  Hit Return to exit."
  ABCR ABCR
  EQ? IF
    B." R. A.   Declination"
  ELSE
    B." Azimuth Altitude"
  THEN
  2CR
( THEN ( )
BEGIN
  TRACK ( - )
  RDAZAL ( - Draz/Dral )
  TAL L@ 2SWAP D- ( - Draz/Draldiff )
  ( adjust difference # displayed if it could be large )
  MDECAL L@
  IF >R D/256 R> THEN DROP
  ( add correction to put offset in range +-8192 counts )
  8192. MDECAL L@ D/256 D< IF D2/ D2/ D2/ THEN
  2SWAP
  TAZ L@ 2SWAP D- ( - Daldif/Dazdif )
  ( adjust az offset for closer direction )
  2DUP MRAAZ L@ 2 DU/ D< ( offset < 1/2 range? )
  ( - Daldif/Dazdif/flag )
  IF
    2DUP MRAAZ L@ 2 DU/ DMINUS D< ( offset < -1/2 range? )
    IF MRAAZ L@ D+ THEN
  ELSE
    MRAAZ L@ D-
  THEN
  ( - Daldif/Dazdif )
  ( adjust difference # displayed if it could be large )
  MRAAZ L@ IF >R D/256 R> THEN DROP
  ( add correction to put offset in range +-8192 counts )
  8192. MRAAZ L@ D/256 D< IF D2/ D2/ D2/ THEN
  ( print out offsets )
  TERMLINEUP ( - )
  6 D.R 6 D.R 2SP
  TERMCR
  CHKNUM ( - false or - n/true )
  ( new part to print out target with backspace )
  IF ( print it out? )
    15 = ( 0X0F is backspace on keypad )
    IF ( display target )
      TRGOBJADDR @ IDOBJADDR !
      TERMLINEUP
      TRGOBJTYPE C@ DUP IDOBJTYPE !
      6 = ( Other? )
      IF
        ( print out target RA/DEC )
        ( ra )
        TRA L@ ( - Dra )
        RAAZ->F2PIRAD ( - f in rad )
  F15. F/ ( account for hours display instead of deg )
        ." RA=" FDISPLAYDMS 2SP TERMCR
        KEY DUP BACKSPACE =
        IF ( another backspace? )
          ( dec )
          TERMLINEUP
          TDEC L@ DECAL->FPI/2RAD
          ." DE=" FDISPLAYDMS 2SP TERMCR
          DROP
          KEY ( Wait to switch back )
        THEN
      ELSE
        TRGOBJNUM @
        LABELOBJ ( ngcicother# -  )
        ( TERMCR is already done )
        ( KEY ( Wait to switch back )
        WAITASEC WAITASEC WAITASEC BACKSPACE
        CHKNUM ( - false or - n/true )
        IF DROP THEN ( This lets a backspace restore display )
      THEN
      BACKSPACE = IF FALSEFLAG ELSE TRUEFLAG THEN
    ELSE
      TRUEFLAG
    THEN
  ELSE
    FALSEFLAG
  THEN
UNTIL ;

 */
