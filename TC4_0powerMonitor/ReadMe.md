# SAMDTelescopeController

## Arduino SAMD21-based Telescope Controller 4.00.00 Power Monitor Sketch

### TC4.0 Power Monitor Sketch Readme File

#### By David Armstrong, February 2026
 - See MIT LICENSE.md file

=============================<br>

This sketch is optional, but useful.  It provides real-time monitoring of
a 12 volt Gel cell battery capacity that could be used to power the telescope controller.
It uses an Adafruit INA219 voltage/current sensor, and an OLED display. It is
designed to use its' own SAMD21 MCU, so as to not add any additional load onto
the main telescope controller MCU.

There are no other parts, besides the SAMD21 MCU, the INA219 sensor and the OLED display for output.
Connections between the three parts are via QWIIC cabling, which can be daisy chained from one to the other device.

The output displayed is as follows:

V: Output voltage of the battery <br>
mA: Current usage in milliamps <br>
mAH: Power draw in milliamp-hours <br>
mW: Power used in milliwatts <br>
%: Percentage estimate of how much power is left to use <br>

If a 120 volt power supply is being used to provide power to the system, then the percent power left
makes no sense.  The other values being reported though are still of use to the observer.

Libraries used: <br>
Adafruit INA219  https://github.com/adafruit/Adafruit_INA219 <br>
Sparkfun Micro OLED Breakout  https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library <br>
