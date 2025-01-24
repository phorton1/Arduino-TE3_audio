# TE3_audio - the teensy 4.0 based USB Audio device

This repo, based on a teensy 4.0, essentially implements a USB Audio
device that plugs into a (router to the) iPad.

It takes a 1/4" mono Guitar Audio Input, split into stereo LINE_IN
inputs (as my guitar has an active preamp that delivers sufficient
output to drive line level inputs), sends that Guitar Audio via USB
Audio to the iPad where it is processed through effects (ToneStack)
and mixed with a software based synthesizer (SampleTank), and returned
via USB Audio, which is then forwarded via I2S for processing by the
rPi Looper, and returned, once again, via I2S for final output to a
pair of stereo LINE_OUT  connectors.

It uses the SGTL5000 teensy audioShield converting the Line Inputs
into I2S data which the teensy then sends out via USB Audio to
the iPad.  It receives the returned USB Audio from the iPad and
converts it back into I2S to then send it to the Looper.  It,
finally, receives the I2S from the Looper and gives to the SGTL5000
to convert into analog Line Outputs.
processing.

It communicates with the central TE3 device vis Serial MIDI. It can
send its debugging output either to the USB Serial Device (for limited
benchtop testing) or via the Serial port to the TE3 which then forwards
it to my laptop for display.

The SGTL5000 and a software Mixer can be configured, controlled, and
polled via Serial Midi.


## Initial Implementation Notes



## Please also see

[**TE3**](https://github.com/phorton1/Arduino-TE3)
The parent **Foot Pedal** and central repositor houses and interfaces
with this device.

[**TE3_common**](https://github.com/phorton1/Arduino-libraries/TE3_commo)
Functionality and definitions common to the **TE3** and **TE3_audio** programs.

The [**circle-Looper**](https://github.com/phorton1/circle-prh-apps-Looper)
repo contains the source code for the **rPi Looper** application that runs
on a Raspberry Pi that is interfaced to this device.



