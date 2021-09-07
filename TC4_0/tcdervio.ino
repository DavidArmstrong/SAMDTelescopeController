// TCDERVIO
// Telescope Controller 4.0 - Derived I/O functions
#include "tcheader.h"

void printDegMinSecs(double n) {
  boolean sign = (n < 0.);
  if (sign) n = -n;
  long lsec = n * 360000.0;
  long deg = lsec / 360000;
  long min = (lsec - (deg * 360000)) / 6000;
  float secs = (lsec - (deg * 360000) - (min * 6000)) / 100.;
  if (sign) TCterminal.print("-");
  if (abs(deg) < 10) { TCterminal.print("0"); }
  TCterminal.print(deg); TCterminal.print(":");
  if (abs(min) < 10) { TCterminal.print("0"); }
  TCterminal.print(min); TCterminal.print(":");
  if (abs(secs) < 10) { TCterminal.print("0"); }
  TCterminal.print((int)abs(secs)); TCterminal.print(" ");
}

void printTime(double n) {
  boolean sign = (n < 0.);
  if (sign) n = -n;
  long lsec = n * 3600.0;
  long deg = lsec / 3600;
  long min = (lsec - (deg * 3600)) / 60;
  float secs = (lsec - (deg * 3600) - (min * 60));
  //if (sign) TCterminal.print("-");
  print2digitsUSB(deg); TCterminal.print(":");
  print2digitsUSB(min); TCterminal.print(":");
  print2digitsUSB(abs(secs)); TCterminal.print(" ");
}

void print2digitsUSB(int number) {
  if (number < 10) {
    TCterminal.print("0");
  }
  TCterminal.print(number);
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

