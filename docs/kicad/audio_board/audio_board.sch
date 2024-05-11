EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A2 23386 16535
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 1800 2200 0    50   ~ 0
GPIO25 RPI_READY conflicts with Blue rPi 3.5" display RESET
Text Notes 1800 2300 0    50   ~ 0
GPIO23 OCTO_MULT2 conflics with Blue rPi 2.5" display CD pin
$Comp
L Connector_Generic:Conn_01x08 J1
U 1 1 666229FF
P 3100 2900
F 0 "J1" V 3100 3450 50  0001 R CNN
F 1 "RPI_ILI9488" V 3250 3050 50  0000 R CNN
F 2 "0_my_footprints:myJSTx08" H 3100 2900 50  0001 C CNN
F 3 "~" H 3100 2900 50  0001 C CNN
	1    3100 2900
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR01
U 1 1 666282FE
P 3500 3100
F 0 "#PWR01" H 3500 2850 50  0001 C CNN
F 1 "GND" V 3500 2900 50  0000 C CNN
F 2 "" H 3500 3100 50  0001 C CNN
F 3 "" H 3500 3100 50  0001 C CNN
	1    3500 3100
	1    0    0    -1  
$EndComp
Text GLabel 6850 9550 0    50   Input ~ 0
RPI_5V
Text GLabel 6950 9550 2    50   Input ~ 0
T40_VIN
Text Notes 1650 1200 0    79   ~ 16
rPi Connector
Text Notes 5800 1050 0    79   ~ 16
Teensy 4.0 as Sound/USB/MIDI device
Text Notes 5800 1200 0    50   ~ 10
Is the USB Audio Device in the system.
Text GLabel 2900 3100 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 3000 3100 3    50   Input ~ 0
RPI_LED_CS
$Comp
L power:GND #PWR06
U 1 1 66611616
P 2600 6650
F 0 "#PWR06" H 2600 6400 50  0001 C CNN
F 1 "GND" V 2600 6450 50  0000 C CNN
F 2 "" H 2600 6650 50  0001 C CNN
F 3 "" H 2600 6650 50  0001 C CNN
	1    2600 6650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 666110E8
P 2800 6650
F 0 "#PWR07" H 2800 6400 50  0001 C CNN
F 1 "GND" V 2800 6450 50  0000 C CNN
F 2 "" H 2800 6650 50  0001 C CNN
F 3 "" H 2800 6650 50  0001 C CNN
	1    2800 6650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 666109AB
P 3300 6650
F 0 "#PWR08" H 3300 6400 50  0001 C CNN
F 1 "GND" V 3300 6450 50  0000 C CNN
F 2 "" H 3300 6650 50  0001 C CNN
F 3 "" H 3300 6650 50  0001 C CNN
	1    3300 6650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 66610187
P 3600 6650
F 0 "#PWR09" H 3600 6400 50  0001 C CNN
F 1 "GND" V 3600 6450 50  0000 C CNN
F 2 "" H 3600 6650 50  0001 C CNN
F 3 "" H 3600 6650 50  0001 C CNN
	1    3600 6650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 6660F832
P 4000 6650
F 0 "#PWR010" H 4000 6400 50  0001 C CNN
F 1 "GND" V 4000 6450 50  0000 C CNN
F 2 "" H 4000 6650 50  0001 C CNN
F 3 "" H 4000 6650 50  0001 C CNN
	1    4000 6650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 6660EE40
P 2300 5300
F 0 "#PWR03" H 2300 5050 50  0001 C CNN
F 1 "GND" V 2300 5100 50  0000 C CNN
F 2 "" H 2300 5300 50  0001 C CNN
F 3 "" H 2300 5300 50  0001 C CNN
	1    2300 5300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 6660E9EA
P 3000 5300
F 0 "#PWR04" H 3000 5050 50  0001 C CNN
F 1 "GND" V 3000 5100 50  0000 C CNN
F 2 "" H 3000 5300 50  0001 C CNN
F 3 "" H 3000 5300 50  0001 C CNN
	1    3000 5300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR05
U 1 1 6660E5AB
P 3800 5300
F 0 "#PWR05" H 3800 5050 50  0001 C CNN
F 1 "GND" V 3800 5100 50  0000 C CNN
F 2 "" H 3800 5300 50  0001 C CNN
F 3 "" H 3800 5300 50  0001 C CNN
	1    3800 5300
	-1   0    0    1   
$EndComp
Text GLabel 4200 5300 1    50   Input ~ 0
RPI_3.3V
Text GLabel 4200 6650 3    50   Input ~ 0
RPI_5V
Text GLabel 4000 5300 1    50   Input ~ 10
OCTO_SCL
Text GLabel 4100 5300 1    50   Input ~ 10
OCTO_SDA
Text GLabel 3700 5300 1    50   Input ~ 10
UNUSED_DISPLAY_TOUCH_IRQ
Text GLabel 2700 6650 3    50   Input ~ 0
RPI_I2S_RX_ACTIVE
Text GLabel 2500 6650 3    50   Input ~ 0
RPI_I2S_TX_ACTIVE
Text GLabel 3200 6650 3    50   Input ~ 0
RPI_READY
Text GLabel 3400 6650 3    50   Input ~ 10
OCTO_MULT3
Text GLabel 3500 6650 3    50   Input ~ 10
OCTO_MULT2
Text GLabel 3600 5300 1    50   Input ~ 10
OCTO_MULT0
Text GLabel 3500 5300 1    50   Input ~ 10
OCTO_MULT1
Text GLabel 2800 5300 1    50   Input ~ 10
OCTO_RESET
Text GLabel 3000 6650 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 3100 6650 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 3900 5300 1    50   Input ~ 0
RPI_CD
Text GLabel 3100 5300 1    50   Input ~ 0
RPI_SCLK
Text GLabel 3200 5300 1    50   Input ~ 0
RPI_MISO
Text GLabel 3300 5300 1    50   Input ~ 0
RPI_MOSI
Text GLabel 3900 6650 3    50   Input ~ 0
RPI_TX
Text GLabel 3800 6650 3    50   Input ~ 0
RPI_RX
Text GLabel 2300 6650 3    50   Input ~ 0
I2S_RX1B
Text GLabel 2400 6650 3    50   Input ~ 0
I2S_TX1B
Text GLabel 2500 5300 1    50   Input ~ 0
LRCLK1
Text GLabel 3700 6650 3    50   Input ~ 0
BCLK1
$Comp
L 0_my_symbols:rpi_connector J2
U 1 1 6636D4F6
P 3250 5950
F 0 "J2" H 4278 5971 50  0001 L CNN
F 1 "rpi_connector" H 2950 5950 50  0000 L CNN
F 2 "0_my_footprints:myRpiConnector_bottom" H 3150 5850 50  0001 C CNN
F 3 "" H 3150 5850 50  0001 C CNN
	1    3250 5950
	1    0    0    -1  
$EndComp
Text Notes 1650 1800 0    50   ~ 10
ILI9488 connector pins may be (are) different for the rPi and T41,\nrequiring different cables, for ease of PCB layout.
Text Notes 2250 9800 0    50   ~ 0
The rPI run pin is handled differently based on the rPi model used.\nOn original rPi Zero, 3B+, and 4Bs, it is a pin in slightly different locations.\nOn the rPi Zero 2W it is a pad on the bottom that must be brought to \nthe connector board via a cable. 
Text Notes 5800 9100 0    50   ~ 10
A jumper will exist to optionally power the teensy 4.0 from the RPI
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 66582C3E
P 6850 9350
F 0 "J3" H 6930 9342 50  0001 L CNN
F 1 "T40 Power" V 6950 9500 50  0000 R CNN
F 2 "0_my_footprints:myPinHeader_1x02" H 6850 9350 50  0001 C CNN
F 3 "~" H 6850 9350 50  0001 C CNN
	1    6850 9350
	0    -1   -1   0   
$EndComp
$Comp
L 0_my_teensy:myTeensy4.0_mod U1
U 1 1 6634F8C6
P 6800 5750
F 0 "U1" H 6122 5753 60  0000 R CNN
F 1 "myTeensy4.0_mod" H 6122 5647 60  0000 R CNN
F 2 "0_my_teensy:myTeensy40_mod" V 7700 5800 60  0001 C CNN
F 3 "" V 7700 5800 60  0000 C CNN
	1    6800 5750
	1    0    0    -1  
$EndComp
Text GLabel 6250 4750 1    50   Input ~ 0
T40_VIN
Text GLabel 6850 4750 1    50   Input ~ 0
LRCLK1
Text GLabel 6750 4750 1    50   Input ~ 0
BCLK1
$Comp
L power:GND #PWR02
U 1 1 6636E8D3
P 6350 4750
F 0 "#PWR02" H 6350 4500 50  0001 C CNN
F 1 "GND" V 6350 4550 50  0000 C CNN
F 2 "" H 6350 4750 50  0001 C CNN
F 3 "" H 6350 4750 50  0001 C CNN
	1    6350 4750
	-1   0    0    1   
$EndComp
Text GLabel 6350 6850 3    50   Input ~ 0
T40_RX1
Text GLabel 6450 6850 3    50   Input ~ 0
T40_TX1
Text GLabel 6950 6850 3    50   Input ~ 0
I2S_RX1B
$Comp
L power:GND #PWR011
U 1 1 66372B69
P 6250 6850
F 0 "#PWR011" H 6250 6600 50  0001 C CNN
F 1 "GND" V 6250 6650 50  0000 C CNN
F 2 "" H 6250 6850 50  0001 C CNN
F 3 "" H 6250 6850 50  0001 C CNN
	1    6250 6850
	1    0    0    -1  
$EndComp
Text GLabel 7800 5950 2    50   Input ~ 0
I2S_TX1B
Text Notes 5800 1300 0    50   ~ 10
Is the USB MIDI Devices in the system.
Text Notes 5800 2300 0    50   ~ 0
Handles USB Host to and from USB MIDI as fast as possible.
Text Notes 5800 2550 0    50   ~ 0
Accepts Serial MIDI input for all relevant SGTL5000 control functions
Text Notes 5800 1400 0    50   ~ 10
Uses SGTL5000 (Audio Shield) as a hat
Text Notes 5800 1550 0    50   ~ 0
Uses teensyQuad channels 1&2 to talk to SGTL5000
Text Notes 5800 1650 0    50   ~ 0
Uses teensyQuad channels 3&4 to talk to rPI (Looper)
Text Notes 5800 1750 0    50   ~ 0
Receives LINE_IN to SGTL500 on teensyQuad input channels 1&2
Text Notes 5800 1850 0    50   ~ 0
Receives USB Audio Out from iPad (Guitar Effects and Synth)
Text Notes 5800 1950 0    50   ~ 0
Sends USB Audio Out to rPi (Looper) on teensyQuad Out channels 3&4
Text Notes 5800 2050 0    50   ~ 0
Receives rPi (Looper) processed audio on teensyQuad channels 3&4
Text Notes 5800 2150 0    50   ~ 0
Sends final proccessed rPi (Looper) audio to SGTL5000 on teensyQuad channels 1&2
Text Notes 5800 2450 0    50   ~ 0
Sends and Recives Serial MIDI over Serial1 port
Text Notes 5800 2650 0    50   ~ 0
Intercepts and acts on Serial MIDI for SGTL5000 functions
Text Notes 5800 2850 0    50   ~ 0
Bidirectional forwarding of all other Serial MIDI to USB Midi device
Text Notes 5800 2750 0    50   ~ 0
Sends Serial MIDI for any SGTL5000 control state changes
Text Notes 5800 3000 0    50   ~ 0
Sends debugging text to Serial1 port
Text Notes 5800 3100 0    50   ~ 0
Can optionally send debugging text to USB Serial port
Text Notes 5800 3200 0    50   ~ 0
May possibly accept Serial text debugging and control input
Text Notes 5800 3450 0    50   ~ 0
Note that the (my) Serial MIDI protocol can be interspersed with text\nand unambiguously decoded
Text Notes 5800 3600 0    50   ~ 10
Has no direct Serial communications with rPi (Looper)
Text Notes 1800 2100 0    50   ~ 0
BOLD pins are reserved for future potential use
Text Notes 1650 2000 0    50   ~ 10
Notes for alternate/future use:
Text Notes 1650 1350 0    50   ~ 0
Connects rPi (Looper) I2S to teensyQuad channels 3&4
Text Notes 1650 1450 0    50   ~ 0
Provides 8 pin connector to SPI (ILI9488) TFT Touch Screen
Text Notes 1650 1550 0    50   ~ 0
Provides 8 pin connector to controlling teensy (previously 3.2, now 4.1)
$Comp
L Connector_Generic:Conn_01x08 J4
U 1 1 663E0DFB
P 3250 8850
F 0 "J4" V 3250 9400 50  0001 R CNN
F 1 "teensy Controller" V 3400 9100 50  0000 R CNN
F 2 "0_my_footprints:myJSTx08" H 3250 8850 50  0001 C CNN
F 3 "~" H 3250 8850 50  0001 C CNN
	1    3250 8850
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR012
U 1 1 663E0E01
P 3550 8650
F 0 "#PWR012" H 3550 8400 50  0001 C CNN
F 1 "GND" V 3550 8450 50  0000 C CNN
F 2 "" H 3550 8650 50  0001 C CNN
F 3 "" H 3550 8650 50  0001 C CNN
	1    3550 8650
	-1   0    0    1   
$EndComp
Text GLabel 3650 8650 1    50   Input ~ 0
RPI_5V
Text GLabel 3450 8650 1    50   Input ~ 0
RPI_TX
Text GLabel 3350 8650 1    50   Input ~ 0
RPI_RX
Text GLabel 3250 8650 1    50   Input ~ 0
RPI_READY
Text GLabel 3050 8650 1    50   Input ~ 0
RPI_I2S_RX_ACTIVE
Text GLabel 2950 8650 1    50   Input ~ 0
RPI_I2S_TX_ACTIVE
Text GLabel 3150 8650 1    50   Input ~ 0
RPI_RUN
Text Notes 5800 3950 0    50   ~ 0
If powered from the rPi we may need another input pin\nto know that the rPI has started and that it is ready to do\nI2S communications
Text GLabel 4100 6650 3    50   Input ~ 0
RPI_5V
Text GLabel 3100 3100 3    50   Input ~ 0
RPI_CD
Text GLabel 2800 3100 3    50   Input ~ 0
RPI_5V
Text GLabel 3200 3100 3    50   Input ~ 0
RPI_MOSI
Text GLabel 3300 3100 3    50   Input ~ 0
RPI_MISO
Text GLabel 3400 3100 3    50   Input ~ 0
RPI_SCLK
Text GLabel 6350 8100 1    50   Input ~ 0
T40_RX1
Text GLabel 6450 8100 1    50   Input ~ 0
T40_TX1
$Comp
L power:GND #PWR013
U 1 1 66466073
P 6250 8100
F 0 "#PWR013" H 6250 7850 50  0001 C CNN
F 1 "GND" V 6250 7900 50  0000 C CNN
F 2 "" H 6250 8100 50  0001 C CNN
F 3 "" H 6250 8100 50  0001 C CNN
	1    6250 8100
	1    0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J6
U 1 1 66460E1E
P 6350 8300
F 0 "J6" V 6222 8480 50  0001 L CNN
F 1 "T4 Serial" V 6450 8100 50  0000 L CNN
F 2 "0_my_footprints:myJSTx03" H 6350 8300 50  0001 C CNN
F 3 "~" H 6350 8300 50  0001 C CNN
	1    6350 8300
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 66476E5C
P 7800 5750
F 0 "#PWR0101" H 7800 5500 50  0001 C CNN
F 1 "GND" V 7800 5550 50  0000 C CNN
F 2 "" H 7800 5750 50  0001 C CNN
F 3 "" H 7800 5750 50  0001 C CNN
	1    7800 5750
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J7
U 1 1 6647A8E0
P 9550 5750
F 0 "J7" H 9630 5742 50  0001 L CNN
F 1 "Host USB" H 9250 6050 50  0000 L CNN
F 2 "0_my_footprints:myJSTx04" H 9550 5750 50  0001 C CNN
F 3 "~" H 9550 5750 50  0001 C CNN
	1    9550 5750
	1    0    0    1   
$EndComp
Text GLabel 7800 5650 2    50   Input ~ 0
HOST_D-
Text GLabel 7800 5850 2    50   Input ~ 0
HOST_D+
Text GLabel 9350 5650 0    50   Input ~ 0
HOST_D-
Text GLabel 9350 5750 0    50   Input ~ 0
HOST_D+
Text GLabel 9350 5550 0    50   Input ~ 0
T40_VIN
$Comp
L power:GND #PWR0102
U 1 1 6647D394
P 9350 5850
F 0 "#PWR0102" H 9350 5600 50  0001 C CNN
F 1 "GND" V 9350 5650 50  0000 C CNN
F 2 "" H 9350 5850 50  0001 C CNN
F 3 "" H 9350 5850 50  0001 C CNN
	1    9350 5850
	0    1    1    0   
$EndComp
$EndSCHEMATC
