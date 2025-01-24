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
Text Notes 2250 1300 0    79   ~ 16
Teensy 4.0 as Sound/USB/MIDI device
Text GLabel 4200 5500 3    50   Input ~ 0
T40_VIN
Text GLabel 3600 5500 3    50   Input ~ 0
I2S_FCLK
Text GLabel 3700 5500 3    50   Input ~ 0
I2S_BCLK
$Comp
L power:GND #PWR03
U 1 1 6636E8D3
P 4100 5500
F 0 "#PWR03" H 4100 5250 50  0001 C CNN
F 1 "GND" V 4100 5300 50  0000 C CNN
F 2 "" H 4100 5500 50  0001 C CNN
F 3 "" H 4100 5500 50  0001 C CNN
	1    4100 5500
	1    0    0    -1  
$EndComp
Text GLabel 4100 4050 1    50   Input ~ 0
T40_RX1
Text GLabel 4000 4050 1    50   Input ~ 0
T40_TX1
$Comp
L power:GND #PWR06
U 1 1 66372B69
P 4200 4050
F 0 "#PWR06" H 4200 3800 50  0001 C CNN
F 1 "GND" V 4200 3850 50  0000 C CNN
F 2 "" H 4200 4050 50  0001 C CNN
F 3 "" H 4200 4050 50  0001 C CNN
	1    4200 4050
	-1   0    0    1   
$EndComp
Text GLabel 4500 2950 3    50   Input ~ 0
T40_RX1
Text GLabel 4400 2950 3    50   Input ~ 0
T40_TX1
$Comp
L power:GND #PWR08
U 1 1 66466073
P 4600 2950
F 0 "#PWR08" H 4600 2700 50  0001 C CNN
F 1 "GND" V 4600 2750 50  0000 C CNN
F 2 "" H 4600 2950 50  0001 C CNN
F 3 "" H 4600 2950 50  0001 C CNN
	1    4600 2950
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J6
U 1 1 66460E1E
P 4500 2750
F 0 "J6" V 4372 2930 50  0001 L CNN
F 1 "T4 Serial1" V 4600 2550 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 4500 2750 50  0001 C CNN
F 3 "~" H 4500 2750 50  0001 C CNN
	1    4500 2750
	0    1    -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J5
U 1 1 6647A8E0
P 1950 6350
F 0 "J5" H 2030 6342 50  0001 L CNN
F 1 "Host USB" H 1650 6650 50  0000 L CNN
F 2 "0_my_footprints2:JST4" H 1950 6350 50  0001 C CNN
F 3 "~" H 1950 6350 50  0001 C CNN
	1    1950 6350
	0    -1   1    0   
$EndComp
Text GLabel 2200 4550 3    50   Input ~ 0
HOST_D-
Text GLabel 2100 4550 3    50   Input ~ 0
HOST_D+
Text GLabel 2050 6150 1    50   Input ~ 0
HOST_D-
Text GLabel 1950 6150 1    50   Input ~ 0
HOST_D+
Text GLabel 2150 6150 1    50   Input ~ 0
T40_VIN
$Comp
L power:GND #PWR07
U 1 1 6647D394
P 1850 6150
F 0 "#PWR07" H 1850 5900 50  0001 C CNN
F 1 "GND" V 1850 5950 50  0000 C CNN
F 2 "" H 1850 6150 50  0001 C CNN
F 3 "" H 1850 6150 50  0001 C CNN
	1    1850 6150
	-1   0    0    1   
$EndComp
Text GLabel 3500 4050 1    50   Input ~ 0
T40_I2S_TXB
$Comp
L Connector_Generic:Conn_01x03 J4
U 1 1 664185CC
P 2200 4350
F 0 "J4" V 2072 4530 50  0001 L CNN
F 1 "T4 bottom" H 1800 4000 50  0000 L CNN
F 2 "0_my_footprints2:pinHeader1x3" H 2200 4350 50  0001 C CNN
F 3 "~" H 2200 4350 50  0001 C CNN
	1    2200 4350
	0    1    -1   0   
$EndComp
Text GLabel 2300 4550 3    50   Input ~ 0
T40_I2S_RXB
Text GLabel 3100 5600 3    50   Input ~ 0
T40_RX3
Text GLabel 3000 5600 3    50   Input ~ 0
T40_TX3
$Comp
L Connector_Generic:Conn_01x05 J9
U 1 1 664488D3
P 2100 2650
F 0 "J9" V 1972 2362 50  0001 R CNN
F 1 "rPi I2S" V 2200 2800 50  0000 R CNN
F 2 "0_my_footprints2:JST5" H 2100 2650 50  0001 C CNN
F 3 "~" H 2100 2650 50  0001 C CNN
	1    2100 2650
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR011
U 1 1 6644DC7F
P 1900 2850
F 0 "#PWR011" H 1900 2600 50  0001 C CNN
F 1 "GND" V 1900 2650 50  0000 C CNN
F 2 "" H 1900 2850 50  0001 C CNN
F 3 "" H 1900 2850 50  0001 C CNN
	1    1900 2850
	-1   0    0    -1  
$EndComp
Text GLabel 2200 2850 3    50   Input ~ 0
I2S_FCLK
Text GLabel 2300 2850 3    50   Input ~ 0
I2S_BCLK
Text GLabel 2100 2850 3    50   Input ~ 0
T40_I2S_TXB
Text GLabel 2000 2850 3    50   Input ~ 0
T40_I2S_RXB
Text GLabel 3100 4050 1    50   Input ~ 10
AUDIO_SD_CS!
$Comp
L 0_my_teensy:teensyAudioRevD U1
U 1 1 664185D7
P 3650 4800
F 0 "U1" H 4528 5303 60  0001 L CNN
F 1 "teensyAudioRevD" H 3550 4800 60  0000 L CNN
F 2 "0_my_teensy:audioShieldRevD" V 4600 4750 60  0001 C CNN
F 3 "" V 4600 4750 60  0000 C CNN
	1    3650 4800
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J7
U 1 1 6643F411
P 3000 6650
F 0 "J7" V 2872 6830 50  0001 L CNN
F 1 "T4 Serial3" V 3100 6450 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 3000 6650 50  0001 C CNN
F 3 "~" H 3000 6650 50  0001 C CNN
	1    3000 6650
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR09
U 1 1 6643F40B
P 2900 6450
F 0 "#PWR09" H 2900 6200 50  0001 C CNN
F 1 "GND" V 2900 6250 50  0000 C CNN
F 2 "" H 2900 6450 50  0001 C CNN
F 3 "" H 2900 6450 50  0001 C CNN
	1    2900 6450
	1    0    0    1   
$EndComp
Text GLabel 3100 6450 1    50   Input ~ 0
T40_TX3
Text GLabel 3000 6450 1    50   Input ~ 0
T40_RX3
Text Notes 3900 2550 0    50   ~ 0
This is the "MIDI Serial Port" (Serial1)\nIt is connected to the TE3 (main) program\nand accepts Serial Midi CC's and values, \ndefined in libraries/sgt5000midi.h\nto communicate with the SGTL5000 in src/sgtl5000.cpp\n\n
Text Notes 2350 3750 1    63   ~ 0
GPIO21\nGPIO20\nGPIO19\nGPIO18
Text Notes 1500 3650 0    50   ~ 0
rPi PINS
Text Notes 1550 6750 0    50   ~ 0
It is definitely a bad idea \nto try to run a USB host\non this "USB Soundcard"
Text Notes 3400 6700 0    50   ~ 0
Likewise, it is probably a bad\nidea fot the TE_audio device to have\nany responsibility for talking\nto the Looper, but if it did,\nit would be through Serial3
$EndSCHEMATC
