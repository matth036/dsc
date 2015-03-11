Digital Setting Circles project

This projects implements digital setting circles on Olimex STM32-E407 and 
STM32-H407 development boards.  Digital setting circles make the telescope
a push-to telescope, not a goto telescope.  There is no provison for a driving mechanism
and roughly the pointing functionality of Orion's IntelliScope is achieved.

* This is a C++11 project with a mix of C and C++ code.  You will need a capable toolchain.

* PJ Nautghter's implementation of the algorithms found in "Astronomical Algorithms" by Jean Meeus are used.
  See: http://www.naughter.com/aa.html

* Star positions are extracted from version 5 of the Bright Star Catalog as found here:
  ftp://cdsarc.u-strasbg.fr/cats/V/50/catalog.gz

* New General Catalog data is extracted from The Historically Corrected New General Catalogue (tm).
  See: http://www.ngcicproject.org/public_HCNGC/HCNGC.htm

* The project is intended to work on a standalone basis, not requiring an external computer.

* Presently only altazimuth mounts with quadrature encoders are supported.


Hardware Notes.

N.B. It is plainly cheaper to buy proprietary digital setting circles than
to gather and assemble these parts.  

This project was first implemented on:

Olimex STM32-E407 board.
Newhaven NHD-0420DZW-AY5 4x20 Character OLED display.
Grayhill 84LS-BB2-016-N 4x4 Keypad.

The project avoided touch screens and sought out parts that
will work in cold temperatures.  Visual observers serious about dark 
adapting thier eyes will proabably find the Newhaven OLED display too 
bright.  The project largely relies on the display having four lines of twenty characters.
The code for the OLED display is intended to also work with liquid crystal displays.
It is adapted from Arduino LCD code.

Also used, 

A generic 8xAAA battery holder with connector to power the board.
A 2032 coin cell battery in a holder with connectors for the real time clock.

Two 4000 count per revolution quadrature encoders and mounting hardware.
These were aquired with the amateur made telescope and probably originate 
from Jim's Mobile.

A 12.5 inch aperture Dobsonian telescope.  

A big clunky project box.  Household RJ11 jacks.  Telepone wires with
RJ11 plugs.  The crimping tool for RJ11 plugs.  
Headers for the STM32-E407 board.  Standoff hardware for mounting the 
board.  Lots of wires and solder.

Olimex ARM-USB-OCD-H JTAG In-Circuit Debugger.  Slightly off standard
usb cable for the same.

This is a difficult project in a very primative state.  You have been 
warned.

Dan Matthews
matth036@yahoo.com










