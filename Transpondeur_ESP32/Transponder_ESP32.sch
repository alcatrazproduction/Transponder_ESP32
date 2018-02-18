EESchema Schematic File Version 2
LIBS:power
LIBS:device
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
LIBS:ESP32-footprints-Shem-Lib
LIBS:ME6211C33M5G
LIBS:Transponder_ESP32-cache
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
L C C5
U 1 1 5A604C04
P 5400 6700
F 0 "C5" H 5425 6800 50  0000 L CNN
F 1 "68p" H 5425 6600 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.8mm_W2.6mm_P2.50mm" H 5438 6550 50  0001 C CNN
F 3 "" H 5400 6700 50  0001 C CNN
	1    5400 6700
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 5A604C3B
P 5650 6550
F 0 "C6" V 5675 6650 50  0000 L TNN
F 1 "47p" V 5675 6450 50  0000 R TNN
F 2 "Capacitors_THT:C_Disc_D3.8mm_W2.6mm_P2.50mm" H 5688 6400 50  0001 C CNN
F 3 "" H 5650 6550 50  0001 C CNN
	1    5650 6550
	0    -1   -1   0   
$EndComp
$Comp
L R R3
U 1 1 5A604D0E
P 4350 6250
F 0 "R3" V 4430 6250 50  0000 C CNN
F 1 "100R" V 4350 6250 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 4280 6250 50  0001 C CNN
F 3 "" H 4350 6250 50  0001 C CNN
	1    4350 6250
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5A604D4E
P 3400 6250
F 0 "R2" V 3480 6250 50  0000 C CNN
F 1 "100R" V 3400 6250 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 3330 6250 50  0001 C CNN
F 3 "" H 3400 6250 50  0001 C CNN
	1    3400 6250
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 5A604D87
P 4200 4950
F 0 "R1" V 4280 4950 50  0000 C CNN
F 1 "1K5" V 4200 4950 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 4130 4950 50  0001 C CNN
F 3 "" H 4200 4950 50  0001 C CNN
	1    4200 4950
	1    0    0    -1  
$EndComp
$Comp
L CP C2
U 1 1 5A604E0C
P 3750 3550
F 0 "C2" H 3775 3650 50  0000 L CNN
F 1 "10uF" H 3775 3450 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.8mm_W2.6mm_P2.50mm" H 3788 3400 50  0001 C CNN
F 3 "" H 3750 3550 50  0001 C CNN
	1    3750 3550
	1    0    0    -1  
$EndComp
$Comp
L LED D1
U 1 1 5A604CCC
P 4200 4650
F 0 "D1" H 4200 4750 50  0000 C CNN
F 1 "LED" H 4200 4550 50  0000 C CNN
F 2 "LEDs:LED_D2.0mm_W4.8mm_H2.5mm_FlatTop" H 4200 4650 50  0001 C CNN
F 3 "" H 4200 4650 50  0001 C CNN
	1    4200 4650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4200 4000 4200 4500
Wire Wire Line
	3400 6600 5150 6600
Wire Wire Line
	5150 6600 5150 6550
Wire Wire Line
	5150 6550 5500 6550
Wire Wire Line
	4350 6800 5150 6800
Wire Wire Line
	5150 6800 5150 6850
Wire Wire Line
	5150 6850 6050 6850
Wire Wire Line
	3500 3200 4900 3200
$Comp
L Antenna_Loop AE1
U 1 1 5A607F52
P 6250 6650
F 0 "AE1" V 6300 6900 50  0000 C CNN
F 1 "Loop Antenna Wire 0.125, 18turns 24x24,19 turns 27x18" V 6300 6850 50  0001 R CNN
F 2 "Socket_Strips:Socket_Strip_Angled_1x02_Pitch2.54mm" H 6250 6650 50  0001 C CNN
F 3 "" H 6250 6650 50  0001 C CNN
	1    6250 6650
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 6550 6050 6650
Wire Wire Line
	6050 6850 6050 6750
Connection ~ 5400 6850
Connection ~ 5400 6550
Wire Wire Line
	5800 6550 6050 6550
Connection ~ 6200 2900
Text Notes 4900 7550 0    60   Italic 0
Loop Antenna sizes ( Wire 0.125mm diam):\n18 Turns for squares 24x24mm\n19 Turns for rectangle 27x18mm\n18 Turns for rectange 36x15mm ???\nL = ??? 20uH or 32uH
$Comp
L ESP32-WROOM U1
U 1 1 5A7ED25E
P 5850 3800
F 0 "U1" H 5150 5050 60  0000 C CNN
F 1 "ESP32-WROOM" H 6350 5050 39  0000 C CNN
F 2 "ESP32-footprints-Lib:ESP32-WROOM" H 6200 5150 60  0001 C CNN
F 3 "" H 5400 4250 60  0001 C CNN
	1    5850 3800
	1    0    0    -1  
$EndComp
$Comp
L ME6211C33M5G IC2
U 1 1 5A7EE31F
P 2400 3200
F 0 "IC2" H 3350 3500 50  0000 L CNN
F 1 "ME6211C33M5G" H 3350 3400 50  0000 L CNN
F 2 "SOT95P282X145-5N" H 3350 3300 50  0001 L CNN
F 3 "http://xinhuidianzi.com.cn/UploadSysFiles/files/LDO-series/ME6211-Series_E15.0.pdf" H 3350 3200 50  0001 L CNN
F 4 "High Speed LDO Regulators, High PSRR, Low noise, ME6211 Series" H 3350 3100 50  0001 L CNN "Description"
F 5 "Microne" H 3350 3000 50  0001 L CNN "Manufacturer_Name"
F 6 "ME6211C33M5G" H 3350 2900 50  0001 L CNN "Manufacturer_Part_Number"
F 7 "RS" H 3350 2800 50  0001 L CNN "Supplier_Name"
F 8 "1.45" H 3350 2600 50  0001 L CNN "Height"
	1    2400 3200
	1    0    0    -1  
$EndComp
$Comp
L CP C1
U 1 1 5A7EE3E1
P 1850 3550
F 0 "C1" H 1875 3650 50  0000 L CNN
F 1 "10uF" H 1875 3450 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.8mm_W2.6mm_P2.50mm" H 1888 3400 50  0001 C CNN
F 3 "" H 1850 3550 50  0001 C CNN
	1    1850 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 1700 1850 3400
Connection ~ 1850 3200
Wire Wire Line
	1850 3700 1850 7500
Connection ~ 1850 5300
Wire Wire Line
	1600 3200 2400 3200
Wire Wire Line
	2400 3400 2200 3400
Wire Wire Line
	2200 3400 2200 3200
Connection ~ 2200 3200
Wire Wire Line
	2400 3300 2300 3300
Wire Wire Line
	6900 5300 1850 5300
Connection ~ 2300 5300
Wire Wire Line
	5400 4850 5400 5900
Wire Wire Line
	6750 4450 6900 4450
Wire Wire Line
	6900 4350 6900 5300
Connection ~ 5400 5300
Wire Wire Line
	6900 4350 6750 4350
Connection ~ 6900 4450
$Comp
L +5V #Vcc01
U 1 1 5A7EE6D0
P 1850 1700
F 0 "#Vcc01" H 1850 1550 50  0001 C CNN
F 1 "+5V" H 1850 1840 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_Pitch2.54mm" H 1850 1700 50  0001 C CNN
F 3 "" H 1850 1700 50  0001 C CNN
	1    1850 1700
	1    0    0    -1  
$EndComp
$Comp
L GNDREF #Gnd02
U 1 1 5A7EE6FA
P 5400 5900
F 0 "#Gnd02" H 5400 5650 50  0001 C CNN
F 1 "GNDREF" H 5600 5800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_Pitch2.54mm" H 5400 5900 50  0001 C CNN
F 3 "" H 5400 5900 50  0001 C CNN
	1    5400 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 4000 4900 4000
Wire Wire Line
	2700 4100 4900 4100
Text Label 4600 4100 0    60   ~ 0
RFOut
Text Label 4600 4000 0    60   ~ 0
LEDOut
Wire Wire Line
	4900 4500 4850 4500
Wire Wire Line
	4850 4500 4850 5300
Connection ~ 4850 5300
Wire Wire Line
	4200 5100 4200 5300
Connection ~ 4200 5300
Wire Wire Line
	1600 5900 1600 3200
Wire Wire Line
	2300 3300 2300 5300
$Comp
L 2N3904 Q2
U 1 1 5A888105
P 4250 7150
F 0 "Q2" H 4450 7225 50  0000 L CNN
F 1 "2N3904" H 4450 7150 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Molded_Narrow" H 4450 7075 50  0001 L CIN
F 3 "" H 4250 7150 50  0001 L CNN
	1    4250 7150
	1    0    0    -1  
$EndComp
$Comp
L 2N3904 Q1
U 1 1 5A888156
P 3300 7150
F 0 "Q1" H 3500 7225 50  0000 L CNN
F 1 "2N3904" H 3500 7150 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Molded_Narrow" H 3500 7075 50  0001 L CIN
F 3 "" H 3300 7150 50  0001 L CNN
	1    3300 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 6400 4350 6950
Wire Wire Line
	3400 6400 3400 6950
Text Label 4550 4200 0    60   ~ 0
/RFOut
Wire Wire Line
	1600 5900 4350 5900
Wire Wire Line
	4350 5900 4350 6100
Connection ~ 4350 6800
Connection ~ 3400 6600
Wire Wire Line
	3400 6100 3400 5900
Connection ~ 3400 5900
Wire Wire Line
	4350 7500 4350 7350
Wire Wire Line
	1850 7500 4350 7500
Wire Wire Line
	3400 7350 3400 7500
Connection ~ 3400 7500
Wire Wire Line
	2700 7150 3100 7150
Wire Wire Line
	2700 4600 2700 7150
$Comp
L R R5
U 1 1 5A888651
P 4000 4450
F 0 "R5" V 4080 4450 50  0000 C CNN
F 1 "1K" V 4000 4450 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 3930 4450 50  0001 C CNN
F 3 "" H 4000 4450 50  0001 C CNN
	1    4000 4450
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 5A888657
P 2700 4450
F 0 "R4" V 2780 4450 50  0000 C CNN
F 1 "1K" V 2700 4450 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 2630 4450 50  0001 C CNN
F 3 "" H 2700 4450 50  0001 C CNN
	1    2700 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 4100 2700 4300
Wire Wire Line
	4050 7150 4000 7150
Wire Wire Line
	4000 7150 4000 4600
Wire Wire Line
	4000 4300 4000 4200
Wire Wire Line
	4000 4200 4900 4200
Wire Wire Line
	3750 3400 3750 3200
Connection ~ 3750 3200
Wire Wire Line
	3750 3700 3750 5300
Connection ~ 3750 5300
$EndSCHEMATC
