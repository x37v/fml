EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:teensy
LIBS:xnorman
LIBS:fml-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
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
$Comp
L Teensy3.6 U1
U 1 1 5A11ADE6
P 5250 3600
F 0 "U1" H 5250 5900 60  0000 C CNN
F 1 "Teensy3.6" H 5250 1300 60  0000 C CNN
F 2 "teensy:Teensy35_36" H 5250 3600 60  0001 C CNN
F 3 "" H 5250 3600 60  0000 C CNN
	1    5250 3600
	1    0    0    -1  
$EndComp
$Comp
L 6N138 U2
U 1 1 5A1454E2
P 2400 1700
F 0 "U2" H 2200 2000 50  0000 L CNN
F 1 "6N138" H 2400 2000 50  0000 L CNN
F 2 "Housings_DIP:DIP-8_W7.62mm" H 2200 1400 50  0001 L CIN
F 3 "" H 2320 1750 50  0001 L CNN
	1    2400 1700
	1    0    0    -1  
$EndComp
Text GLabel 6900 4300 2    60   Output ~ 0
5v
$Comp
L R R1
U 1 1 5A150143
P 1500 1550
F 0 "R1" V 1580 1550 50  0000 C CNN
F 1 "220R" V 1500 1550 50  0000 C CNN
F 2 "xnorman:Resistor_Horizontal_RM10mm-xnorman" V 1430 1550 50  0001 C CNN
F 3 "" H 1500 1550 50  0001 C CNN
	1    1500 1550
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 5A15017C
P 3350 1400
F 0 "R3" V 3430 1400 50  0000 C CNN
F 1 "270R" V 3350 1400 50  0000 C CNN
F 2 "xnorman:Resistor_Horizontal_RM10mm-xnorman" V 3280 1400 50  0001 C CNN
F 3 "" H 3350 1400 50  0001 C CNN
	1    3350 1400
	1    0    0    -1  
$EndComp
$Comp
L D D1
U 1 1 5A15019B
P 1900 1700
F 0 "D1" H 1900 1800 50  0000 C CNN
F 1 "1n914" H 1900 1600 50  0000 C CNN
F 2 "Diodes_ThroughHole:D_A-405_P7.62mm_Horizontal" H 1900 1700 50  0001 C CNN
F 3 "" H 1900 1700 50  0001 C CNN
	1    1900 1700
	0    1    1    0   
$EndComp
$Comp
L C C1
U 1 1 5A1501BC
P 3150 1700
F 0 "C1" V 3200 1550 50  0000 L CNN
F 1 "0.1u" V 3100 1500 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D5.0mm_W2.5mm_P2.50mm" H 3188 1550 50  0001 C CNN
F 3 "" H 3150 1700 50  0001 C CNN
	1    3150 1700
	-1   0    0    1   
$EndComp
$Comp
L R R5
U 1 1 5A150283
P 4400 950
F 0 "R5" V 4480 950 50  0000 C CNN
F 1 "47R" V 4400 950 50  0000 C CNN
F 2 "xnorman:Resistor_Horizontal_RM10mm-xnorman" V 4330 950 50  0001 C CNN
F 3 "" H 4400 950 50  0001 C CNN
	1    4400 950 
	0    1    1    0   
$EndComp
$Comp
L R R4
U 1 1 5A1502BE
P 4400 850
F 0 "R4" V 4350 1000 50  0000 C CNN
F 1 "47R" V 4400 850 50  0000 C CNN
F 2 "xnorman:Resistor_Horizontal_RM10mm-xnorman" V 4330 850 50  0001 C CNN
F 3 "" H 4400 850 50  0001 C CNN
	1    4400 850 
	0    1    1    0   
$EndComp
$Comp
L GND #PWR01
U 1 1 5A150446
P 2850 2100
F 0 "#PWR01" H 2850 1850 50  0001 C CNN
F 1 "GND" H 2850 1950 50  0000 C CNN
F 2 "" H 2850 2100 50  0001 C CNN
F 3 "" H 2850 2100 50  0001 C CNN
	1    2850 2100
	1    0    0    -1  
$EndComp
Text GLabel 2800 1350 1    60   Input ~ 0
5v
$Comp
L Conn_01x02 J1
U 1 1 5A150574
P 950 1550
F 0 "J1" H 950 1650 50  0000 C CNN
F 1 "midi_in" H 950 1350 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 950 1550 50  0001 C CNN
F 3 "" H 950 1550 50  0001 C CNN
	1    950  1550
	-1   0    0    -1  
$EndComp
$Comp
L Conn_01x03 J2
U 1 1 5A150850
P 4000 850
F 0 "J2" H 4000 1050 50  0000 C CNN
F 1 "midi_out" H 4000 650 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 4000 850 50  0001 C CNN
F 3 "" H 4000 850 50  0001 C CNN
	1    4000 850 
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6250 4300 6900 4300
Wire Wire Line
	3350 2000 2750 2000
Wire Wire Line
	3350 1550 3350 2000
Wire Wire Line
	3150 1900 3150 1850
Wire Wire Line
	2700 1900 3150 1900
Wire Wire Line
	2850 2100 2850 1900
Connection ~ 2850 1900
Wire Wire Line
	3150 1500 3150 1550
Wire Wire Line
	2700 1500 3150 1500
Wire Wire Line
	2800 1500 2800 1350
Connection ~ 2800 1500
Wire Wire Line
	2100 1600 2050 1600
Wire Wire Line
	2050 1550 2050 1600
Wire Wire Line
	1650 1550 2050 1550
Wire Wire Line
	2100 1800 2000 1800
Wire Wire Line
	2000 1850 2000 1800
Wire Wire Line
	1300 1850 2000 1850
Connection ~ 1900 1550
Wire Wire Line
	1350 1550 1150 1550
Wire Wire Line
	1150 1650 1300 1650
Wire Wire Line
	1300 1650 1300 1850
Connection ~ 1900 1850
Wire Wire Line
	4250 950  4200 950 
Wire Wire Line
	4250 850  4200 850 
$Comp
L GND #PWR02
U 1 1 5A150A6E
P 4650 700
F 0 "#PWR02" H 4650 450 50  0001 C CNN
F 1 "GND" H 4650 550 50  0000 C CNN
F 2 "" H 4650 700 50  0001 C CNN
F 3 "" H 4650 700 50  0001 C CNN
	1    4650 700 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4200 750  4650 700 
$Comp
L R R2
U 1 1 5A150B51
P 2850 1750
F 0 "R2" V 2930 1750 50  0000 C CNN
F 1 "10k" V 2850 1750 50  0000 C CNN
F 2 "xnorman:Resistor_Horizontal_RM10mm-xnorman" V 2780 1750 50  0001 C CNN
F 3 "" H 2850 1750 50  0001 C CNN
	1    2850 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 1600 2700 1600
Connection ~ 3150 1900
Wire Wire Line
	3350 1250 3350 1150
Text GLabel 3350 1150 1    60   Input ~ 0
3.3v
Wire Wire Line
	2750 2000 2750 1800
Wire Wire Line
	2750 1800 2700 1800
Wire Wire Line
	4250 1600 3450 1600
Wire Wire Line
	3450 1600 3450 1650
Wire Wire Line
	3450 1650 3350 1650
Connection ~ 3350 1650
Wire Wire Line
	4550 950  4600 950 
Wire Wire Line
	4600 950  4600 1250
Wire Wire Line
	4600 1250 4000 1250
Wire Wire Line
	4000 1250 4000 1700
Wire Wire Line
	4000 1700 4250 1700
Text GLabel 4600 850  2    60   Input ~ 0
3.3v
Wire Wire Line
	4600 850  4550 850 
$Comp
L GND #PWR03
U 1 1 5A151034
P 3900 1500
F 0 "#PWR03" H 3900 1250 50  0001 C CNN
F 1 "GND" H 3900 1350 50  0000 C CNN
F 2 "" H 3900 1500 50  0001 C CNN
F 3 "" H 3900 1500 50  0001 C CNN
	1    3900 1500
	0    1    1    0   
$EndComp
Wire Wire Line
	3900 1500 4250 1500
Wire Wire Line
	6250 2600 7000 2600
Text GLabel 7000 2600 2    60   Output ~ 0
3.3v
Wire Wire Line
	6250 2700 6850 2700
Wire Wire Line
	6850 4000 6850 2700
Wire Wire Line
	6250 4000 6850 4000
Wire Wire Line
	6250 3900 6400 3900
Wire Wire Line
	6400 3900 6400 4000
Connection ~ 6400 4000
$Comp
L GND #PWR04
U 1 1 5A15181B
P 7050 3750
F 0 "#PWR04" H 7050 3500 50  0001 C CNN
F 1 "GND" H 7050 3600 50  0000 C CNN
F 2 "" H 7050 3750 50  0001 C CNN
F 3 "" H 7050 3750 50  0001 C CNN
	1    7050 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 3750 7050 3650
Wire Wire Line
	7050 3650 6850 3650
Connection ~ 6850 3650
Text Label 1200 1500 1    60   ~ 0
4
Text Label 1200 1750 1    60   ~ 0
5
Text Label 4250 700  1    60   ~ 0
2
Text Label 4200 1050 1    60   ~ 0
5
Text Label 4200 900  1    60   ~ 0
4
$EndSCHEMATC
