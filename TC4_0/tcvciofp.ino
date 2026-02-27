/* Telescope Controller 4.00.00 - Variables, Constants, and Basic I/O related routines
// February 2026
// See MIT LICENSE.md file and ReadMe.md file for essential information
// Highly tailored to the AdaFruit M4 Metro
// DO NOT ATTEMPT TO LOAD THIS ONTO A STANDARD UNO */

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
          xnL = 0L;
          if (irrecv.getResults()) {
            irdecoder.decode();  //Decode it
            xnL = irdecoder.value;
            if (xnL == 0xffffffffL) xnL = 0L;
            irrecv.enableIRIn(); // Receive the next value
          }
        }

        if (irsetup && (xnL != 0L)) {
          // Return an IR input char if IR Table has been initialized
          for (int i = 0; i < 20; i++) {
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

long GETnum(char* buf) { //convert string to long number
  return atol(buf);
}

void LCDline1() { // move cursor to beginning of first line
  if (eecharbuf.strunion.LCDavrflag && !eecharbuf.strunion.LCDserialflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write(254); //Send command character
    Wire.write(128); //Change the position (128) of the cursor
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.write(254); //Send command character
    TC_LCD.write(128); //Change the position (128) of the cursor to 1st row (0), position 0
  }
}
void LCDline2() { // move cursor to beginning of second line
  if (eecharbuf.strunion.LCDavrflag && !eecharbuf.strunion.LCDserialflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write(254); //Send command character
    Wire.write(128 + 64); //Change the position (128) of the cursor
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.write(254); //Send command character
    TC_LCD.write(128 + 64); //Change the position (128) of the cursor to 2nd row (64), position 0
  }
}
void LCDline3() { // move cursor to beginning of third line
  if (eecharbuf.strunion.LCDavrflag && !eecharbuf.strunion.LCDserialflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write(254); //Send command character
    Wire.write(128 + 20); //Change the position (128) of the cursor
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.write(254); //Send command character
    TC_LCD.write(128 + 20); //Change the position (128) of the cursor to 3rd row (20), position 0
  }
}
void LCDline4() { // move cursor to beginning of fourth line
  if (eecharbuf.strunion.LCDavrflag && !eecharbuf.strunion.LCDserialflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write(254); //Send command character
    Wire.write(128 + 84); //Change the position (128) of the cursor
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.write(254); //Send command character
    TC_LCD.write(128 + 84); //Change the position (128) of the cursor to 4th row (84), position 0 (0)
  }
}
void LCDclear() { // Clear display and home cursor
  if (eecharbuf.strunion.LCDavrflag && !eecharbuf.strunion.LCDserialflag) {
    // OpenLCD (AVR) based LCD on QWIIC I2C bus
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write('|'); //Put LCD into setting mode
    Wire.write('-'); //Send clear display command
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag && !eecharbuf.strunion.LCDavrflag) {
    // PIC based LCD on TC_LCD TxD line
    TC_LCD.write(0xfe); // = 254 decimal
    TC_LCD.write(0x01);
  } else if (eecharbuf.strunion.LCDavrflag && eecharbuf.strunion.LCDserialflag) {
    // OpenLCD on TC_LCD TXD line
    TC_LCD.write('|'); //Send setting character = 124 = 0x7c
    TC_LCD.write('-'); //Send clear display character
  }
}
void LCDbrighter() {
  int tmp = eecharbuf.strunion.LCDbrightness++;
  int clear = 0x0;
  if (eecharbuf.strunion.LCDavrflag && !eecharbuf.strunion.LCDserialflag) {
    // OpenLCD (AVR) based LCD on QWIIC I2C bus
    if (tmp < 0x0) tmp = 0x0;
    if (tmp > 0xff) tmp = 0xff;
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write('|'); //Put LCD into setting mode
    Wire.write('+'); //Send RGB display command
    Wire.write(tmp); //Red
    Wire.write(clear); //Green
    Wire.write(clear); //Blue
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag && !eecharbuf.strunion.LCDavrflag) {
    // PIC based LCD on TC_LCD TxD line
    if (tmp < 0x80) tmp = 0x80;
    if (tmp > 0x9d) tmp = 0x9d;
    TC_LCD.write(0x7c); // =  decimal
    TC_LCD.write(tmp);
  } else if (eecharbuf.strunion.LCDavrflag && eecharbuf.strunion.LCDserialflag) {
    // OpenLCD on TC_LCD TXD line
    if (tmp < 0x0) tmp = 0x0;
    if (tmp > 0xff) tmp = 0xff;
    TC_LCD.write('|'); //Send setting character = 124 = 0x7c
    TC_LCD.write('+'); //Send RGB display character
    TC_LCD.write(tmp); //Red
    TC_LCD.write(clear); //Green
    TC_LCD.write(clear); //Blue
  }
  eecharbuf.strunion.LCDbrightness = tmp;
}
void LCDdimmer() {
  int tmp = eecharbuf.strunion.LCDbrightness--;
  int clear = 0x0;
  if (eecharbuf.strunion.LCDavrflag && !eecharbuf.strunion.LCDserialflag) {
    // OpenLCD (AVR) based LCD on QWIIC I2C bus
    if (tmp < 0x0) tmp = 0x0;
    if (tmp > 0xff) tmp = 0xff;
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.write('|'); //Put LCD into setting mode
    Wire.write('+'); //Send RGB display command
    Wire.write(tmp); //Red
    Wire.write(clear); //Green
    Wire.write(clear); //Blue
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag && !eecharbuf.strunion.LCDavrflag) {
    // PIC based LCD on TC_LCD TxD line
    if (tmp < 0x80) tmp = 0x80;
    if (tmp > 0x9d) tmp = 0x9d;
    TC_LCD.write(0x7c); // =  decimal
    TC_LCD.write(tmp);
  } else if (eecharbuf.strunion.LCDavrflag && eecharbuf.strunion.LCDserialflag) {
    // OpenLCD on TC_LCD TXD line
    if (tmp < 0x0) tmp = 0x0;
    if (tmp > 0xff) tmp = 0xff;
    TC_LCD.write('|'); //Send setting character = 124 = 0x7c
    TC_LCD.write('+'); //Send RGB display character
    TC_LCD.write(tmp); //Red
    TC_LCD.write(clear); //Green
    TC_LCD.write(clear); //Blue
  }
  eecharbuf.strunion.LCDbrightness = tmp;
}
void LCDprint(int tmp) { // Print to LCD
  if (eecharbuf.strunion.LCDavrflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.print(tmp);
  }
}
void LCDprint(long tmp) { // Print to LCD
  if (eecharbuf.strunion.LCDavrflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.print(tmp);
  }
}
void LCDprint(double tmp, int frac) { // Print to LCD
  if (eecharbuf.strunion.LCDavrflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp, 2);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.print(tmp, frac);
  }
}
void LCDprint(char tmp) { // Print to LCD
  if (eecharbuf.strunion.LCDavrflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.print(tmp);
  }
}
void LCDprint(char* tmp) { // Print to LCD
  if (eecharbuf.strunion.LCDavrflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.print(tmp);
  }
}
void LCDprint(const char* tmp) { // Print to LCD
  if (eecharbuf.strunion.LCDavrflag) {
    Wire.beginTransmission(LCDi2c_ADR);
    Wire.print(tmp);
    Wire.endTransmission();
  } else if (eecharbuf.strunion.LCDserialflag) {
    TC_LCD.print(tmp);
  }
}

void SETINITFLAG() {
  INITFLAG = true;
}
void RESETINITFLAG() {
  INITFLAG = false;
}
boolean NOTINITquestion() {
  return !INITFLAG;
}
void SETIMEFLAG() {
  TIMEFLAG = true;
}
void RESETIMEFLAG() {
  TIMEFLAG  = false;
}
boolean NOTIMESETquestion() {
  return !TIMEFLAG;
}
boolean INITCOMMANDquestion() {
  return INITCMD == COMMAND;
}
boolean TIMECOMMANDquestion() {
  return SETIMECMD == COMMAND;
}
void SETIRFLAG() {
  IRFLAG = true;
}
void RESETIRFLAG() {
  IRFLAG = false;
}
boolean IRquestion() {
  return !IRFLAG;
}
void SETTCIFLAG() {
  TCIFLAG = true;
}
void RESETTCIFLAG() {
  TCIFLAG = false;
}
boolean TCIquestion() {
  return !TCIFLAG;
}
void SETdisplayFLAG() {
  displayFLAG = true;
}
void RESETdisplayFLAG() {
  displayFLAG = false;
}
boolean Displayquestion() {
  return !displayFLAG;
}
void SETPIDFLAG() {
  PIDFLAG = true;
}
void RESETPIDFLAG() {
  PIDFLAG = false;
}
boolean PIDquestion() {
  return !PIDFLAG;
}
void SETERRFLAG() {
  ERRFLAG = true;
}
void RESETERRFLAG() {
  ERRFLAG = false;
}

/* (This was lifted from a web blog - somewhere, then heavily modified)
  //    int len;
  //    char buffer[32];
  //    Serial.print ("\r\nPlease type something: ");
  //    len = ACCEPT (buffer, 32); */
int ACCEPT(char *buf, int limit) {
  // read a line from input into buffer, return char count
  int x;
  int ptr = 0;
  *buf = 0;

  for (x = 0; x < limit; x++) {
    buf[x] = 0;
  }
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

void TERMclear() {
  //Clear VT100 screen and home cursor
  ansi.clearScreen();
}
void TERMlineup() {
  // Move cursor up one line
  ansi.cursorUp(1);
}
void TERMcursor() {
  //Set VT100 cursor to underline
  TCterminal.write(0x1b); // esc
  TCterminal.write(0x5b); // [
  TCterminal.write(3);
  TCterminal.print(" q");
}
void TERMxy(int x, int y) {
  newdelay(20); // Serialx has a small Tx buffer
  // Position cursor on VT100 screen at coordinates x,y
  //ansi.gotoXY(y, x);
  ansi.gotoXY(x, y); // Change for ANSI 2.0 library
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
  if (n < 1) return;
  // Wait until the next second tick
  DateTime now = rtczero.now();
  long initialTime = now.second();

  for (int x = 0; x < n; x++) {
    now = rtczero.now();
    while (initialTime == now.second()) {
      now = rtczero.now();
    }
    initialTime = now.second();
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
  //Check for a key press, somewhere
  // Used to use: irrecv.decode(&results)
  IRkey = 0L;
  if (irrecv.getResults()) {
    irdecoder.decode();  //Decode it
    IRkey = irdecoder.value;
    if (IRkey == 0xffffffffL) IRkey = 0L;
    irrecv.enableIRIn(); // Receive the next value
  }
  boolean flag = TCterminal.available() || TC_LCD.available() || (IRkey != 0L);
  return flag;
}

#ifdef __HMC6352__
boolean getMagCompassPresent() {
  return MagCompasspresent;
}
#endif
float magneticDeclination(float Latitude, float Longitude, uint8_t year, uint8_t month, uint8_t day) {
  float declination; // magnetic variation from True North
  declination = myDeclination.magneticDeclination(Latitude, Longitude, year, month, day);
  spData.declination = declination;
  return declination;
}

#ifdef __HMC6352__
float getMagCompassHeading() {
  float FMAGHDG;
  HMC6352.Wake();
  newdelay(10);
  FMAGHDG = HMC6352.GetHeading();
  HMC6352.Sleep();
  return FMAGHDG;
}
#endif

double getMMC5983MagCompassHeading() {
  unsigned int rawValue = 0;
    double heading = 0;
    double normalizedX = 0;
    double normalizedY = 0;
    double normalizedZ = 0;

    rawValue = MMC5983MAmag.getMeasurementX();
    normalizedX = (double)rawValue - 131072.0;
    normalizedX /= 131072.0;

    rawValue = MMC5983MAmag.getMeasurementY();
    normalizedY = (double)rawValue - 131072.0;
    normalizedY /= 131072.0;

    rawValue = MMC5983MAmag.getMeasurementZ();
    normalizedZ = (double)rawValue - 131072.0;
    normalizedZ /= 131072.0;

    // Magnetic north is oriented with the Y axis
    if (normalizedY != 0)
    {
        if (normalizedX < 0)
        {
            if (normalizedY > 0)
                heading = 57.2958 * atan(-normalizedX / normalizedY); // Quadrant 1
            else
                heading = 57.2958 * atan(-normalizedX / normalizedY) + 180; // Quadrant 2
        }
        else
        {
            if (normalizedY < 0)
                heading = 57.2958 * atan(-normalizedX / normalizedY) + 180; // Quadrant 3
            else
                heading = 360 - (57.2958 * atan(normalizedX / normalizedY)); // Quadrant 4
        }
    }
    else
    {
        // atan of an infinite number is 90 or 270 degrees depending on X value
        if (normalizedX > 0)
            heading = 270;
        else
            heading = 90;
    }
    return heading;
}

boolean getRockerTiltPresent() {
  return rockerTiltPresent;
}
boolean getTubeTiltPresent() {
  return tubeTiltPresent;
}

double getAltitude() {
  //DECAL = DECALenc.read();
  double faltitudenow = (double)DECAL * 90.0 / (double)RDECAL;
  return faltitudenow;
}
double getAzimuth() {
  // Return Azimuth degrees
  //TcRAAZ = (TRA * (double)RRAAZ / 360.) - AzimuthMagneticEncoderOffset - magVariationInAzimuthCounts;
  RAAZ = RAAZenc.read();
  double fazimuthnow = 0.;
  if (magOffsetComputed) {
  fazimuthnow = (double)(RAAZ + AzimuthMagneticEncoderOffset + magVariationInAzimuthCounts) * 360.0 / (double)RRAAZ;
  } else {
  fazimuthnow = (double)(RAAZ) * 360.0 / (double)RRAAZ;
  }
  return fazimuthnow;
}

boolean getAzRefSensor() {
  // Return true if Azimuth Reference Sensor, which is a Hall Effect Sensor, (Non-Latching)
  // is detecting the magnet mounted in the bottom board of the telescope.
  // This provides a fixed reference point for all Azimuth measurements,
  // initialization, and alignments.
  
  return (digitalRead(AZREFsensor) == LOW);
}

boolean startMotorToTarget(int motor, int direction, unsigned long currentPosition, unsigned long targetPosition) {
  // We define here the routine to drive the Azimuth or Altitude motors.
  // So if any changes need to be made, it only has to be done here, not everywhere.
  // uses i2cMotorDriver.setDrive( motorNum, direction, level ) to drive the motor
  int currentSpeed, currentDir, speed;
  unsigned long previousMillis = 0L;
  float Kp, Ki, Kd, error[3], N, tau;
  float d0, d1, fd0, fd1;
  float maxRange;
  if ( (motor != ALTITUDE_MOTOR) && (motor != AZIMUTH_MOTOR) ) return false;
  if ( (direction != CW_DIRECTION) && (direction != CCW_DIRECTION) ) return false;

  if (motor == AZIMUTH_MOTOR) {
    currentSpeed = currentAzMtrSpeed;
    currentDir = currentAzMtrDir;
	maxRange = RRAAZ;
	previousMillis = AzPreviousMillis;
	Kp = azKp;
	Ki = azKi;
	Kd = azKd;
	for (int i = 0; i < 3; i++) error[i] = azError[i];
	N = azN;
	tau = azTau;
	d0 = azD0;
    d1 = azD1;
	fd0 = azFd0;
	fd1 = azFd1;
  }
  if (motor == ALTITUDE_MOTOR) {
    currentSpeed = currentAlMtrSpeed;
    currentDir = currentAlMtrDir;
	maxRange = RDECAL;
	previousMillis = AlPreviousMillis;
	Kp = alKp;
	Ki = alKi;
	Kd = alKd;
	for (int i = 0; i < 3; i++) error[i] = alError[i];
	N = alN;
	tau = alTau;
	d0 = alD0;
    d1 = alD1;
	fd0 = alFd0;
	fd1 = alFd1;
  }
  if (MotorDriverflag) {
    if (currentDir != direction) {
      // Need to stop motor before having it reverse direction
      for (int i = currentSpeed; i >= 0; i--) {
        newdelay(5);
        i2cMotorDriver.setDrive( motor, currentDir, i);
      }
      currentSpeed = 0;
    }
    // PID = See https://en.wikipedia.org/wiki/PID_controller
	// We do this each time this is called - usually in approx. one second intervals
	unsigned long currentMillis = millis();
    float deltaT = (currentMillis - previousMillis) / 1000.;

	// Recommended PID from Wikipedia
	float A0 = Kp + Ki*deltaT;
	float A1 = -Kp;
	float output = currentSpeed / 255.;  // Usually the current value of the actuator
	float A0d = Kd/deltaT;
	float A1d = - 2.0*Kd/deltaT;
	float A2d = Kd/deltaT;
	float alpha = deltaT / (2.*tau);

	// Loop section that recomputes the output value specifically for this time slot
    error[2] = error[1];
    error[1] = error[0];
    error[0] = ( targetPosition - currentPosition ) / maxRange;
    // Proportial/Integral part
    output = output + A0 * error[0] + A1 * error[1];
    // Filtered Derivative
    d1 = d0;
    d0 = A0d * error[0] + A1d * error[1] + A2d * error[2];
    fd1 = fd0;
    fd0 = ((alpha) / (alpha + 1)) * (d0 + d1) - ((alpha - 1) / (alpha + 1)) * fd1;
    output = output + fd0;
	
	// Use output value to determine speed to set
	speed = abs(output * 255.);
	if (speed > 255) speed = 255;
    i2cMotorDriver.setDrive( motor, direction, speed );
	
	if (motor == AZIMUTH_MOTOR) {
      currentAzMtrSpeed = speed;
      currentAzMtrDir = direction;
	  AzPreviousMillis = currentMillis;
	  for (int i = 0; i < 3; i++) azError[i] = error[i];
	  azD0 = d0;
	  azD1 = d1;
	  azFd0 = fd0;
	  azFd1 = fd1;
    }
    if (motor == ALTITUDE_MOTOR) {
      currentAlMtrSpeed = speed;
      currentAlMtrDir = direction;
	  AlPreviousMillis = currentMillis;
	  for (int i = 0; i < 3; i++) alError[i] = error[i];
	  alD0 = d0;
	  alD1 = d1;
	  alFd0 = fd0;
	  alFd1 = fd1;
    }
  }
  return true;
}

boolean driveMotor(int motor, int direction, int speed) {
  // We define here the routine to drive the Azimuth or Altitude motors.
  // So if any changes need to be made, it only has to be done here, not everywhere.
  // uses .setDrive( motorNum, direction, level ) to drive the motor
  int currentSpeed = 0;
  int currentDir = 0;
  if ( (motor != ALTITUDE_MOTOR) && (motor != AZIMUTH_MOTOR) ) return false;
  if ( (direction != CW_DIRECTION) && (direction != CCW_DIRECTION) ) return false;
  if ( (speed < 0) || (speed > 255) ) return false;
  if (motor == ALTITUDE_MOTOR) {
    currentSpeed = currentAlMtrSpeed;
    currentDir = currentAlMtrDir;
  }
  if (motor == AZIMUTH_MOTOR) {
    currentSpeed = currentAzMtrSpeed;
    currentDir = currentAzMtrDir;
  }
  if (MotorDriverflag) {
    //i2cMotorDriver.setDrive( motor, direction, speed );
    //Smoothly move one motor up to speed and back (drive level 0 to 255)
    if (currentDir != direction) {
      // Need to stop motor before having it reverse direction
      for (int i = currentSpeed; i >= 0; i--) {
        newdelay(5);
        i2cMotorDriver.setDrive( motor, currentDir, i);
      }
      currentSpeed = 0;
    }
    if (currentSpeed < speed) {
      for (int i = currentSpeed; i < speed; i++) {
        newdelay(5);
        i2cMotorDriver.setDrive( motor, direction, i); 
      }
    } else {
      for (int i = currentSpeed; i >= speed; i--) {
        newdelay(5);
        i2cMotorDriver.setDrive( motor, direction, i);
      }
    }
    if (motor == ALTITUDE_MOTOR) {
      currentAlMtrSpeed = speed;
      currentAlMtrDir = direction;
    }
    if (motor == AZIMUTH_MOTOR) {
      currentAzMtrSpeed = speed;
      currentAzMtrDir = direction;
    }
  }
  return true;
}
boolean driveMotorStop(int motor) {
  // We define here the routine to stop the Azimuth or Altitude motors.
  // So if any changes need to be made, it only has to be done here, not everywhere.
  int currentSpeed = 0;
  int currentDir = 0;
  if ( (motor != ALTITUDE_MOTOR) && (motor != AZIMUTH_MOTOR) ) return false;
  if (motor == ALTITUDE_MOTOR) {
    currentSpeed = currentAlMtrSpeed;
    currentDir = currentAlMtrDir;
  }
  if (motor == AZIMUTH_MOTOR) {
    currentSpeed = currentAzMtrSpeed;
    currentDir = currentAzMtrDir;
  }
  if (MotorDriverflag) {
    // uses .setDrive( motorNum, direction, level ) to stop the motor
    // i2cMotorDriver.setDrive( motor, 0, 0 );
    for (int i = currentSpeed; i >= 0; i--) {
      newdelay(5);
      i2cMotorDriver.setDrive( motor, currentDir, i);
    }
    if (motor == ALTITUDE_MOTOR) currentAlMtrSpeed = 0;
    if (motor == AZIMUTH_MOTOR) currentAzMtrSpeed = 0;
  }
  return true;
}
