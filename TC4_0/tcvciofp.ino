// TCVCIOFP
// Telescope Controller 4.0 - Basic I/O
#include "tcheader.h"

char KEY() { // return a character of input from any source
  char n = ' ';
  long xnL;
  boolean rcvd = false;

  do {
  if (TCterminal.available()) {
    n = TCterminal.read();
    rcvd = true;
  } else {
    if (TC_LCD.available()) {
      n = TC_LCD.read();
      rcvd = true;
    } else {
      xnL = 0L;
      if (irsetup && (IRkey != 0L)) {
        // we have already read a key
        xnL = IRkey;
        IRkey = 0L;
      } else {
        if (irrecv.decode(&results)) {
          xnL = results.value;
          if (xnL == 0xffffffffL) xnL = 0L;
          irrecv.resume(); // Receive the next value
        }
      }
      
      if (irsetup && (xnL != 0L)) {
        // Return an IR input char if IR Table has been initialized
        for (int i=0; i < 20; i++) {
          if ((rcvd == false) && (xnL == eecharbuf.strunion.IRinput[i])) {
            n = eecharbuf.strunion.IRchar[i];
            rcvd = true;
            //break;
          }
        }
      }
    }
  }
  } while (rcvd == false);
  return n;
}

void EMIT(char n) {
  TCterminal.write(n);
  LCDprint(n);
}

long GETnum(char* buf) { //convert string to double number
  return atol(buf);
}

void LCDline1() { // move cursor to beginning of first line
  if (eecharbuf.strunion.LCDi2cflag && !eecharbuf.strunion.LCDpicflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write(254); //Send command character
    Wire.write(128); //Change the position (128) of the cursor
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag) {
    TC_LCD.write(254); //Send command character
TC_LCD.write(128); //Change the position (128) of the cursor to 1st row (0), position 0
  }
}

void LCDline2() { // move cursor to beginning of second line
  if (eecharbuf.strunion.LCDi2cflag && !eecharbuf.strunion.LCDpicflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write(254); //Send command character
    Wire.write(128 + 64); //Change the position (128) of the cursor
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag) {
    TC_LCD.write(254); //Send command character
TC_LCD.write(128 + 64); //Change the position (128) of the cursor to 2nd row (64), position 0
  }
}

void LCDline3() { // move cursor to beginning of third line
  if (eecharbuf.strunion.LCDi2cflag && !eecharbuf.strunion.LCDpicflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write(254); //Send command character
    Wire.write(128 + 20); //Change the position (128) of the cursor
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag) {
    TC_LCD.write(254); //Send command character
TC_LCD.write(128 + 20); //Change the position (128) of the cursor to 3rd row (20), position 0
  }
}

void LCDline4() { // move cursor to beginning of fourth line
  if (eecharbuf.strunion.LCDi2cflag && !eecharbuf.strunion.LCDpicflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write(254); //Send command character
    Wire.write(128 + 84); //Change the position (128) of the cursor
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag) {
    TC_LCD.write(254); //Send command character
TC_LCD.write(128 + 84); //Change the position (128) of the cursor to 4th row (84), position 0 (0)
  }
}

void LCDclear() { // Clear display and home cursor
  if (eecharbuf.strunion.LCDi2cflag && !eecharbuf.strunion.LCDpicflag) {
    // OpenLCD (AVR) based LCD on QWIIC I2C bus
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write('|'); //Put LCD into setting mode
    Wire.write('-'); //Send clear display command
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag && !eecharbuf.strunion.LCDi2cflag) {
    // PIC based LCD on TC_LCD TxD line
    TC_LCD.write(0xfe); // = 254 decimal
    TC_LCD.write(0x01);
  } else if (eecharbuf.strunion.LCDi2cflag && eecharbuf.strunion.LCDpicflag) {
    // OpenLCD on TC_LCD TXD line
    TC_LCD.write('|'); //Send setting character = 124 = 0x7c
    TC_LCD.write('-'); //Send clear display character
  }
}

void LCDprint(int tmp) { // Print to LCD
  if (eecharbuf.strunion.LCDi2cflag && !eecharbuf.strunion.LCDpicflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag) {
    TC_LCD.print(tmp);
  }
}

void LCDprint(double tmp, int frac) { // Print to LCD
  if (eecharbuf.strunion.LCDi2cflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp, 2);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag) {
    TC_LCD.print(tmp, frac);
  }
}

void LCDprint(char tmp) { // Print to LCD
  if (eecharbuf.strunion.LCDi2cflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag) {
    TC_LCD.print(tmp);
  }
}

void LCDprint(char* tmp) { // Print to LCD
  if (eecharbuf.strunion.LCDi2cflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag) {
    TC_LCD.print(tmp);
  }
}

void LCDprint(const char* tmp) { // Print to LCD
  if (eecharbuf.strunion.LCDi2cflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDpicflag) {
    TC_LCD.print(tmp);
  }
}

void SETINITFLAG() { INITFLAG = true; }
void RESETINITFLAG() { INITFLAG = false; }
boolean NOTINITquestion() { return !INITFLAG; }
void SETIMEFLAG() { TIMEFLAG = true; }
void RESETIMEFLAG() { TIMEFLAG  = false; }
boolean NOTIMESETquestion() { return !TIMEFLAG; }
boolean INITCOMMANDquestion() { return INITCMD == COMMAND; }
boolean TIMECOMMANDquestion() { return SETIMECMD == COMMAND; }
void SETIRFLAG() { IRFLAG = true; }
void RESETIRFLAG() { IRFLAG = false; }
boolean IRquestion() { return !IRFLAG; }
void SETTCIFLAG() { TCIFLAG = true; }
void RESETTCIFLAG() { TCIFLAG = false; }
boolean TCIquestion() { return !TCIFLAG; }
void SETPRECESSFLAG() { PFLAG = true; }
void RESETPRECESSFLAG() { PFLAG = false; }
boolean PRECESSquestion() { return PFLAG; }
void SETREFRACTFLAG() { RFLAG = true; }
void RESETREFRACTFLAG() { RFLAG = false; }
boolean REFRACTquestion() { return RFLAG; }
void SETERRFLAG() { ERRFLAG = true; }
void RESETERRFLAG() { ERRFLAG = false; }


// (This was lifted from a web blog - somewhere, then heavily modified)
//    int len;
//    char buffer[32];
//    Serial.print ("\r\nPlease type something: ");
//    len = ACCEPT (buffer, 32);
int ACCEPT(char *buf, int limit) {
  // read a line from input into buffer, return char count
  int x;
  int ptr = 0;
  *buf = 0;

  for (x = 0; x < limit; x++) { buf[x] = 0; }
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
        LCDprint("\b \b"); // erase bs'd char
      }
    } else {
      if (ptr < (limit - 1)) {
        TCterminal.print ((char) x); // echo char
        LCDprint((char) x); // echo char
        buf[ptr] = x; // store in buffer
        ptr++;
      }
    }
  }
}

boolean RDLBTN() {
  //return Lock button status
  return !digitalRead(LOCKBTN); //!RBTN(n);
}

void alarmMatch() {
  // Increment seconds counter at the one minute mark
  TIMER += 60L;
}

void TERMLINEUP() {
  // Moves up one line on a serial display
  //TCterminal.write(0x1b); // esc
  //TCterminal.write(0x5b); // [
  //TCterminal.write(0x41); // A
  ansi.cursorUp(1);
}

void TERMclear() {
  TCterminal.print(" ");
  newdelay(500);
  //Clear VT100 screen and home cursor
  /*
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  TCterminal.write(0x48); // H - Go to home position
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  TCterminal.write(0x32); // 2
  TCterminal.write(0x4a); // J - Clear the screen
  */
  ansi.clearScreen();
}

void TERMcursor() {
  //Set VT100 cursor to underline
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  TCterminal.write(3);
  TCterminal.print(" q");
}

void TERMxy(int x, int y) {
  newdelay(40); // Serialx has a small Tx buffer
  // Position cursor on VT100 screen at coordinates x,y
  /*
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  TCterminal.print(y); // vertical
  TCterminal.write(0x3b); // ; or semi-colon
  TCterminal.print(x); // horizontal
  TCterminal.write(0x48); // H - Go to x,y position
  */
  ansi.gotoXY(y,x);
}

void TERMtextcolor( char buf ) {
  //Set foreground text color on vt100 terminal screen
  int tmp;
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  switch (buf) {
    case 'r':  case 'R': tmp = 31; break;
    case 'g':  case 'G': tmp = 32; break;
    case 'y':  case 'Y': tmp = 33; break;
    case 'b':  case 'B': tmp = 34; break;
    case 'm':  case 'M': tmp = 35; break;
    case 'c':  case 'C': tmp = 36; break;
    default: tmp = 37;
  }
  TCterminal.print(tmp); // color code
  TCterminal.write(0x6d); // m
}

void WAITASEC(int n) {
  // Wait until the next second tick
  long initialTime = rtczero.getSeconds();

  for (int x = 0; x < n; x++) {
    while (initialTime == rtczero.getSeconds()) {
      ; //Wait
    }
    initialTime == rtczero.getSeconds();
  }
}

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

boolean CHKNUM() {
  //Check for number between 0 and $20
  if (irrecv.decode(&results)) {
    IRkey = results.value;
    if (IRkey == 0xffffffffL) IRkey = 0L;
    irrecv.resume(); // Receive the next value
  }
  boolean flag = TCterminal.available() || TC_LCD.available() || (IRkey != 0L);
  return flag;
}

