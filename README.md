# JBD2SOLIS
Simple Bridge between JBD BMS with a DIY Battery and Solis Hybrid inverter using Pylontech CAN bus protocol.

I wanted to send the State of Charge (SoC) of the battery to the inverter 
to give greater control. I have a DIY 16 cell LiFePO4 battery (about 4.5kWh).
The BMS used is the JBD-SP25S003-L16S-100A. It was setup as a lead acid battery on my Solis inverter 
(RHI-3K-48ES-5G) and worked OK for 18 months, as long as you don't over discharge. 
When it gets near to 20% SoC, at my battery voltage is about 51V, under low load. I noted that at 20% it did not completely disconnect the battery, but it continues to discharge about 42W, so I set force charge at 15%. 

BMS has 2 output ports, One UART for BT module and I use the app to log the voltage and current data. 
I can also use the app to change the default properties of the BMS. 
The other port is a RS485 port with 2 wires A & B,           
I used this port to connect the BMS to the inverter.

HAREWARE                                                                                         
Arduino Nano, RS485 to UART module, MCP2515 CAN bus module and DC to DC converter.

CODE is based on:  
https://github.com/bres55/Smart-BMS-arduino-Reader - which has been heavily modified.

https://github.com/coryjfowler/MCP_CAN_lib/ - modified to cope with multiple CAN messages 

https://cryptii.com/pipes/integer-encoder - for checking output HEX values 

LIBRARY:                                                                                  
mcp_can.h  - by cory j fowler,  Tried other libraries but I could not get them to communicate with the Solis inverter.

PYLONTECH Emulation: This consists of six CAN packets once per second. CAN ID – followed by 2 to 8 bytes of data. 
The data rate is 500kbps 

SCHEMATIC
![Solis_Sch2](https://github.com/martc55/JBD2SOLIS/assets/40126951/3cbb71ba-61c7-4f2f-ab63-f85e19b3d75f)

MCP2515 MODULE                                                                                            
The MCP2515 module is a CAN interface which communicates with the Nano via SPI. It is powered from 5V, same as the Nano.  J1 is for enabling the terminator resistor so  leave it connected. J2 is for the connection to the CAN bus. I have designed a PCB that connects all modules, and had to reverse all pin connections by re soldering the pins. The J3 now connects to the RJ45 socket on the PCB.
Output pins SCK -> D13,  SI -> D11,  SO -> D12,  CS -> D10 on the Nano.

RS485 to UART MODULE                                                                                   
Input side it has 3 pins, A and B plus an earth (not ground). If the RS485 communication cable runs outside the building then this pin should be connected to earth. I ignored this connection.
Output can be either 5V or 3.3V but I used 5V to make it common with the other componance.
Tx -> D5 and Rx -> D6 on the Nano.

DC to DC CONVERTER MODULE                                                                                                                                                             
Input is up to 60Vdc because I connect directly to the storage battery.
Output 5Vdc

RJ45 CONNECTOR                                                                                 
CAN bus Cable:  H (Blue) to pin 4 and L (Blue/White) to pin 5,   Ground to pin 2.

CODE                                                                                    
Look for the lines in the code and change if required:                                       

MCP_CAN CAN0(10); - (Set CS to Nano pin 10)                                                   
if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) - (must be set to 500BPS but 8MHZ can be changed )                                    
CAN0.setMode(MCP_NORMAL); - (Set to normal mode to allow messages to be transmitted)                            
SoftwareSerial MySoftSerial(6, 5); - (UART Connections Rx -> D6, Tx -> D5, Vcc 5V, Gnd) 

float batteryChargeVoltage = 55.5; - (BMS set to 57.5V)                                    
float ChargeCurrentLimit = 25; - (BMS set to 30A)                               
float DischargeCurrentLimit  = 60; - (BMS set to 65A)                    
float StateOfHealth  = 99; - (BMS dos not calculate SoH)           
(The BMS should be set up as a safety device if the inverter falls.)
       
CAN bus PylonTech Protocol
![CAN_bus2](https://github.com/martc55/Jbd2Solis/assets/40126951/c49fbcd9-d86c-4adb-a7a8-d5cfb6922707)
0x359 - Protection & Alarm flags.                               
0x351 - Voltage & Current Limits set up in the code.                                     
0x355 - SoC varies, and SoH fixed in the code.                      
0x356 - Voltage / Current / Temp. - data varies                                      
0x35C - Battery charge request flags.                                      
0x35E - Manufacturer name (“PYLON”).                                    
0x305 - Solis sends this message every second.

I used another Nano and MCP2515 to read and check the CAN bus output on the serial monitor,  
his must be correct before connecting to the Solis inverter. 
You should see 6 packets of data every second, the 7th packet (0x305) only when connected to the inverter.      

BATTERY SETUP AS “PYLON-LV” sample displays
![Solis_Displays](https://github.com/martc55/Jbd2Solis/assets/40126951/f142b169-5b99-4e7a-9ec1-8328fc439f86)

USE THIS AT YOUR OWN RISK!                                                             
LiFePO4 Batteries are one of the safer batteries but can still be DANGEROUS. 
This is a working prototype, it might have some glitches. 
It is possible that sometimes the inverter may overcharge/undercharge. 
Make sure your BMS is set up properly with safety limits. 
I take no responsibility for any damage or injury that may occur.



