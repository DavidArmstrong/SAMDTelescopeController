# SAMDTelescopeController
Arduino SAMD-based Telescope Controller 4.00.00
By David Armstrong, July 2022
See MIT LICENSE.md file
=================================================================

This repository implements one of many possible design scenarios for a Telescope Controller, using the
Arduino IDE as a development environment.  It's purpose is to allow me, personally,
to use my telescope to look at stuff, instead of spending a lot of time setting it up.
(It can be used just as Digital Setting circles with no motors, or motors can be added later.)

I'm making this available for others to use as a reference in designing their own systems.  As
such, feel free to clone it and change as your needs dictate.

The files in the "Arduino Telescope Interface" directory show an Eagle schematic of how to connect various interfaces.  There is also a drawio based diagram showing how all the parts communicate with each other.

Use the TC4_0diagnostics.ino sketch to test the installed libraries and hardware interfaces
before going to the TC4_0.ino sketch.

Note: DO NOT ATTEMPT TO LOAD THIS CODE ONTO A STANDARD Arduino UNO BOARD.


This is still very much a work in progress.  I'm putting the code here to make it easier to manage, and to avoid a possible future catastrophe.  (Murphy knows me.)

So, not ready for Prime Time, and not ready for a real Telescope either yet. (But getting close.)
