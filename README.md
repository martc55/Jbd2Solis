# JBD2SOLIS
Bridge between JBD BMS and a DIY Battery and Solis Hybrid inverter using Pylontech CAN bus protocol
Inspired by https://github.com/loopyengineeringco/Daly2Sofar I wanted to send the State of Charge (SoC) of the battery to the inverter to give greater control. I have a DIY 16 cell LiFePO4 battery (about 4.5kWh), The BMS used is the JBD 16S LiFePO4 100A. That was setup as a lead acid battery on a Solis inverter, working fine for 18 months. So why do I need the BMS to communicate to the inverter?
I need it to control the battery using the SoC, not the voltage.
BMS has 2 output ports, One UART for BT module and I use the app to log the voltage and current data. I can also use the app to change the default properties of the BMS. The other port is a RS485 port with 2 wires A & B, I used this port to connect the BMS to the inverter.

Project Hardware:
Arduino Nano, RS485 to UART module, MCP2515 CAN bus module and DC to DC converter.

CODE is based on:
https://github.com/bres55/Smart-BMS-arduino-Reader  which has been heavily modified. 
https://github.com/coryjfowler/MCP_CAN_lib/  modified to cope with multiple CAN messages sent to the inverter every second.
https://cryptii.com/pipes/integer-encoder      for checking output HEX values 

Libraries:
mcp_can.h  - by cory j fowler  Tried other libraries but they did not communicate with the Solis inverter.

PYLONTECH Emulation: This consists of six CAN packets once per second. CAN ID – followed by 2 to 8 bytes of data. The data rate is 500kbps 
