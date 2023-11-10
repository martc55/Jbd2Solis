void Get_Data()            // DATA FROM JDB BMS
 {
  write_request_start();        // Found this helps timing issues,
  write_request_end() ;         
  write_request_start();        
  write_request_end() ;         // And accomodates long delays if you want them at the end
  
// CELLS VOLTAGE  
  call_get_cells_v();           // requests cells voltage, returns with up to date, inString= chars, 
  get_bms_feedback();           // inInts[]= numbers, chksum in last 2 bytes, Length (length of data string) got cell voltages
                                // bytes 0 and 1, its 16 bit, high and low, go through and print the Length = Length - 2; print headings                                
  for (int i = 2; i < (Length + 1); i = i + 2) {
      }                                   
  CellMax = 0;
  CellMin = 5;
  for (int i = 0; i < Length; i = i + 2) {
    highbyte = (inInts[i]);
    lowbyte = (inInts[i + 1]);
    uint16_t Cellnow = two_ints_into16(highbyte, lowbyte);
    float Cellnowf = Cellnow / 1000.0f;           // convert to float
    myCellVoltages[i/2] = Cellnowf;               // Update value in array
    if (Cellnowf > CellMax) {                     // get high and low
     CellMax = Cellnowf;
      
    }
    if (Cellnowf < CellMin) {
     CellMin = Cellnowf; 
    }
  }   
  Celldiff = CellMax - CellMin;         // difference between highest and lowest

 // BASIC INFOMATION AND FEEDBACK
  call_Basic_info();            // requests basic info.
  get_bms_feedback();           // get that data, used to get BALANCE STATE byte 17 less 4, decimal=byte 13

  
// STATE OF CHARGE AS A PERCENTAGE
  RSOC = (inInts[19]);
  int deviation = RSOC + PSOC;                 // To reduce fluctuations in RSOC data
  if (abs(deviation) <= maxDev){               // Maximum deviation of (default 250)
   SOC = RSOC;                                 // RSOC input, PSOC previous, SOC output
    PSOC = RSOC;
  }else{
    SOC = PSOC;                                
  } 
  
// BATERY VOLTAGE   (16 bit, high and low)
  highbyte = (inInts[0]);                     // bytes 0 and 1
  lowbyte = (inInts[1]);
  uint16_t PackVoltage = two_ints_into16(highbyte, lowbyte);
  PackVoltagef = PackVoltage / 100.0f;        // convert to float and leave at 2 dec places

// DISCHAGED (-)& CHARGE CURRENT (+) UNO/NANO
  highbyte = (inInts[2]);                     // bytes 2 and 3
  lowbyte = (inInts[3]);
  int PackCurrent = two_ints_into16(highbyte, lowbyte);
  PackCurrentf = PackCurrent /100.0f;        // convert to float and leave at 2 dec places

// CAPACITY REMAINING Ah - Optional                    
  highbyte = (inInts[4]);                    // bytes 4 and 5
  lowbyte = (inInts[5]);
  uint16_t RemainCapacity = two_ints_into16(highbyte, lowbyte);
  RemainCapacityf = RemainCapacity /100.0f;  // convert to float and leave at 2 dec places

// TEMPERATURE 1 ( deg C)
  highbyte = (inInts[23]);
  lowbyte = (inInts[24]);
  float Temp_1 = two_ints_into16(highbyte, lowbyte);
  Temp_probe_1 = (Temp_1 - 2731) / 10.0f;   // convert to float and leave at 2 dec places
  if (Temp_probe_1 < -3 || Temp_probe_1 > 50) {
    Temp_probe_1 = 20;                      // to prevent random values sent to the inverter
  }
  
// TEMPERATURE 2 - Optional
  highbyte = (inInts[25]);
  lowbyte = (inInts[26]);
  float Temp_2 = two_ints_into16(highbyte, lowbyte);
  Temp_probe_2 = (Temp_2 - 2731) / 10.0f;    // convert to float and leave at 2 dec places

// MOSFETS CONTROLS - Optional
  chargeFet = inInts[20] & 1; //bit0
  dischargeFet = (inInts[20] >> 1) & 1; //bit1

// AFE ERROR - Optional
  AFEerror = (inInts[16] >> 3) & 1;

 
  inString = "";
  Length = 0;
}


//------------------------------------------------------------------------------------------------------
//                         Do not edit anything below this line
//------------------------------------------------------------------------------------------------------
//  uint16_t PackCurrent = two_ints_into16(highbyte, lowbyte);
  uint16_t two_ints_into16(int highbyte, int lowbyte) 
                                              // turns two bytes into a single long integer
{
  a16bitvar = (highbyte);
  a16bitvar <<= 8;                           //Left shift 8 bits,
  a16bitvar = (a16bitvar | lowbyte);         //OR operation, merge the two
  return a16bitvar;
}
// ----------------------------------------------------------------------------------------------------
// total voltage, current, Residual capacity, Balanced state, MOSFET control status
void call_Basic_info()
{
  flush();                                    // flush first

                                              //  DD  A5 03 00  FF  FD  77
                                              // 221 165  3  0 255 253 119
  uint8_t data[7] = {221, 165, 3, 0, 255, 253, 119};
  MySerial.write(data, 7);
}
//--------------------------------------------------------------------------------------------------------
void call_get_cells_v()
{
  flush();                                     // flush first

                                               // DD  A5  4 0 FF  FC  77
                                               // 221 165 4 0 255 252 119
  uint8_t data[7] = {221, 165, 4, 0, 255, 252, 119};
  MySerial.write(data, 7);
}
//--------------------------------------------------------------------------------------------------------
void call_Hardware_info()                      
{
  flush();                                     // flush first

                                               //  DD  A5 05 00  FF  FB  77
                                               // 221 165  5  0 255 251 119
  uint8_t data[7] = {221, 165, 5, 0, 255, 251, 119};
                                               // uint8_t data[7] = {DD, A5, 05, 00, FF, FB, 77};
  MySerial.write(data, 7);
}
//---------------------------------------------------------------------------------------------------------
void write_request_start()
{
  flush();                                   // flush first
                                             //   DD 5A 00  02 56  78  FF  30  77
                                             //  221 90  0  2  86  120 255 48  119
  uint8_t data[9] = {221, 90, 0, 2, 86, 120, 255, 48, 119};
  MySerial.write(data, 9);
}
//---------------------------------------------------------------------------------------------------------
void write_request_end()
{
  flush();                                   // flush first
                                             //  DD 5A 01  02 00 00   FF  FD 77
                                             //  221 90 1  2  0  0  255  253 119
  uint8_t data[9] = {221, 90, 1, 2, 0, 0, 255, 253, 119};
  MySerial.write(data, 9);
}
//---------------------------------------------------------------------------------------------------------
void eprom_read()                           
{
  flush();                                   // flush first
                                             //delay(5);
                                             // SENT CODE depends on WHAT IS REQD???
                                             //   DD  A5  A2 0  FF 5E  77...BAR CODE
                                             //  221 165 162 0 255 94 119
                                             // uint8_t data[7] = {221, 165, 162, 0, 255, 94, 119};
  uint8_t data[7] = {221, 165, 32, 0, 255, 224, 119};
  MySerial.write(data, 7);
}
//---------------------------------------------------------------------------------------------------------
void eprom_end()                             
{
  flush();                                   // flush first
                                             // delay(5);
                                             //DD  A5  AA  0 FF  56  77
                                             //221 165 170 0 255 86  119
                                             // from eprom read
  uint8_t data[7] = {221, 165, 170, 0, 255, 86, 119};
  MySerial.write(data, 7);
}
//----------------------------------------------------------------------------------------------------------
void flush()
{                                            // FLUSH
  delay(100);                                // give it a mo to settle, seems to miss occasionally without this
  while (MySerial.available() > 0)
  { MySerial.read();
  }
  delay(50);                                 // give it a mo to settle, seems to miss occasionally without this
}
//------------------------------------------------------------------------------------------------------------
void get_bms_feedback()                      // returns with up to date, inString= chars, inInts= numbers,                                             // chksum in last 2 bytes Length Data only, exclude first 3 bytes
{
  inString = "";                             // clear instring for new incoming
  delay(100);                                // give it a mo to settle, seems to miss occasionally without this
  if (MySerial.available() > 0) {
    {
      for (int i = 0; i < 4; i++)            // just get first 4 bytes
      {
        incomingByte = MySerial.read();
        if (i == 3)
        {                                    // could look at 3rd byte, it's the ok signal
          Length = (incomingByte);           // The fourth byte holds the length of data, excluding last 3 bytes checksum etc
          // Serial.print(" inc ");
          //Serial.print(incomingByte);
        }
        if (Length == 0) {
          Length = 1;                        // in some responses, length=0, dont want that, so, make Length=1
        }
      }
                                                //  Length = Length + 2; want to get the checksum too, for writing back, 
                                                // saves calculating it later
      for (int i = 0; i < Length + 2; i++) {    // get the checksum in last two bytes, just in case need later
        incomingByte = MySerial.read();         // get the rest of the data, how long it might be.
        inString += (char)incomingByte;         // convert the incoming byte to a char and add it to the string
        inInts[i] = incomingByte;               // save incoming byte to array as int
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------------
byte Bit_Reverse( byte x )                                   
{
                                                 //   01010101  |   10101010
  x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
                                                 //   00110011  |   11001100
  x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
                                                 //   00001111  |   11110000
  x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
  return x;
}
