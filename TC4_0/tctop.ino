// TCTOP
// Telescope Controller 4.0 - High level functionality
#include "tcheader.h"

// Main Telescope Controller Routines
void tcintro() { // Startup screen for user
  // Set up I2C and serial ports
  Wire.begin(); // I2C init
  SerialUSB.begin(9600); //May use VT100 compatible terminal emulator - Full Menus
  Serial1.begin(9600); // use serial 4x20 LCD display - Easy to mount onto telescope
  Serial2.begin(9600); //Or put VT100 compatible terminal emulator - Full Menus here

  pinPeripheral(2, PIO_SERCOM); // Define Pins for Serial2 UART port
  pinPeripheral(3, PIO_SERCOM_ALT);
  newdelay(1600); // Give time for SerialUSB to init
  TERMclear(); //Clear terminal screen and home cursor
  TERMtextcolor('g');
  TCterminal.println("\nArduino SAMD Telescope Controller");
  TCterminal.println("(c) 2020, MIT License");
  TERMtextcolor('r');
  TCterminal.println("\nThis software provided as-is without warranty of any kind.");
  TCterminal.println("The User assumes all responsibility for use of this");
  TCterminal.println("program and all related documentation files.");
  TERMtextcolor('w');
  TCterminal.print("\nInitializing system for ");
  if (EQquestion())
    TCterminal.print("Equatorial");
  else
    TCterminal.print("Alt-Azimuth");
  TCterminal.println(" mounted telescopes.");
  TCterminal.print("\nVersion ");
  TCterminal.println(tcversion);
  TCterminal.println("");


  // This was the first half of the topmost Forth word TC - used for initialization
  TERMtextcolor('b');
  inithardware(); //See what we have out there connected, and set it up

  /* Site ID string copy to sized buffer (overflow safe): */
  strncpy ( siteid, eecharbuf.strunion.SiteID, sizeof(eecharbuf.strunion.SiteID) );

  //Handle Lock btn status on Reset
  if (lockbtninitflag) { //Lock button pressed on reset?
    TCterminal.println("\nLock button pressed on reset.  Forcing reinitialization.");
    LCDline2();
    LCDprint("TC SYSTEM RESET");
    RESETIMEFLAG(); RESETINITFLAG(); // Actual re-initialization done in TC_main()
    //CstoreCLK (0, BFLAGS); //reset all init flags
    eecharbuf.strunion.BFLAGS = 0; //reset all init flags
    WAITASEC(3); // Let user know what we've done
  }
  // make it so it doesn't assume errors on start
  if (NOTIMESETquestion() && NOTINITquestion()) { RESETERRFLAG(); SMPDONE = 0; }
  
  WAITASEC(2);
  //BINIT(); // Default initialize on Boot
  LCDline3();
  LCDprint(hitkey);
  TERMtextcolor('w');
  TCterminal.println(hitkey);
  KEY();
  TCterminal.println("    ");
  printstatusscreen();
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
    //SMPINIT();
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
  //if (callscreenredraw) printstatusscreen();
  COMMAND = STAYFUNC; // Dummy command value to start
  MAIN( COMMAND ); // Main loop to process user commands
  //STAYHERE( ); // Stay at current position
  WAITASEC(1);
}

void topEEPROMwrite() {
  if ( tcEEPROM.write(2L, eecharbuf.ebuf, sizeof(EEstruct)) != 0 ) {
      //there was a problem
      TCterminal.println("EEPROM write error.");
      TCterminal.println(hitkey);
      KEY();
    }
    byte tmpbyte = 0x55;
    tcEEPROM.write(0L, &tmpbyte, 1);
    printstatusscreen();
}

// UPDATEDCOUNTER )
// WAITFORNEXT DISPOS SHWDT DISPTIME )
// IDENTIFY DISCURPOS )
// LISTFLAGS LISTPARAMS SHOWIT CHECKMSR SCANKEY )
// SHWNGC? CHECKOBJ SCAN OBJDISPSET SETUPCOMMAND MAIN )
/*
: UPDATEDCOUNTER ( Pflag - flag )
( n/Dra/Ddec/Daz/Dal/alflag/flag - when coming in )
( Pflag is for signalling if there can be a valid Position command )
( to save the shown position to a User defined coordinate memory. )
TRACK ( - ) ( comment out for S12X )
TERM? -DUP ( -n/true OR - false )
IF ( - Pflag/n )
  DUP BACKSPACE = ( - Pflag/n/flag )
  OVER ENTER = OR ( - Pflag/n/flag )
  IF ( - Pflag/n )
    BACKSPACE =
    IF ( - Pflag )
      DISPLAYCOUNTER C@ 1+ ( - Pflag/n )
      DUP 5 = ( - Pflag/n/flag )
      IF DROP 1 THEN
      DISPLAYCOUNTER C!
      FALSEFLAG
    ELSE
      TRUEFLAG
    THEN ( - Pflag/flag )
  ELSE ( - Pflag/n )
    50 = OVER AND ( P char? and Pflag? )
    IF ( Pflag - Pflag ) ( any other char is ignored )
      5 2PICK ( get Dra )
      5 2PICK ( get Ddec )
      SAVEUCOORD? ( Dra/Ddec - )
      TRUEFLAG ( get out of calling routine after this )
    ELSE
      FALSEFLAG
    THEN
  THEN
ELSE
  FALSEFLAG
THEN
( now get rid of old flag, replace with new )
SWAP DROP ;

: WAITFORNEXT ( flag - flag/flag )
DUP ( - flag/flag )
IF
  0
  BEGIN ( - flag/0 )
    DROP
    ( For 6809 )
    BEGIN ( wait for a key )
      TRACK TERM? -DUP
    UNTIL ( - n )
    ( For S12X )
    ( wait for a key )
    ( KEY ( - n )
    DUP BACKSPACE = ( - n/flag )
    OVER ENTER = OR ( - n/flag )
  UNTIL ( - flag/n )
  BACKSPACE =
  IF
    DUP ( - flag/flag )
  ELSE ( cr=toggle flag to exit loop in calling word )
    0= ( true to false )
    FALSEFLAG ( - flag1/flag2 )
  THEN
ELSE
  DUP ( - flag/flag )
THEN ;

: DISPOS ( n/Dra/Ddec - n )
1 DISPLAYCOUNTER C!
TERMCOORDMSG
KEYMSG 2CR CR
0. 0. 0
BEGIN
  2DROP FDROP ( throw away 5 stack values )
  RA/DEC->AZ/AL ( - n/Dra/Ddec/Daz/Dal )
  REFRACTION ( adds alflag )
  DISPLAYSTKCOORDS ( - n/Dra/Ddec/Daz/Dal/alflag )
  TRUEFLAG UPDATEDCOUNTER ( Pflag - flag )
UNTIL
3 NFDROP ; ( Efficiently clean stack )

DECIMAL
: SHWDT ( - )
( Show date )
UTYEAR @ UTMONTH C@ UTDAY C@ ( - year/month/day )
0 <# # # 46 HOLD ( day )
2DROP 0 # # 46 HOLD ( month )
2DROP 0 # # # # #> TYPE ( year + print ) ;

: DISPTIME ( - )
1 DISPLAYCOUNTER C!
( TERMIF ( )
  ( 2 TMSG )
  BCR
  ABCR AB." Date: year.mo.dy"
  ABCR
  ABCR AB." Time prompts: hr.mn.sc"
  ABCR AB." LT=   Local Time"
  ABCR AB." UTC=  Universal Time Coordinated"
  ABCR AB." LST=  Local Siderial Time"
  KEYMSG
( THEN ( )
2CR 2CR
BEGIN
  TERMLINEUP TERMLINEUP TERMLINEUP TERMLINEUP
  CR ." DATE=" SHWDT ( year + print )
  CR ." LT=" LT DISTIME
  CR ." UTC=" UT DISTIME
  CR ." LST=" LST DISTIME
  WAITASEC
  FALSEFLAG UPDATEDCOUNTER ( Pflag - flag )
UNTIL ;

: IDENTIFY ( - )
( find object in tables closest to current position )
( TERMIF ( )
  BCR ABCR
  AB." Object listing from the internal tables that is closest"
  ABCR AB." to the current telescope position:"
  ABCR AB." (Hit <CR> to exit.)" ABCR
( THEN ( )
CR ( This is needed to clear the LED display, as well as )
   ( to move down another line on the terminal display )
BEGIN
  TERMLINEUP ( - )
  ( To set up to call IDENTOBJ, the stack must be as follows: )
  ( staradr/#stars/ngcadr/#ngcs/cra/cdec - type#/objaddr )
  STRTAB NSTARS ( init star object pointer )
  NGCTAB NGCNUM ICNUM +
  GETCURRENTAZAL ( - Daz/Dal )
  ANTIREFRACT ( Dral - Dal )
  AZ/AL->RA/DEC ( Daz/Dal - Dra/Ddec/Daz/Dal )
  2DROP 2DROP ( - Dra/Ddec )
  PRECESS?
  IF
    UTYEAR @ PRECESSTO2000
  THEN
  T3->T2 ( Dra/Ddec - ra/dec )
( now do something with it )
  TRACK ( comment out for S12X )
  IDENTOBJ ( - type#/objaddr )
  ( ---- )
  DUP IDOBJADDR ! ( save object addr )
  SWAP IDOBJTYPE ! ( save object type )
  ( - objaddr )
  ( convert objaddr to ngc/ic/other # )
  ( for ngc/ic, there are 4 bytes per entry )
  0 NGCTAB 0 D-
  ( 4 / ) D2/ D2/
  DROP 1+ ( - ngc#+ )
  DUP NGCNUM >
  IF ( IC or Other object )
    NGCNUM - ( Make an IC # )
    DUP ICNUM >
    IF ( Other object )
      DROP IDOBJADDR @ 2- T@ ( Get Object # )
    ELSE
      10000 + ( Catalog IC format )
    THEN
  THEN ( - ngcicother# )
( after stars is Messier and Caldwell index tables )
( 219 = 110 Messier + 109 Caldwell )
  IDOBJTYPE @ 0 <> ( Don't search Msr/Caldwell/Hershel if star )
  IF
    DUP MSRTAB ( - ngcicother#/ngcicother#/MsrCldHrslTabAddr )
    CMPOBJ ( obj#/MsrCldHrslTabaddr - type#/objaddr )
    ( object type is zero if it wasn't found )
    OVER IF IDOBJADDR ! IDOBJTYPE ! ELSE 2DROP THEN
  THEN
  LABELOBJ ( ngcicother# - )
  TRACK ( - ) ( update telescope pointing ) ( comment out for S12X )
  CHKNUM ( - falseflag or n/trueflag )
UNTIL DROP ;

: DISCURPOS ( - )
( Here command - Display Current Position )
( This command is actually a combination of capabilities )
( 1 - Start by displaying offsets from current to target position )
DISPOFF ( - )
( 2 - Option to identify object we are looking at )
( TERMIF ( )
  BCR ABCR
  AB." Match telescope position to an object in the internal tables?"
  ABCR AB." (Warning!  Time consuming!)"
( THEN ( )
CR ." IDENTIFY?" GETYORN
IF  ( Only do it if really wanted, 'cause it's so long! )
  IDENTIFY ( - )
THEN
( 3 - Show coordinates of current telescope position )
1 DISPLAYCOUNTER C!
TERMCOORDMSG
KEYMSG 2CR CR
BEGIN
  GETCURRENTAZAL ( - Daz/Dal )
  ANTIREFRACT ( Daz/Dal - Daz/Dal )
  AZ/AL->RA/DEC ( - Dra/Ddec/Daz/Dal )
  REFRACTION ( adds alflag )
  DISPLAYSTKCOORDS ( - Dra/Ddec/Daz/Dal/alflag )
  3 NFDROP ( Efficiently clean 9 stack values )
  TRUEFLAG UPDATEDCOUNTER ( Pflag - flag )
UNTIL ;

: LISTFLAGS ( flag - flag/flag )
TERMIF DUP ELSE WAITFORNEXT THEN ;

: LISTPARAMS ( - )
( TERMIF ( )
  BCR ABCR
  AB." First page of initialization parameters:"
  ABCR
( THEN ( )
BEGIN
  TRUEFLAG
  TERMIF
    BCR AB." Longitude and Latitude are " DEGMSG
    LONGMSG FLONGITUDE F@ FDISPLAYDMS
    6 SPACES ." LA=" FLATITUDE F@ FDISPLAYDMS
  ELSE
    CR ." LA=" FLATITUDE F@ FDISPLAYDMS
    LISTFLAGS ( flag - flag/flag )
    IF
      LONGMSG FLONGITUDE F@ FDISPLAYDMS
    THEN
  THEN
  LISTFLAGS
  IF
    TERMIF
      COUNTMSG
      EQ? IF
        ." Right Ascension and Declination:"
      ELSE
        ." Azimuth and Altitude:"
      THEN
      CR ." MAX "
    ELSE
      CR 77 EMIT ( M )
    THEN
    EQ? IF ." RA=" ELSE ." AZ=" THEN
    MRAAZ L@ D.
  THEN
  LISTFLAGS
  IF
    TERMIF 8 SPACES ." MAX " ELSE CR 77 EMIT THEN
    EQ? IF ." DE=" ELSE ." AL=" THEN
    MDECAL L@ D.
  THEN
  LISTFLAGS
  IF
    ( TERMIF ( )
      BCR ABCR
      AB." Azimuth Offset from North is " DEGMSG
    ( THEN ( )
    CR ." AO=" AZOFF L@ RAAZ->F2PIRAD FDISPLAYDMS
  THEN
  EQ? IF
    LISTFLAGS
    IF
      TERMIF
        BCR ABCR
        AB." Equatorial Mount Latitude:    "
      ELSE
        CR
      THEN
      ." EQL=" FELAT F@ FDISPLAYDMS
    THEN
  THEN
  ( TERMIF ( )
    BCR ABCR
    AB." To see the next parameters page, hit <backspace>."
    KEYMSG
  ( THEN ( )
  WAITFORNEXT ( always wait here )
  DUP TERMDISPLAY? AND IF
    BCR ABCR
    AB." The Second initialization parameters page:"
  THEN
  REFRACT? IF
    IF
      ( TERMIF ( )
        BCR ABCR
        AB." Refraction is accounted for.  Temperature and "
        AB." Barometric Pressure are:"
     ( THEN ( )
      CR ." TEMPF=" FTEMPF F@ F.
    THEN
    LISTFLAGS
    IF
      TERMIF 8 SPACES ELSE CR THEN
      ." PINHG=" FPINHG F@ F.
    THEN
    LISTFLAGS
    IF
      ( TERMIF ( )
        BCR ABCR
AB." If Elevation was used to compute Barometric Pressure, it was:"
      ( THEN ( )
      CR ." EL=" FHEIGHT F@ F.
    THEN
    LISTFLAGS
  THEN
  PRECESS? IF
    IF
      TERMIF
        2CR
." Coordinates are updated to account for Precession."
      ELSE
        CR ." PRECESSING.."
      THEN
    THEN
    LISTFLAGS
  THEN
  USERR? IF
    IF
      TERMIF
        2CR
        ." Telescope mount errors are being corrected."
        CR
        ." Non-perpendicular axis offset is " DEGMSG
      THEN
      CR ." RHO=" FRHO F@ FDISPLAYDMS
    THEN
    LISTFLAGS
    IF
      TERMIF
        BCR AB." Collimation error is " DEGMSG
        4 SPACES
      ELSE
        CR
      THEN
      ." CE=" FCEW F@ FDISPLAYDMS
    THEN
    LISTFLAGS
  THEN
  IF
    TZMSG ." T ZONE=" GETZONE
    DSTFLAG C@ IF 1- THEN ( take out daylight savings time )
    . ( print it out )
    TERMIF
      2CR ." To see the first parameters page, hit <backspace>."
      KEYMSG
    THEN
  THEN
  WAITFORNEXT ( - flag/flag )
  DROP ( don't need flag for next if stmt )
  2CR 0=
UNTIL ;

DECIMAL
: SHOWIT ( command # - ) ( continue to track )
DUP SHWSTAR <
IF
  PRINTITEM ( n - n )
  DUP
  GETSMPOSITION ( n - Dra/Ddec )
  DISPOS ( n/Dra/Ddec - n )
THEN
DUP SHWSTAR =
IF
  GETSTAR ( - Dra/Ddec/mag )
  2CR ." MAG="
  DUP -1 = ( No self respecting stellar magnitude would be -1! )
  IF ( Print Solar Magnitude )
    ." -26.7" DROP
  ELSE
    ( mag=[n/24]-1.5 )
    0 D->F F24. F/
    F1.5 F- ( subtract 1.5 )
    FDUP FABS 100. D->F F* ( - fmagsign/fmag )
    F->D >R >R FSIGN F->S R> R>
    <# # # 46 HOLD # SIGN #> TYPE
  THEN
  TERMDISPLAY? 0= IF
    KEY DROP ( now wait for a key to be pressed to go on if using keypad )
  THEN
  DISPOS ( n/Dra/Ddec - n )
THEN
DUP SHWMESS =
IF
  GETMESSNGC ( - Dra/Ddec )
  DISPOS ( n/Dra/Ddec - n )
THEN
DUP SHWARB =
IF
  GETARB ( - Dra/Ddec )
  DISPOS ( n/Dra/Ddec - n )
THEN
DUP SHWTIME =
IF DISPTIME THEN
DUP SHWHERE =
IF DISCURPOS THEN
DUP SHWPARM =
IF LISTPARAMS THEN ;

( -- This is added for keeping track of target object -- )
: OBJDISPSET ( - )
( Setup to display object when aiming )
IDOBJADDR @ TRGOBJADDR !
TMPOBJTYPE C@ TRGOBJTYPE C!
TMPOBJNUM @ TRGOBJNUM ! ; ( record for display )

( ----- SCAN functions section starts here ----- )

: CHECKMSR ( msr#addr - Dra/Ddec/flag/next_addr/ngc_num )
DUP @ >R ( Put NGC number onto return stack )
( Let user see that we are doing something. )
DUP
TERMCR
TERMLINEUP
MSRLAB ( msr#addr - )
." ..."
DUP 2+ >R ( save address of next object )
@ ( msr#addr - object# )
MKTABADR ( object# - table_addr )
BTAB->RA/DEC ( table_addr - Dra/Ddec )
PRECESSIONTAB RA/DEC->AZ/AL ( - Dra/Ddec/Daz/Dal )
2SWAP 2DROP ( and drop Daz )
( now check to make sure it's at least 15 deg above horizon )
DECAL->FPI/2RAD ( Dal - frad )
FPI/2 6 S->F F/ F> ( - Dra/Ddec/flag )
R> R> ;

HEX
: SCANKEY ( - n )
0
BEGIN
  DROP
  KEY ( - n )
  DUP  30 =    ( "0" ) ( - n/flag )
  OVER 31 = OR ( "1" )
  OVER 4E = OR ( "N" )
  OVER 6E = OR ( "n" )
  OVER 59 = OR ( "Y" )
  OVER 79 = OR ( "y" )
  OVER ENTER = ( <cr> ) ( - n/flag/quitflag )
    IF
      2DROP ( get rid of key and flag )
      2DROP 2DROP ( get rid of Dra/Ddec vars )
      FDROP ( dump other stack vars )
      ENTER TRUEFLAG ( flags to finish this )
      R> ( get rid of return to SHWNGC? )
      R> ( get rid of return address to CHECKOBJ )
      R> ( get rid of return address to SCAN )
( now the next addr on the return stack will be of the )
( function that called SCAN. )
      FDROP
    THEN
UNTIL
DUP EMIT ( echo good output )
59 31 79 TRPLCMP ;

DECIMAL
: SHWNGC? ( Dra/Ddec - )
." ?" SCANKEY
IF
  RA/DEC->TARG ( Dra/Ddec - )
  OBJDISPSET ( Keep track of target )
  DISPOFF
ELSE
  2DROP 2DROP
THEN ;

: CHECKOBJ ( - tab#/tabaddr/obj# - )
( Let user see that we're working )
TERMCR TERMLINEUP ( )
3 PICK 7 =
IF ( Best Object )
  ( Each entry in the Best table is: tab#-1 byte/obj#-2 bytes )
  ( - tab#/tabaddr/obj# - ) ( duplicate this for object )
  OVER DUP ( - tab#/tabaddr/obj#/tabaddr/tabaddr )
  C@ ( get object type or tab# )
  SWAP 1+ @ ( Actually, this fetches the object # )

  ( It just needs to be processed here to get object address )
  ( - Btab#/Btabaddr/Bobj#/tab#/obj# )
  OVER 1 =
  IF ( Stars table )
    DUP 1- 5 * ( Offset, with 5 bytes per object )
    STRTAB ( table address )
    + ( Add offset to table base )
    >R ( save on return stack )
  THEN
  OVER 2 =
  IF ( Messier )
    DUP 1- 2* ( offset, since only 2 bytes are used per object )
    MSRTAB ( table address )
    + ( Add offset to table base )
    >R ( save on return stack )
  THEN
  OVER 3 =
  IF ( Caldwell )
    DUP 1- 2* ( offset, since only 2 bytes are used per object )
    MSRTAB ( table address )
    + ( Add offset to table base )
    220 + ( Offset past Messier table )
    >R ( save on return stack )
  THEN
  OVER 4 =
  IF ( Hershel )
    DUP CHKHRSHL ( hershel# - ngc# )
    MKTABADR ( object_num - table_addr )
    >R ( save on return stack )
  THEN
  OVER 4 >
  IF ( NGC/IC/Other )
    DUP MKTABADR ( object_num - table_addr )
    >R ( save on return stack )
  THEN
  R> SWAP ( - Btab#/Btabaddr/Bobj#/tab#/tabaddr/obj# )
  ( now we're ready to let the conditionals below handle it )
THEN
( save object number )
DUP TMPOBJNUM !
3 PICK 1 =
IF ( Star )
  0 IDOBJTYPE !
  DUP STARLAB
  DUP >R ( star object num to return stack )
THEN
3 PICK DUP 2 = SWAP 3 = OR
IF ( Messier/Caldwell )
  3 IDOBJTYPE !
  OVER DUP IDOBJADDR !
  MSRLAB ( msr#addr - )
  OVER @ >R ( save ngcic# to return stack )
THEN
3 PICK 4 =
IF ( Hershel )
  4 IDOBJTYPE !
  OVER 2+ ( - tab#/addr/obj#/addr+2 )
  DUP IDOBJADDR !
  HRSHLAB ( hershel#addr+2 - )
  OVER 2+ @ >R ( save ngcic# to return stack )
THEN
3 PICK 4 >
4 PICK 7 <> AND ( Not Best table )
IF ( NGC/IC/Other )
  2 IDOBJTYPE !
  OVER IDOBJADDR !
  DUP ( - tab#/addr/obj#/obj# )
  SHWOBJ ( ngcic# - tab#/addr/obj# )
  DUP >R ( save ngcic# to return stack )
THEN

." ..." R>
( 3 PICK 1 <> ( Old way )
IDOBJTYPE @ ( Will be 0 for stars )
IF
  MKTABADR ( tab#/tabaddr/obj#/object_num - tab#/tabaddr/obj#/table_addr )
  BTAB->RA/DEC ( - tab#/tabaddr/obj#/Dra/Ddec )
  PRECESSIONTAB
ELSE
  STARCOORDS ( n - Dra/Ddec/mag )
  ( A PRECESSIONTAB is done in STARCOORDS )
  DROP
THEN
RA/DEC->AZ/AL
2SWAP 2DROP
( - tab#/addr/obj#/Dra/Ddec/Dal )
( make sure altitude > 15. )
REFRACTION ( - alflag )
DROP DECAL->FPI/2RAD FPI/2 6 S->F F/ F>
( - tab#/addr/obj#/Dra/Ddec/flag )
IF ( object is above horizon, offer it as an option to see )
  CR
  7 PICK 1 =
  IF ( Star )
    6 PICK IDOBJADDR !
    5 PICK STARLAB
  THEN
  7 PICK DUP 2 = SWAP 3 = OR
  IF ( Messier/Caldwell )
    6 PICK MSRLAB ( msr#addr - )
  THEN
  7 PICK 4 =
  IF ( Hershel )
    6 PICK 2+ ( - tab#/addr/obj#/addr+2 )
    HRSHLAB ( hershel#addr+2 - )
  THEN
  7 PICK 4 >
  IF ( NGC/IC/other/Best )
    5 PICK ( - obj# )
    SHWOBJ ( ngcic# - )
  THEN
  IDOBJTYPE @ TMPOBJTYPE C! ( need this to label DISPOFF correctly )
  SHWNGC? ( Dra/Ddec - )
ELSE ( object below horizon )
  2DROP 2DROP ( throw out Dra/Ddec )
THEN

( clean up ) ( - tab#/addr/obj# )
3 PICK 4 =
IF ( Hershel )
  DROP 4 + DUP @
ELSE
  3 PICK 6 =
  IF ( IC or Other )
    1+ SWAP 4 + SWAP ( IC updated )
    DUP ICNUM 10001 + = ( First Other table )
    IF DROP DUP T@ THEN
    DUP ICNUM 10001 + > ( Rest of Other table )
    IF DROP DUP 2+ SWAP T@ THEN
  ELSE ( NGC, MSR, Caldwell )
   1+ SWAP 2+
   3 PICK 5 = IF 2+ THEN ( additional 2 bytes for NGC )
   SWAP
  THEN
THEN

BESTFLAG @ IF
   ( Need to also check to see if we are doing Best table )
   SCANTAB C@ 7 =
   IF ( fix up retrieval of Best object )
     FDROP ( throw away the dummy variables )
     SWAP 3 + ( update Best table address )
     SWAP 1+ ( increment object # )
     ( - tab#/tabaddr/obj# )
   THEN
THEN ; ( updated addr and obj# )

: SCAN ( - junk_num )
( Scan non-stellar tables function )
TERMIF
  2CR
  ." Scan function selected."
THEN
( Allow use of Best table - store flag in BESTFLAG - Use only one value! )
 TRUEFLAG ( ) ( FALSEFLAG ( )
TABLADR ( n - tab#/radecaddr ) ( radecaddr is ngcicaddr, EXCEPT for Best Tab )
SCANOBJ# @ ( - tabnum/radecaddr/objectnum )
( For MSR/Caldwell/Hershel/Best, replace radecaddr with table addr )
3 PICK 4 <
IF ( Messier + Part Caldwell ) ( - tabnum/radecaddr/objectnum )
  SWAP DROP ( - tabnum/objectnum )
  DUP 1- 2* MSRTAB + SWAP
THEN
3 PICK 3 =
IF ( Caldwell )
  SWAP ( - tabnum/objectnum/tabaddr )
  220 + SWAP
THEN
3 PICK 4 =
IF ( Hershel )
  SWAP DROP ( - tabnum/objectnum )
  HERSHELOFF ( offset to Hershel #'s )
  MSRTAB ( table address )
  + ( - tabnum/objectnum/tabaddr )
  BEGIN
    DUP @ ( - tabnum/objectnum/tabaddr/hershel# )
    DUP -1 <> ( - tabnum/objectnum/tabaddr/hershel#/flag )
    SWAP 4 PICK <> AND
  WHILE
    4 +
  REPEAT
  SWAP ( - tabnum/tabaddr/objectnum )
THEN
( Comment out, because already done by TABLADR )
( 3 PICK 7 = ( )
( IF ( Best Objects table ) ( - tabnum/radecaddr/objectnum )
  ( replace radecaddr with table addr )
  ( objectnum = 1 already for Best table here )
(   SWAP DROP ( - tabnum/objectnum )
(   BESTAB SWAP ( )
( THEN ( )
CR
DUP 0 <> ( 0 means no valid # was selected )
IF
  BEGIN ( look for object in table )
    3 PICK ( - tabnum/addr/objectnum/tabnum )
    DUP 2 = IF ( MSR ) OVER 111 < >R THEN
    DUP 3 = IF ( CLD ) OVER 110 < >R THEN
    DUP 4 = IF ( HERSHEL ) OVER -1 <> >R THEN
    DUP 5 = IF ( NGC ) OVER NGCNUM 1+ < >R THEN
( This sort of cheats with the different format of the Other table )
    DUP 6 = IF ( IC and Other ) OVER -1 <> >R THEN
    7 = IF ( Best ) OVER C@ 255 <> >R THEN
    R> ( retrieve flag )
  WHILE
    CHECKOBJ ( - tab#/radecaddr/obj# - )
  REPEAT
THEN
2DROP ;
( ----- End SCAN function section ----- )

HEX
void SETUPCOMMAND( int n ) {
COMMAND = n;
// Remove solar filter if not tracking Sun or Staying here )
//DUP FNDSUN = OVER STAYFUNC = OR 0=
//IF RMSOLFLTR THEN
DUP FNDSTAR < ( - n/flag )
IF ( Sun, Moon, or Planet )
  DUP 0F AND PRINTITEM ( - n/n' )
  ( Save type and # to show target later )
  TRGOBJNUM !
  5 TRGOBJTYPE = 5;
  GETSMPOSITION( n, Dra/Ddec ); // compute once
  RA/DEC->TARG ( Dra/Ddec - )
  FNDARB COMMAND C! ( and track it )
  DISPOFF
THEN
DUP FNDSTAR =
IF
  ( object type, num is set in GETSTAR )
  GETSTAR DROP ( don't need mag )
  RA/DEC->TARG ( Dra/Ddec - )
  OBJDISPSET
  DISPOFF
THEN
DUP FNDMESS =
IF
  GETMESSNGC RA/DEC->TARG
  OBJDISPSET
  DISPOFF
THEN
DUP SETOHNDBOX =
IF
  HANDBOX ( - az/al )
  6 TRGOBJTYPE C! ( store invalid object type # )
  STAYONAZAL( az, al ); // Track this position
THEN
DUP FNDARB =
IF
  GETARB
  6 TRGOBJTYPE C! ( store invalid object type # )
  RA/DEC->TARG ( Dra/Ddec - )
  DISPOFF
THEN
DUP STAYFUNC =
IF
  NOTRACK
  6 TRGOBJTYPE C! ( store invalid object type # )
THEN ( Don't move )
SCANFUNC = IF SCAN DROP THEN ; ( throw out scan addr )
}
// */
void MAIN(int cmd) {
  // Main processing loop
  readsensors(); //Update environment status
  oledprintData(); //Show it on a quick display, if present
  updatestatusscreen(); // Update Status screen
  if (GPSpresent == false) WAITASEC(1);
  if (CHKNUM()) {
    char num = KEY();
    if (num == 0x08) {
      // Initialization menu
      TERMclear();
      TCterminal.println(" Initialization menu");
      TCterminal.println(" 1 Time");
      TCterminal.println(" 2 Orientation");
      TCterminal.println(" 3 IR decode");
      TCterminal.println(" 4 TC Interface");
      num = KEY();
      // Reinit on Backspace
      if (num == '1') RESETIMEFLAG();
      if (num == '2') RESETINITFLAG();
      if (num == '3') RESETIRFLAG();
      if (num == '4') RESETTCIFLAG();
      if ((num < '1') || (num > '4')) printstatusscreen();
    }
  }
}
/*
: MAIN ( n - n )
BEGIN
  SETUPCOMMAND ( command # - ) ( set parameters )
  TERMALLOPTIONS
  BEGIN
    TRACK();
    FALSEFLAG ( for UNTIL ) ( - false flag )
    CHKNUM ( -command #/true or - false )
    IF
      TRACKINGCOMMAND?
      ( false flag/command # - false flag/command #/flag )
      IF
        SWAP 0=
        ( false flag/command # - command #/true flag )
      ELSE
        DUP SHWPARM =
        IF
          SHOWIT ( false flag/command # - false flag )
        THEN
        TERMALLOPTIONS
      THEN
    THEN ( -false or - command #/true )
  UNTIL ( command #/true - command # )
  INITCOMMAND? ( command # - command #/flag )
  SWAP
  TIMECOMMAND?
  ROT OR ( flag/command #/flag - command #/flag )
UNTIL ;
*/
