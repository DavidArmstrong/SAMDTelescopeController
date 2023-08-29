# SAMDTelescopeController
Arduino SAMD51-based Telescope Controller 4.00.00

#### By David Armstrong, August 2023
 - See MIT LICENSE.md file

=============================<br>

This folder contains the actual Telescope Controller sketch, TC4_0.ino.  There are several other
files used with this file to create the final image that is transferred to the SAMD51 flash.

The system may be used just as Digital Setting circles with no motors. But that means manually
moving the 'scope around. Of course, motors can be added later.

DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.


It is assumed that the sketch under TC4_0diagnostics has already been run, and it has validated the
hardware interfaces and library installations needed.
That sketch has its own Readme file.  Its purpose was to prove that you have all the needed
libraries installed, and that the relevant hardware interfaces exist and can communicate with the
system. If one get a library error, the code gives a URL for where each
library can be downloaded.

After the TC4_0diagnostics has run, move on to this main sketch, TC4_0.

## Tailoring the Sketch For Your System

At the beginning of the file is a section to allow setting various options and known values.

Lines 22-23: Encoder count range per axis, if known.  If not, leave defined at 0L.
If you know the encoder count range in azimuth and altitude, these values can be hardcoded and always used.  Otherwise, the system will attempt to figure out on its own what the ranges are.

Lines 26-27: Uncomment if associated Encoder is changing counts backwards.

Lines 30-31: Uncomment if associated Motor is moving backwards.

Line 35: Default Time Zone, usually in the range -12.0 to +12.0. This is in hours offset from GMT.

Lines 37-38: Daylight saving usage - Auto figures out if DST is active or not in US locations only.

Lines 46, 49: Select Terminal display output, either Serial (USB) or Serial2 (XeeBee module)

Line 54: Select LCD output. Defaults to Serial1.

Other options can be set inside the sketch itself while it is running. Those options are set to default settings
defined in tcinit.ino.


## Sketch Output

On a Windows computer, one can use the terminal program Tera-Term to access the board via USB.
However, any VT-100 compatible terminal program should work with the sketch.
Do not use the default Arduino IDE Serial output window, as it does not handle VT-100 control codes.

The Reset button may need to be pressed after programming via the Arduino IDE to see the 
program output from the beginning.

Reference: 
https://learn.sparkfun.com/tutorials/terminal-basics/tera-term-windows

==========================<br>
## Software Behavior

The primary design focus has been to require as little input as possible from the user to set up. A telescope should be as easy as a toaster to use, and not require the training and agility of an astronaut.

1. Make it wireless.  Battery power is on the telescope itself, not external to it. So no visible wires.
2. No alignment star hunting, if at all possible.
3. No input of parameters, other than what is needed to specify the object to look at.

The system allows three options for user interfaces.  There can be some overlap. These are set in the file TC4_0.ino,
or internally in the sketch itself as a selectable option when it is run.

A. Use the USB interface to the processor board.  It's the default, but requires a USB cable. (Serial)
B. Use Xbee hardware to allow a wireless serial connection to the system. (Serial2)
C. Use an LCD display with IR remote for input. (Serial1 or I2C)

One has to choose either A or B.  Option C can be used by itself, but one of the
other methods has to initially at least be designated as well.

When the system first powers on, it detects what is connected, and adapts as it can accordingly.

To that end, there should be an Azimuth Reference Sensor provided on the telescope mount.  This can be either a switch, light sensor, or magnetic Hall-effect sensor.  Otherwise, the program will ask for manual help in initialization. An inclinometer can also be added to the rocker board to help level the telescope in azimuth, and the system will prompt for that when it is ready.  Note that placement of the Azimuth Reference sensor should be such so that it permits leveling adjustments to be done when the telescope is at that position.

The altitude range needs either sensors for horizon and zenith positions, or an inclinometer. Again, not providing either of these options results in the program prompting for user input to initialize the Altitude axis.

====================================<br>

