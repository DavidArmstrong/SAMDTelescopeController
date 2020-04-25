// TCVCIOFP
// Telescope Controller 4.0 - Basic I/O
#include "tcheader.h"

int FtoS(double f) { return (int)f; }

long FtoD(double f) { return (long)f; }

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
boolean USERRquestion() { return ERRFLAG; }
boolean FORKquestion() { return EMOUNTFLAG; }


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

// Replace this with RDLBTN()
//boolean HSIG() {
  // return flag of Lock button status
//  return digitalRead(LOCKBTN);
//}

boolean EQquestion() {
  //Equatorial mount flag
  // 0 = AltAz, 8 = Equatorial mount
  return EQUflag;
}

void MTRSON (int azdir, long Dtraaz, long Dtdecal) { // Turn on motors towards target
  
}

void ZEROAZ(long Dzraaz) {
  // Reset Azimuth counter
  RAAZ = RAAZenc.read();
  RAAZ -= Dzraaz;
  if (RAAZ < 0) RAAZ += MRAAZ;
  RAAZenc.write(RAAZ);
}

void RDAZAL() {
  // Return current AZimuth, ALtitude
  RAAZ = RAAZenc.read();
  DECAL = DECALenc.read();
}

long ITAZAL(long Drraaz, long Drdecal, boolean eqflag) {
  //Initialize AZimuth, ALtitude counters
  //( Drraaz/Drdecal/eqflag - Dmraaz/Dmdecal/Deqlat )
  // MRAAZ = Maximum encoder counts in RA/Azimuth ( 3 bytes each )
  // MDECAL = Maximum encoder counts in Declination/Altitude
  EQFLAG = eqflag;
  if (NOLIM)
    return INOLIM(Drraaz, Drdecal);
  else
    return ITAZALwithLIM(Drraaz, Drdecal);
}

long INOLIM(long Drraaz, long Drdecal) {
  
  return 0L;
}

long ITAZALwithLIM(long Drraaz, long Drdecal) {
  TURNAZ = 0;
  TURNAL = TURNAZ;
  RAAZ = 0L; //Will stop with scope at limit=position 0
  TDECAL = RAAZ; //Goto Horizon first
  //Make target position ridiculously high for Azimuth
  MRAAZ = 0xFFFFFFFFL; //Assume max number of encoder counts
  MDECAL = MRAAZ;
  DECAL = MDECAL; //Assume current Alt. high
  TRAAZ = DECAL; //Target AZ = max counts
  //Output message of where we're going
  if (EQquestion()) {
    const char* tmp = "RA Meridian  ";
    LCDline1();
    TCterminal.println(tmp);
    LCDprint(tmp);
  } else {
    const char* tmp = "Aximuth Start  ";
    LCDline1();
    TCterminal.println(tmp);
    LCDprint(tmp);
  }

  
  return 0L;
}

boolean KYSTAT() {
  // Return status of char availability
  return CHKNUM();
}

int GETZONE() {
  return TZONE;
}

boolean RDLBTN() {
  //return Lock button status
  return !digitalRead(LOCKBTN); //!RBTN(n);
}

void SETIME(long dtime) {
  //update seconds time count
  TIMER = dtime - rtczero.getSeconds();
//  SETRTC();
}

long RDTIME() {
  //Return time in seconds
  return TIMER + rtczero.getSeconds();
}

void alarmMatch() {
  // Increment seconds counter at the one minute mark
  TIMER += 60L;
}

void TERMLINEUP() {
  // Moves up one line on a serial display
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  TCterminal.write(0x41); // A
}

void TERMclear() {
  TCterminal.print(" ");
  newdelay(500);
  //Clear VT100 screen and home cursor
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  TCterminal.write(0x48); // H - Go to home position
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  TCterminal.write(0x32); // 2
  TCterminal.write(0x4a); // J - Clear the screen
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
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  TCterminal.print(y); // vertical
  TCterminal.write(0x3b); // ; or semi-colon
  TCterminal.print(x); // horizontal
  TCterminal.write(0x48); // H - Go to x,y position
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

void Fdot(double f) {
  TCterminal.print(f, 2);
  LCDprint(f, 2);
}

int ds1307_read( int address ) {
  // Read a byte from RAM
  if (address < 8 || address > 0x3f) return 0xff;
  Wire.beginTransmission(DS1307_ADR);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADR, (uint8_t) 1);
  
  return Wire.read();
}

void ds1307_write( int address, int value ) {
  // Write a byte to RAM
  if (address < 8 || address > 0x3f) return;
  Wire.beginTransmission(DS1307_ADR);
  Wire.write(address);
  Wire.write(value);
  Wire.endTransmission();
}
/* We now deal with EEPROM as a structure - read/write all at once
byte CatCLK(int eeaddress) {
  //Read byte value from EEPROM
  Wire.beginTransmission(EEPROM_ADR);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_ADR, 1);
  byte rdata = 0xFF;
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

int atCLK(int eeaddress) {
  //Read word value from EEPROM
  int msb = CatCLK(eeaddress);
  int lsb = CatCLK(eeaddress + 1);
  return (msb << 8) & lsb;
}

void CstoreCLK (byte data, int eeAddress) {
  // write byte data into EEPROM at addr
  Wire.beginTransmission(EEPROM_ADR);
  Wire.write((int)(eeAddress >> 8)); // MSB
  Wire.write((int)(eeAddress & 0xFF)); // LSB

  //Write byte to EEPROM
  Wire.write(data); //Write the data
  Wire.endTransmission(); //Send stop condition
}

void storeCLK (int data, int eeAddress) {
  // write word data into EEPROM at addr
  Wire.beginTransmission(EEPROM_ADR);
  Wire.write((int)(eeAddress >> 8)); // MSB
  Wire.write((int)(eeAddress & 0xFF)); // LSB

  //Write bytes to EEPROM
  byte LSB = data & 0xFF;
  byte MSB = (data & 0xFF00) >> 8;
  Wire.write(MSB); //Write the data
  Wire.write(LSB); //Write the data
  Wire.endTransmission(); //Send stop condition
}

long threeBatCLK(int eeaddress) {
  // return 3 byte contents in EERPOM starting at addr
  int msb = CatCLK(eeaddress);
  int mid = CatCLK(eeaddress + 1);
  int lsb = CatCLK(eeaddress + 2);
  return (msb << 16) & (mid << 8) & lsb;
}

void threeBstoreCLK(long data, int eeAddress) {
  // Write long data into EEPROM at addr as three bytes
  Wire.beginTransmission(EEPROM_ADR);
  Wire.write((int)(eeAddress >> 8)); // MSB
  Wire.write((int)(eeAddress & 0xFF)); // LSB

  //Write bytes to EEPROM
  byte LSB = data & 0xFF;
  byte middleSB = (data & 0x00FF00) >> 8;
  byte MSB = (data >> 16) & 0xFF;
  Wire.write(MSB); //Write the data
  Wire.write(middleSB); //Write the data
  Wire.write(LSB); //Write the data
  Wire.endTransmission(); //Send stop condition
}
*/
