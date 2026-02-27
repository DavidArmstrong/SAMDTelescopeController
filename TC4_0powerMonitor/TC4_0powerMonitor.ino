/* Arduino SAMD-based Telescope Controller 4.00.00 Optional Power Monitor - February 2026
// See MIT LICENSE.md file and ReadMe.md file for essential information
// Uses Sparkfun Micro OLED breakout, and Adafruit INA219 breakout
// to monitor Power Use when a 12 volt battery is used to provide system power.
*/
// I2C OLED display (IIC address 0x3c)
// I2C Voltage/Current sensor (IIC address 0x40)

// Account for Weirdness with SerialUSB
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#include <Arduino.h>
#include "pins_arduino.h"
#include <Wire.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <SFE_MicroOLED.h>  // https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library
#include <Adafruit_INA219.h> // https://github.com/adafruit/Adafruit_INA219

//The OLED library assumes a reset pin is necessary.
//The Qwiic OLED has RST hard-wired, so pick an IO pin that is not being used */
const int PIN_RESET = 40; //This is for the Neo-Pixel LED, which isn't used
/* This OLED definition needed - The DC_JUMPER is the I2C Address Select jumper.
// Set to 1 if the jumper is open (Default), or set to 0 if it's closed. */
#define DC_JUMPER 1
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration
Adafruit_INA219 ina219; // Bus voltage monitor
// Keep track of total time and milliamp measurements for milliamp-hour computation.
uint32_t total_sec = 0;
float total_mA = 0.0;
float maxVoltage = 12.6; // Fully charged voltage
float minVoltage = 10.2; // Completely discharged voltage

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

uint8_t counter = 0;
void powerupdate() {
  if (counter == 0) {
    update_power_display();
  } else {
    counter = (counter+1) % 10 ;
  }
}

void setup() {
  // Try to initialize the INA219
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  // By default the INA219 will be calibrated with a range of 32V, 2A.
  // However uncomment one of the below to change the range.  A smaller
  // range can't measure as large of values but will measure with slightly
  // better precision.
  ina219.setCalibration_16V_400mA();
  //ina219.setCalibration_32V_1A();

  newdelay(500);
	oled.begin();
  oled.clear(ALL);
  oled.display();
	newdelay(500);
  oled.clear(PAGE);
	oled.setFontType(0);
  oled.setCursor(0, 0);
  oled.print("  OLED    ");
	oled.print(" Battery  ");
  oled.print("  Power   ");
  oled.print(" Monitor  ");
	oled.print("TC 4.00.00");
	oled.display();
  newdelay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  powerupdate();
  newdelay(5000);
}

void update_power_display() {
  // Read voltage and current from INA219.
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();

  // Compute load voltage, power, and milliamp-hours.
  float loadvoltage = busvoltage + (shuntvoltage / 1000);
  float power_mW = loadvoltage * current_mA;
  (void)power_mW;

  total_mA += current_mA;
  total_sec += 1;
  float total_mAH = total_mA / 3600.0;
  (void)total_mAH;
  
  // Calculate percentage (clamped between 0-100)
  float batteryPercentage = (loadvoltage - minVoltage) / (maxVoltage - minVoltage) * 100;
  if (batteryPercentage > 100) batteryPercentage = 100;
  else if (batteryPercentage < 0) batteryPercentage = 0;

  // Update display.
  oled.clear(PAGE);
  oled.setCursor(0,0);

  // Mode 0, display volts and amps.
  printSIValue(loadvoltage, "V:", 2, 10);
  oled.setCursor(0, 9);
  printSIValue(current_mA/1000.0, "A:", 5, 10);
  oled.setCursor(0, 18);
  printSIValue(total_mAH/1000.0, "AH:", 5, 10);
  oled.setCursor(0, 27);
  printSIValue(power_mW/1000.0, "W:", 5, 10);
  oled.setCursor(0, 36);
  printSIValue(batteryPercentage, "%:", 2, 10);
  oled.display();
}

void printSIValue(float value, const char* units, int precision, int maxWidth) {
  // Print a value in SI units with the units left justified and value right justified.
  // Will switch to milli prefix if value is below 1.

  // Add milli prefix if low value.
  if (fabs(value) < 1.0) {
    oled.print('m');
    maxWidth -= 1;
    value *= 1000.0;
    precision = max(0, precision-3);
  }

  // Print units.
  oled.print(units);
  maxWidth -= strlen(units);

  // Leave room for negative sign if value is negative.
  if (value < 0.0) {
    maxWidth -= 1;
  }

  // Find how many digits are in value.
  int digits = ceil(log10(fabs(value)));
  if (fabs(value) < 1.0) {
    digits = 1; // Leave room for 0 when value is below 0.
  }

  // Handle if not enough width to display value, just print dashes.
  if (digits > maxWidth) {
    // Fill width with dashes (and add extra dash for negative values);
    for (int i=0; i < maxWidth; ++i) {
      oled.print('-');
    }
    if (value < 0.0) {
      oled.print('-');
    }
    return;
  }

  // Compute actual precision for printed value based on space left after
  // printing digits and decimal point.  Clamp within 0 to desired precision.
  int actualPrecision = constrain(maxWidth-digits-1, 0, precision);

  // Compute how much padding to add to right justify.
  int padding = maxWidth-digits-1-actualPrecision;
  for (int i=0; i < padding; ++i) {
    oled.print(' ');
  }

  // Finally, print the value
  oled.print(value, actualPrecision);
}
