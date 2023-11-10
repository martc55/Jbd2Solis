void CAN_bus()  // Sends package message every second 
{         
 unsigned long currentMillisCAN = millis();
  if (currentMillisCAN - previousMillisCAN >= intervalCAN) {
  previousMillisCAN = currentMillisCAN;

  updateCANData();

    CAN0.sendMsgBuf(0x359, 0, 7, CANData359);   // Protection & Alarm flags
    delay(5);
    memset(0x359, 0, 7);
    
    CAN0.sendMsgBuf(0x351, 0, 8, CANData351);   // Battery voltage + current limits
    delay(5);
    memset(0x351, 0, 8);
  
    CAN0.sendMsgBuf(0x355, 0, 4, CANData355);   // State of Charge (SOC) / State of Health (SOH)
    delay(5);
    memset(0x355, 0, 4);
    
    CAN0.sendMsgBuf(0x356, 0, 6, CANData356);  // Voltage / Current / Temp
    delay(5);
    memset(0x356, 0, 6);

    CAN0.sendMsgBuf(0x35C, 0, 2, CANData35C);  // Battery charge request flags
    delay(5);
    memset(0x35C, 0, 2);

    CAN0.sendMsgBuf(0x35E, 0, 8, CANData35E);  // Manufacturer name (“PYLON “)
    delay(5);
    memset(0x35E, 0, 8);
  
 }  // millis ends
} 
