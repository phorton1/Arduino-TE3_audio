# USBC PD Experiments

OK, so I have proven I can use a *USBC Decoy Board* to get 5-20V from
a hefty 65W USBC power supply, use 15V to power a *65W USBC Charger Module*
and connect that to the power input on a *Powered USBC Hub* and, at least
*power the iPad* from the Hub (even through a USB extension cable).

In separate tests I have proven that the given USBC Hub can transmit
and receive *USB Audio* and *TE and FTP Midi Data* to and from the
*iPad* (to/from AudioBus, SampleTank, and Tonestack).

So now I have disassembled the **USBC Hub**, and need to connect it to the
**Charger Module**.  I obviously don't want to use (the tested) 3 foot long
USBC cable, since these will be inches from each other in a potential
**Looper3** box.

I did not see any 3" or less USBC power deliver cables on eBay. The smallest
USBC cables I did see were 1ft, still too big for my liking.

## What I have

I have some *USBC breakout boards* that are intended, apparently, to convert
the USBC to USB3A using fixed resistors to deliver 5V at (?) 500-3000ma.

I don't have any male USBC plugs or breakout boards.  The only thing I do have
are several (now useless) USBC to Lightning cables.

- USBC inline power meter/tester
- USBC female (to USB3) breakout boards
- USBC decoy board modified with jumpers for voltages
- USBC (cut) to Lightning USBC cable.
- 30W USBC Power Supply Dongle
- 65W USBC Power Supply for laptops
- iPad (currently de-charge) as a test sink
- various USBC and USBC extension cables


## Test1 - Baseline 15V proof of concept

- USBC 30W PSU to
- Black USBC Extension to
- USBC Power Meter to
- Black USBC Cable to
- Decoy Board jumpered for 15V to
- Volt Meter

Result: 15V showing on USBC Meter and Voltmeter

**Conclusion:** essentially, I am trying to replace the black
USBC cable with a small cable made from ends cut from
USBC-Lightning or other junk USBC cables.


## Test2 - cut white USBC-Lignthing exploration

- Cut white Ankler USBC-Lightning Cable to
- Red USBC Breakout board


In this test I want to check continuity and resistances
from the ends of the cut cable to the USBC Breakout board.

The cable has the following ends (black and red are heavier guage):

Continuity Test

- aluminum foil shield
  appears to be plastic coated on outside
  when I got a good connection I could find nothing attached to it

- black == GND
- red == VBUS
- white == CC1/2 depending on orientation of plug
- yellow == D+
- green == D-

Resistance Test

The has "512" resistors from CC1 and CC2 to ground,
and I measured 5.1K from CCx to ground.

**Conclusion:** I am guessing the cable is
straight thru - that the 5 wires are directly
mapped, with no internal logic or resistors,
to the GND, VBUS, D-, D+, an CCx pins on the
USBC connector.  If so, it may be simple to
construct a cable from two ends.

I also have a lot of USBC->USBA cables.


## Test3 - cut old Plastic USBC-USBA cable

- Cut white old USBC-USBA Cable to
- Red USBC Breakout board

This one has a braided shield outside an aluminum shield,
a nylon twine inside, and no yellow.  I suspect that the
white is D+, and there is no CCx pin presented on this
cable


- shield - presents as the SS casing on the USBC connector
- black - connected to shield, SS casing, and ground pin
- red - present as 39 ohm to VUSB; I suspect the cable is munged
- green - presents as D+ on breakout
- white - presents as D- on breakout


**Conclusion:** This cable is mnged, and even if it wasn't
it would be uselss for me because it does not present CCx pin.



## Test4 - cut cream colored brand new braided USBC-USBA cable

- Cut cream colored USBC-USBA Cable to
- Red USBC Breakout board

Unshielded, only 4 wires (B,R,G,W)

**Conclusion:** probably same as Test3, there is no CCx pin
presented, so these USBC-USBA cables are useless to me.

I have a very limited supply of USBC cables.

Double checking Ebay.

I disassembled the USBC connector.  As suspected
there are GND, VUSB, D-, and D+ wires.  It looks
like some tiny reistors on CCx lines,

When facing BOTTOM UP (with the GND, VUSB, D-, and D+ pins on the bottom),
and the connector to the right (red board to left), there are two resistors
labelled R2 and R1.  The "inside pads" of those resistors appear to be connected
to CC1 and CC2 respectively. The resistors measure 30.2K.  The outsides of the sistors
are connected to VBUS (they are pullups), not pulldowns like I expected.

I believe I can (a) remove the resistors, solder the red and black from
the white lightning USBC cable to VBUS and GND, and connect the white
to the inside of either resistor (but probably not both) to effect a
power cable.


## Test4 - hand made 3 wire cable

So, using the deconstructed connector, and the cut USBC-Lightning
cable, I did as said, soldering the Red, Black, and White from
the cable to the VBUS, VGND, and soldering the white (via a
small additional wire) to the "inside" of the R1 resistor.

IT WORKED!

So now I know I can make, and don't need to buy the short
2" USBC-USBC cable to go from the 65W charger to the USBC hub!!

It's a SHAME however that you cannot buy generic USBC connectors.

They are either like the ones in the USBC-USBA converters, with
pre-soldered resistors and NO CC1 CC2 connectors, or they are
$8 a set for a female and male pair (when the others are like
10 for two bucks).   I think it is because the mfr of the "good"
ones only sells them by the 1000's and all the USBC-USBA ones are
now getting old and available as surplus.

For my project I will deconstruct some other cables, sigh.


