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
Text Notes 1900 1450 0    79   ~ 16
Teensy 4.0 as Sound/USB/MIDI device
Text GLabel 2300 2700 1    50   Input ~ 0
T40_VIN
Text GLabel 2900 2700 1    50   Input ~ 0
I2S_FCLK
Text GLabel 2800 2700 1    50   Input ~ 0
I2S_BCLK
$Comp
L power:GND #PWR03
U 1 1 6636E8D3
P 2400 2700
F 0 "#PWR03" H 2400 2450 50  0001 C CNN
F 1 "GND" V 2400 2500 50  0000 C CNN
F 2 "" H 2400 2700 50  0001 C CNN
F 3 "" H 2400 2700 50  0001 C CNN
	1    2400 2700
	-1   0    0    1   
$EndComp
Text GLabel 2400 4150 3    50   Input ~ 0
T40_RX1
Text GLabel 2500 4150 3    50   Input ~ 0
T40_TX1
$Comp
L power:GND #PWR06
U 1 1 66372B69
P 2300 4150
F 0 "#PWR06" H 2300 3900 50  0001 C CNN
F 1 "GND" V 2300 3950 50  0000 C CNN
F 2 "" H 2300 4150 50  0001 C CNN
F 3 "" H 2300 4150 50  0001 C CNN
	1    2300 4150
	1    0    0    -1  
$EndComp
Text GLabel 2000 5850 1    50   Input ~ 0
T40_RX1
Text GLabel 2100 5850 1    50   Input ~ 0
T40_TX1
$Comp
L power:GND #PWR08
U 1 1 66466073
P 1900 5850
F 0 "#PWR08" H 1900 5600 50  0001 C CNN
F 1 "GND" V 1900 5650 50  0000 C CNN
F 2 "" H 1900 5850 50  0001 C CNN
F 3 "" H 1900 5850 50  0001 C CNN
	1    1900 5850
	1    0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J6
U 1 1 66460E1E
P 2000 6050
F 0 "J6" V 1872 6230 50  0001 L CNN
F 1 "T4 Serial1" V 2100 5850 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 2000 6050 50  0001 C CNN
F 3 "~" H 2000 6050 50  0001 C CNN
	1    2000 6050
	0    -1   1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J5
U 1 1 6647A8E0
P 5600 4300
F 0 "J5" H 5680 4292 50  0001 L CNN
F 1 "Host USB" H 5300 4600 50  0000 L CNN
F 2 "0_my_footprints2:JST4" H 5600 4300 50  0001 C CNN
F 3 "~" H 5600 4300 50  0001 C CNN
	1    5600 4300
	1    0    0    1   
$EndComp
Text GLabel 5200 3250 2    50   Input ~ 0
HOST_D-
Text GLabel 5200 3350 2    50   Input ~ 0
HOST_D+
Text GLabel 5400 4200 0    50   Input ~ 0
HOST_D-
Text GLabel 5400 4300 0    50   Input ~ 0
HOST_D+
Text GLabel 5400 4100 0    50   Input ~ 0
T40_VIN
$Comp
L power:GND #PWR07
U 1 1 6647D394
P 5400 4400
F 0 "#PWR07" H 5400 4150 50  0001 C CNN
F 1 "GND" V 5400 4200 50  0000 C CNN
F 2 "" H 5400 4400 50  0001 C CNN
F 3 "" H 5400 4400 50  0001 C CNN
	1    5400 4400
	0    1    1    0   
$EndComp
Text GLabel 3000 4150 3    50   Input ~ 0
T40_I2S_TXB
$Comp
L Connector_Generic:Conn_01x03 J4
U 1 1 664185CC
P 5000 3250
F 0 "J4" V 4872 3430 50  0001 L CNN
F 1 "T4 bottom" H 4650 3500 50  0000 L CNN
F 2 "0_my_footprints2:pinHeader1x3" H 5000 3250 50  0001 C CNN
F 3 "~" H 5000 3250 50  0001 C CNN
	1    5000 3250
	-1   0    0    -1  
$EndComp
Text GLabel 5200 3150 2    50   Input ~ 0
T40_I2S_RXB
Text GLabel 2600 5850 1    50   Input ~ 0
T40_RX3
Text GLabel 2700 5850 1    50   Input ~ 0
T40_TX3
$Comp
L power:GND #PWR09
U 1 1 6643F40B
P 2500 5850
F 0 "#PWR09" H 2500 5600 50  0001 C CNN
F 1 "GND" V 2500 5650 50  0000 C CNN
F 2 "" H 2500 5850 50  0001 C CNN
F 3 "" H 2500 5850 50  0001 C CNN
	1    2500 5850
	1    0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J7
U 1 1 6643F411
P 2600 6050
F 0 "J7" V 2472 6230 50  0001 L CNN
F 1 "T4 Serial3" V 2700 5850 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 2600 6050 50  0001 C CNN
F 3 "~" H 2600 6050 50  0001 C CNN
	1    2600 6050
	0    -1   1    0   
$EndComp
Text GLabel 3200 5850 1    50   Input ~ 0
T40_RX4
Text GLabel 3300 5850 1    50   Input ~ 0
T40_TX4
$Comp
L power:GND #PWR010
U 1 1 6643FE08
P 3100 5850
F 0 "#PWR010" H 3100 5600 50  0001 C CNN
F 1 "GND" V 3100 5650 50  0000 C CNN
F 2 "" H 3100 5850 50  0001 C CNN
F 3 "" H 3100 5850 50  0001 C CNN
	1    3100 5850
	1    0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J8
U 1 1 6643FE0E
P 3200 6050
F 0 "J8" V 3072 6230 50  0001 L CNN
F 1 "T4 Serial4" V 3300 5850 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 3200 6050 50  0001 C CNN
F 3 "~" H 3200 6050 50  0001 C CNN
	1    3200 6050
	0    -1   1    0   
$EndComp
Text GLabel 3400 2700 1    50   Input ~ 0
T40_RX3
Text GLabel 3500 2700 1    50   Input ~ 0
T40_TX3
Text GLabel 3300 2700 1    50   Input ~ 0
T40_RX4
Text GLabel 3200 2700 1    50   Input ~ 0
T40_TX4
$Comp
L Connector_Generic:Conn_01x05 J9
U 1 1 664488D3
P 4400 6050
F 0 "J9" V 4272 5762 50  0001 R CNN
F 1 "rPi I2S" V 4500 6200 50  0000 R CNN
F 2 "0_my_footprints2:JST5" H 4400 6050 50  0001 C CNN
F 3 "~" H 4400 6050 50  0001 C CNN
	1    4400 6050
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR011
U 1 1 6644DC7F
P 4600 5850
F 0 "#PWR011" H 4600 5600 50  0001 C CNN
F 1 "GND" V 4600 5650 50  0000 C CNN
F 2 "" H 4600 5850 50  0001 C CNN
F 3 "" H 4600 5850 50  0001 C CNN
	1    4600 5850
	1    0    0    1   
$EndComp
Text GLabel 4500 5850 1    50   Input ~ 0
I2S_FCLK
Text GLabel 4400 5850 1    50   Input ~ 0
I2S_BCLK
Text GLabel 4300 5850 1    50   Input ~ 0
T40_I2S_TXB
Text GLabel 4200 5850 1    50   Input ~ 0
T40_I2S_RXB
Text GLabel 3400 4150 3    50   Input ~ 10
AUDIO_SD_CS!
$Comp
L 0_my_teensy:teensyAudioRevD U1
U 1 1 664185D7
P 2850 3400
F 0 "U1" H 3728 3903 60  0000 L CNN
F 1 "teensyAudioRevD" H 3728 3797 60  0000 L CNN
F 2 "0_my_teensy:audioShieldRevD" V 3800 3350 60  0001 C CNN
F 3 "" V 3800 3350 60  0000 C CNN
	1    2850 3400
	1    0    0    -1  
$EndComp
$EndSCHEMATC
