// *********************** CAN Data ***************************** 
void updateCANData() {
// PYLONTECH CANbus Protocol, little endian,

// STATE OF CHARGE 1%, 2 bytes, 16 bit UNSIGNED int  
    CANData355[0] = (int)SOC;                   
    CANData355[2] = (int)StateOfHealth;
    
// VOLTAGE. 0.01V, 2 bytes, 16 bit SIGNED int      
    int16_t int16volts = PackVoltagef*100;    // Value x 100, pylon unit is 0.01V
    CANData356[0] = int16volts;
    CANData356[1] = int16volts >>8;

// CURRENT. 0.1A, 2 bytes, 16 bit SIGNED int     
    int16_t int16amps = PackCurrentf * 10;   // Value x 10, pylon unit is 0.1A  
    CANData356[2] = int16amps;
    CANData356[3] = int16amps >>8;

// TEMPERATURE. 0.1C, 2 bytes, 16 bit SIGNED int  
    int16_t int16temp = Temp_probe_1*10;    // Value  x 10, pylon unit is in 0.1C 
    CANData356[4] = int16temp;
    CANData356[5] = int16temp >>8;

// BATTERY MAX CHARGE VOLTAGE: 0.1V, 16 bit UNSIGNED int
    uint16_t uint16chargevolts = batteryChargeVoltage*10;  // Value x 10, pylon unit is 0.1V
    CANData351[0] = uint16chargevolts;        //  little endian. 
    CANData351[1] = uint16chargevolts>>8;  

// BATTERY CHARGE LIMIT: 0.1A, 16 bit SIGNED int
    uint16_t uint16currentLimit = ChargeCurrentLimit*10;  // Value x 10, pylon unit is 0.1A
    CANData351[2] = uint16currentLimit;        //  little endian. 
    CANData351[3] = uint16currentLimit>>8; 

// BATTERY DISCHARGE LIMIT: 0.1A, 16 bit SIGNED int
    uint16_t uint16dischargeLimit = DischargeCurrentLimit*10;  // Value x 10, pylon unit is 0.1A
    CANData351[4] = uint16dischargeLimit;        //  little endian. 
    CANData351[5] = uint16dischargeLimit>>8;              

}
