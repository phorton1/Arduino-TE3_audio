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
Text Notes 4700 1900 0    79   ~ 16
Teensy 4.0 as Sound/USB/MIDI device
Text GLabel 8900 8700 3    50   Input ~ 0
T40_VIN
Text GLabel 8300 8700 3    50   Input ~ 0
I2S_FCLK
Text GLabel 8400 8700 3    50   Input ~ 0
I2S_BCLK
$Comp
L power:GND #PWR03
U 1 1 6636E8D3
P 8800 8700
F 0 "#PWR03" H 8800 8450 50  0001 C CNN
F 1 "GND" V 8800 8500 50  0000 C CNN
F 2 "" H 8800 8700 50  0001 C CNN
F 3 "" H 8800 8700 50  0001 C CNN
	1    8800 8700
	1    0    0    -1  
$EndComp
Text GLabel 8800 7250 1    50   Input ~ 0
T40_RX1
Text GLabel 8700 7250 1    50   Input ~ 0
T40_TX1
$Comp
L power:GND #PWR06
U 1 1 66372B69
P 8900 7250
F 0 "#PWR06" H 8900 7000 50  0001 C CNN
F 1 "GND" V 8900 7050 50  0000 C CNN
F 2 "" H 8900 7250 50  0001 C CNN
F 3 "" H 8900 7250 50  0001 C CNN
	1    8900 7250
	-1   0    0    1   
$EndComp
Text GLabel 8800 5800 3    50   Input ~ 0
T40_RX1
Text GLabel 8700 5800 3    50   Input ~ 0
T40_TX1
$Comp
L power:GND #PWR08
U 1 1 66466073
P 8900 5800
F 0 "#PWR08" H 8900 5550 50  0001 C CNN
F 1 "GND" V 8900 5600 50  0000 C CNN
F 2 "" H 8900 5800 50  0001 C CNN
F 3 "" H 8900 5800 50  0001 C CNN
	1    8900 5800
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J6
U 1 1 66460E1E
P 8800 5600
F 0 "J6" V 8672 5780 50  0001 L CNN
F 1 "T4 Serial1" V 8900 5400 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 8800 5600 50  0001 C CNN
F 3 "~" H 8800 5600 50  0001 C CNN
	1    8800 5600
	0    1    -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J5
U 1 1 6647A8E0
P 6350 10100
F 0 "J5" H 6430 10092 50  0001 L CNN
F 1 "Host USB" H 6050 10400 50  0000 L CNN
F 2 "0_my_footprints2:JST4" H 6350 10100 50  0001 C CNN
F 3 "~" H 6350 10100 50  0001 C CNN
	1    6350 10100
	0    -1   1    0   
$EndComp
Text GLabel 6550 8100 3    50   Input ~ 0
HOST_D-
Text GLabel 6450 8100 3    50   Input ~ 0
HOST_D+
Text GLabel 6450 9900 1    50   Input ~ 0
HOST_D-
Text GLabel 6350 9900 1    50   Input ~ 0
HOST_D+
Text GLabel 6550 9900 1    50   Input ~ 0
T40_VIN
$Comp
L power:GND #PWR07
U 1 1 6647D394
P 6250 9900
F 0 "#PWR07" H 6250 9650 50  0001 C CNN
F 1 "GND" V 6250 9700 50  0000 C CNN
F 2 "" H 6250 9900 50  0001 C CNN
F 3 "" H 6250 9900 50  0001 C CNN
	1    6250 9900
	-1   0    0    1   
$EndComp
Text GLabel 8200 7250 1    50   Input ~ 0
T40_I2S_TXB
$Comp
L Connector_Generic:Conn_01x03 J4
U 1 1 664185CC
P 6550 7900
F 0 "J4" V 6422 8080 50  0001 L CNN
F 1 "T4 bottom" H 6150 7550 50  0000 L CNN
F 2 "0_my_footprints2:pinHeader1x3" H 6550 7900 50  0001 C CNN
F 3 "~" H 6550 7900 50  0001 C CNN
	1    6550 7900
	0    1    -1   0   
$EndComp
Text GLabel 6650 8100 3    50   Input ~ 0
T40_I2S_RXB
Text GLabel 7800 8700 3    50   Input ~ 0
T40_RX3
Text GLabel 7700 8700 3    50   Input ~ 0
T40_TX3
$Comp
L Connector_Generic:Conn_01x05 J9
U 1 1 664488D3
P 6750 5500
F 0 "J9" V 6622 5212 50  0001 R CNN
F 1 "rPi I2S" V 6850 5650 50  0000 R CNN
F 2 "0_my_footprints2:JST5" H 6750 5500 50  0001 C CNN
F 3 "~" H 6750 5500 50  0001 C CNN
	1    6750 5500
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR011
U 1 1 6644DC7F
P 6550 5700
F 0 "#PWR011" H 6550 5450 50  0001 C CNN
F 1 "GND" V 6550 5500 50  0000 C CNN
F 2 "" H 6550 5700 50  0001 C CNN
F 3 "" H 6550 5700 50  0001 C CNN
	1    6550 5700
	-1   0    0    -1  
$EndComp
Text GLabel 6850 5700 3    50   Input ~ 0
I2S_FCLK
Text GLabel 6950 5700 3    50   Input ~ 0
I2S_BCLK
Text GLabel 6750 5700 3    50   Input ~ 0
T40_I2S_TXB
Text GLabel 6650 5700 3    50   Input ~ 0
T40_I2S_RXB
Text GLabel 7800 7250 1    50   Input ~ 10
AUDIO_SD_CS!
$Comp
L 0_my_teensy:teensyAudioRevD U1
U 1 1 664185D7
P 8350 8000
F 0 "U1" H 9228 8503 60  0001 L CNN
F 1 "teensyAudioRevD" H 8250 8000 60  0000 L CNN
F 2 "0_my_teensy:audioShieldRevD" V 9300 7950 60  0001 C CNN
F 3 "" V 9300 7950 60  0000 C CNN
	1    8350 8000
	-1   0    0    1   
$EndComp
Wire Wire Line
	6450 8650 6450 8950
Wire Wire Line
	6550 8650 6550 8950
Wire Wire Line
	6550 10300 6550 10650
Wire Wire Line
	6250 10300 6250 10900
Wire Bus Line
	6000 10650 9600 10650
Wire Bus Line
	6000 10900 9600 10900
Wire Wire Line
	6650 7400 6650 7050
Wire Bus Line
	6300 5000 9900 5000
Wire Wire Line
	8950 10250 8950 10650
Wire Wire Line
	8850 10250 8850 10900
Wire Wire Line
	6950 8950 6950 8650
Wire Wire Line
	6950 7400 6950 7050
Wire Wire Line
	6850 8950 6850 8650
Wire Wire Line
	6850 7400 6850 7050
Wire Wire Line
	6750 6400 8200 6400
$Comp
L Connector_Generic:Conn_01x03 J7
U 1 1 6643F411
P 7700 10300
F 0 "J7" V 7572 10480 50  0001 L CNN
F 1 "T4 Serial3" V 7800 10100 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 7700 10300 50  0001 C CNN
F 3 "~" H 7700 10300 50  0001 C CNN
	1    7700 10300
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR09
U 1 1 6643F40B
P 7600 10100
F 0 "#PWR09" H 7600 9850 50  0001 C CNN
F 1 "GND" V 7600 9900 50  0000 C CNN
F 2 "" H 7600 10100 50  0001 C CNN
F 3 "" H 7600 10100 50  0001 C CNN
	1    7600 10100
	1    0    0    1   
$EndComp
Text GLabel 7800 10100 1    50   Input ~ 0
T40_TX3
Text GLabel 7700 10100 1    50   Input ~ 0
T40_RX3
Wire Wire Line
	8400 9350 6950 9350
Wire Wire Line
	8300 9450 6850 9450
$EndSCHEMATC
