// JBD (Xiaoxiang) BMS to SOLIS Inveter via CAN bus
// Hardware:
// Arduino Nano
// RS485 to UART Module  Diymore  or simular
// MCP2515 Module (5V) 8/16Mhz crystal 
// DC to DC Converter (12 - 60V input to 5V)
// Pin outs:
// RS485 to Nano:     Rx -> D6, Tx -> D5, Gnd, 5V
// MCP2515 to Nano:   CS -> D10, SCK -> D13, SO -> D12, SI -> D11, Gnd, 5V

// CODE is based on:
// Read BMS:   https://github.com/bres55/Smart-BMS-arduino-Reader  which has been heavily modified
// CAN bus:    https://github.com/coryjfowler/MCP_CAN_lib/  adapted to cope with multiple messages sent
// HEX lookup: https://cryptii.com/pipes/integer-encoder  for working out and checking HEX values

// PYLONTECH Emulation: 
// This consists of six CAN packets once per second.
// CAN ID – followed by 2 to 8 bytes of data:
// example:
// 0x359 – 7 - 00 00 00 00 00 50 4E          – (7) Protection & Alarm flags
// 0x351 – 8 - 14 02 74 0E 74 0E CC 01       – (8) Battery voltage + current limits
// 0x355 – 4 - 1A 00 64 00                   – (4) State of Charge (SOC) / State of Health (SOH)
// 0x356 – 6 - 4E 13 02 03 04 05             – (6) Voltage / Current / Temp
// 0x35C – 2 - C0 00                         – (2) Battery charge request flags
// 0x35E – 8 - 50 59 4C 4F 4E 20 20 20       – (8) Manufacturer name (“PYLON “)

// 0x305 - 8 - 00 00 00 00 00 00 00 00       - (8) Sent every once second from Solis Inverter
// ***************************************************

// Revised 09/11/23, Tested on Solis Inverter on 13/10/23

#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>

#include <mcp_can.h>
MCP_CAN CAN0(10);                     // Set CS to pin 10

#include <SoftwareSerial.h>           // Second serial port for connecting to the BMS
SoftwareSerial MySoftSerial(6, 5);    // UART Connections Rx -> D6, Tx -> D5 Vcc 5V, Gnd, 
#define MySerial MySoftSerial

// ***************JDB BMS Variables ******************
String inString = "";              // string to hold input
int incomingByte, Length, highbyte, lowbyte;
byte Mosfet_control, mosfetnow, BatteryConfigH, BatteryConfigL, bcl, bcln, bch, Checksum, switche;
uint8_t BYTE1, BYTE2, BYTE3, BYTE4, BYTE5, BYTE6, BYTE7, BYTE8, BYTE9, BYTE10;
uint8_t inInts[40], data[9];       // an array to hold incoming data, not seen any longer than 34 bytes, or 9
uint16_t a16bitvar;

// Global battery stat variables (For printing to displays)
float CellMax = 5;                 // Default value > max possible cell votlage
float CellMin = 0; 
float Celldiff = 0;
float myCellVoltages[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // upto 20 cells
float PackVoltagef = 0;
float PackCurrentf = 0;
float RemainCapacityf = 0;         // Optional
float Temp_probe_1 = 0;
float Temp_probe_2 = 0;            // Optional
bool chargeFet = 0;                // Optional
bool dischargeFet = 0;             // Optional
bool AFEerror = 0;                 // Optional
const word  maxDev = 250;          // maximum deviation of SOC, to reduce fluctuations in SoC.
word RSOC, SOC, PSOC;              // RSOC (State of Charge from BMS), SOC (Output), PSOC (previous reading)

float batteryChargeVoltage = 56;   // BMS set to 57V (BMS higher for safety)
float ChargeCurrentLimit = 20;     // BMS set to 40A 
float DischargeCurrentLimit  = 60; // BMS set to 65A 
float StateOfHealth  = 100;        // manually set to 99-100%

// **************** CAN Timings ******************
unsigned long previousMillisCAN = 0;   // will store last time a CAN package was send
const int intervalCAN = 1000;          // interval at which send CAN package (milliseconds)

void setup()   {
  Serial.begin(9600);                // for debugging

// Init RS485 to UART 
   MySerial.begin(9600);               

// Init MCP2515
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) // Change if MCP2515 uses 16MHz
  Serial.println("MCP2515 Initialized Successfully!");
  else 
  Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);      // Set to normal mode to allow messages to be transmitted
  
} // setup ends

// PYLONTECH Protocol defaults values
unsigned char CANData359[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x4E};
unsigned char CANData351[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char CANData355[4] = {0x00, 0x00, 0x00, 0x00}; 
unsigned char CANData356[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char CANData35C[2] = {0xC0, 0x00};
unsigned char CANData35E[8] = {0x50, 0x59, 0x4C, 0x4F, 0x4E, 0x20, 0x20, 0x20};
     
void loop() 
{
  Get_Data();           // Get data from the BMS
  CAN_bus();

  Serial.print("SoC: ");
  Serial.print(SOC);
  Serial.print("   Volts: ");
  Serial.print(PackVoltagef,1);
  Serial.print("   Current: ");
  Serial.print(PackCurrentf,1);
  Serial.print("   Temp: ");
  Serial.println(Temp_probe_1,1);


}  // loop ends
