// TCDERVIO
// Telescope Controller 4.0 - Derived I/O functions
#include "tcheader.h"

void twoCR() { TCterminal.println("\n"); }

long GETIME( long* addr ) { // returns long type seconds
  // address of double # of seconds var
  long tmp = RDTIME();
  long lstoffset = 0L;
  if ( addr == &LST ) {
    // get seconds since then
    lstoffset = (tmp / 365L);
    // add seconds for correct LST
  }
  if ( addr == &LT) {
    // Take DST into account for Local Time
    lstoffset = (DSTFLAG?1:0) * 3600L;
  }
  return *addr + tmp + lstoffset;
}

void TIME( long* addr, int* days, int* hours, int* minutes, int* seconds ) {
  // returns sec/min/hrs/days
  long tmp = GETIME( addr );
  *seconds = tmp % 60L; //rem=sec, dquot=min
  *minutes = (tmp / 60L) % 60L; //rem=min, dquot=hrs
  *hours = (tmp / 3600L) % 24L; //rem=mins, quot=hrs
  *days = tmp / 86400L; //rem=hrs, quot=days
}

void DATEFIX() {
boolean flag;
byte updatednumofdays, monthdays;
int days, hours, minutes, seconds;
// Correct date variables for elapsed time
GRDAY = GBDAY;
GRMONTH = GBMONTH;
GRYEAR = GBYEAR;
TIME ( &UT, &days, &hours, &minutes, &seconds ); // sets sec/min/hrs/days
if ( days != 0 ) {
  // at least one extra day needs to be taken into account
  updatednumofdays = GRDAY + days;
  do { // keep looping until new days fit into month
    // check for leap year now
    flag = (((GRYEAR % 4) == 0) && (GRYEAR % 100)) || ((GRYEAR % 400) == 0);
    // 0 for normal year, 1 for leap year
    if (flag) { // Leap year
      monthdays = MONTHDAYS[GRMONTH + 13];
    } else {
      monthdays = MONTHDAYS[GRMONTH];
    }
    if (monthdays < updatednumofdays) {
      updatednumofdays -= monthdays; // subtract off month of days
      GRMONTH += 1; // increment month #
      if (GRMONTH == 13 ) {
        GRMONTH = 1; // Month goes back to January
        GRYEAR += 1; // increment year
      }
      flag = FALSEFLAG; // be sure to loop again
    } else {
      flag = TRUEFLAG; // okay to exit loop structure
    }
  } while ( !flag );
  GRDAY = updatednumofdays; // store updated day
}
}

int UTYEAR() { DATEFIX(); return GRYEAR ; }
byte UTMONTH() { DATEFIX(); return GRMONTH ; }
byte UTDAY() { DATEFIX(); return GRDAY ; }

void DISTIME( long* n ) { // n is address of double # of seconds )
  int days, hours, minutes, seconds;
  TIME(n, &days, &hours, &minutes, &seconds);
  if (hours < 10) { TCterminal.print("0"); }
  TCterminal.print(hours); TCterminal.print(":");
  if (minutes < 10) { TCterminal.print("0"); }
  TCterminal.print(minutes); TCterminal.print(":");
  if (abs(seconds) < 10) { TCterminal.print("0"); }
  TCterminal.print(abs(seconds));
}

void FDISPLAYDMS( double f ) {
  // display f in rad in +deg.mn.sc format
  double ffabs, fsecs;
  long dhours, dmin, dsecs;
  fsecs = F360div2PI * f; // f in seconds
  //TCterminal.print(fsecs); while(1) ;
  ffabs = fabs(fsecs);
  dsecs = (long)ffabs;
  dhours = dsecs / 3600L;
  dmin = (dsecs / 60L) - (dhours * 60L);
  dsecs = dsecs - (dmin * 60L) - (dhours * 3600L);
  if (fsecs < 0.0) TCterminal.print("-");
  TCterminal.print(dhours);
  TCterminal.print(".");
  if (dmin < 10) { TCterminal.print("0"); }
  TCterminal.print((dmin));
  TCterminal.print(".");
  if (dsecs < 10) { TCterminal.print("0"); }
  TCterminal.print((dsecs));
}

boolean TRPLCMP( int n1, int n2, int n3, int n4 ) {
  return (n4 == n1) || (n4 == n2) || (n4 == n3);
}

boolean GETYORN() { // True for Yes, False for No
  char n;
  boolean flag;
  do {
    n = KEY();
    flag = ((n == '0') || (n == '1')
       || (n == 'N') || (n == 'n')
       || (n == 'Y') || (n == 'y'));
  } while ( !flag );
  EMIT(n); // echo good input
  return TRPLCMP('Y', '1', 'y', n);
}

boolean HUNTquestion( int n ) {
 return TRPLCMP(0x48, 0x68, 0x4A, n); // H or J?
}

boolean DEFAULTquestion( int n ) {
 return TRPLCMP(ENTER, 0x64, 0x44, n); // <enter> or D?
}

boolean SKIPquestion( int n ) {
 return TRPLCMP(0x53, 0x73, 0x08, n); // S or backspace?
}

boolean INPUTquestion( int n ) {
  return TRPLCMP(0x49, 0x69, 0x50, n); // I or P?
}

long GETDDECNUM() {
  // read a line from user into buffer, return char count
  char PAD[10];
  inputstrlen = ACCEPT( PAD, 10 );
  return GETnum(PAD); //converted string to long
}

int GETINUM() { return (int)GETDDECNUM(); }

int GETINUM(int n) {
  // read a line from user into buffer, return char count
  char PAD[10];
  inputstrlen = ACCEPT( PAD, 10 );
  if (inputstrlen == 1) return n;
  return (int)GETnum(PAD); //converted string to long
}

long CONVHMS(char *buf) { // return d seconds
  // Convert an input string in hour.minutes.seconds format to a long
  long secs = 0L; // return seconds
  int x;
  int y = 0;
  int countdots = 0;
  hmsseconds = hmsmin = hmshours = 0;
  //Assume string is no longer than 20 characters
  for (x=0; x < 20; x++) {
    if (buf[x] == '.') {
      //A decimal point means we need to move to the next units
      hmshours = hmsmin;
      hmsmin = hmsseconds;
      secs += (long)y;
      hmsseconds = y;
      if (countdots > 1) return secs;
      secs *= 60L;
      countdots++;
      y = 0;
    } else if (buf[x] == 0) {
      //Done converting
      if (countdots == 0) {
        hmshours = y;
        secs = y * 3600L;
      } else if (countdots == 1) {
        hmshours = hmsmin;
        hmsmin = y;
        secs = (y + secs) * 60L;
        hmsseconds = 0;
      } else {
        hmshours = hmsmin;
        hmsmin = hmsseconds;
        secs += (long)y;
        hmsseconds = y;
      }
      return secs;
    } else {
      // add in the next digit
      y = (y * 10) + (buf[x] - '0');
    }
  }
}

long INPUTHMS() {
  char PAD[13];
  inputstrlen = ACCEPT(PAD, 12); //Get 12 chars max
  if (PAD[0] == '-') {
    //Minus sign present
    PAD[0] = '0'; //remove the sign char to get a positive number
    return -1L * CONVHMS(PAD);
  } else {
    return CONVHMS(PAD);
  }
}

double GETFDECNUM() {
  //Get a floating point number from user, in int.fraction format
  //Allow for negative numbers
  char PAD[13];
  int len;
  len = ACCEPT(PAD, 12); //Get 12 chars max
  return atof(PAD);
}

void readsensors() {
  //Update environment status
  if (BMEpresent) {
    FTEMPF = bme280.readTempF();
    FHUMID = bme280.readFloatHumidity();
    FPINHG = (bme280.readFloatPressure() * 0.0002953);
    FHEIGHT = bme280.readFloatAltitudeFeet();
  }
  if (GPSpresent) {
    FALTEMP = eecharbuf.strunion.ELEVATION = ubloxGPS.getAltitude() * 3.28084 / 1000.; //Elevation in feet
  }
  if (MagCompasspresent) {
    //HMC6352.Wake();
    //newdelay(10);
    FMAGHDG = HMC6352.GetHeading();
    //HMC6352.Sleep();
  }
  if (eecharbuf.strunion.INA219flag) { // Voltage monitor
    float shuntvoltage = 0;
    float busvoltage = 0;
    //float current_mA = 0;
    float loadvoltage = 0;
    //float power_mW = 0;

    shuntvoltage = ina219.getShuntVoltage_mV();
    busvoltage = ina219.getBusVoltage_V();
    //current_mA = ina219.getCurrent_mA();
    //power_mW = ina219.getPower_mW();
    busvolts = loadvoltage = busvoltage + (shuntvoltage / 1000);
  }
}
