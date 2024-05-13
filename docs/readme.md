# TE3 - teensyExpression3

This is the third, and hopefully last, major iteration of
the teensyExpression/vGuitar rig design and implementation.

Please see the
[vGuitar System](https://github.com/phorton1/phorton1/blob/master/vGuitar/vGuitar.md)
for information regardind the system as a whole.

This repository contains the source code for the portion
of the system that runs on a *teensy4.1* that services
the *Foot Pedal*, handling the buttons, knobs, expression pedals,
and LEDs for the buttons, and providing the UI on a TFT (LCD) display.

It also contains all of the **3D Printing** Fusion 360 designs
and STL file needed print the Foot Pedal box, as well as all of
the kicad **schematics** and **PCB** (printed circuit board)
designs for the PCB's in the system.


## Please also see

[**TE3_hub**](https://github.com/phorton1/Arduino-TE3_hub)
contains the code for the **USB Audio and Midi** device with
a **USB Host** that runs on *teensy 4.0*. It *may or may not*
contain a separate *PCB* design for that portion of the device.

The [**circle-Looper**](https://github.com/phorton1/circle-prh-apps-Looper)
repo contains the source code for the *Looper application* that runs
on a Raspberry Pi.



