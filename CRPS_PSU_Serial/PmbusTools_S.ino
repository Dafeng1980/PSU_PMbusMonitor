#define RAEDADDR 0x07C0

void print_title()
{
  Serial.print(F("\n*****************************************\n"));
  Serial.print(F("* PSU_CRPS   PMbus control demo        *\n"));
  Serial.print(F("* This program demonstrates how to sent *\n"));
  Serial.print(F("* and receive data form  PSU      *\n"));
  Serial.print(F("*****************************************\n"));
}

void print_memu()
    {
              Serial.print(F("\n  1-Read Voltage&Current\n"));
              Serial.print(F("  2-Read All Sensor\n"));
              Serial.print(F("  3-Read All Status\n"));

              Serial.print(F("  5-i2c_PMbus Detect\n"));
              Serial.print(F("  6-Set CRbus Mode\n"));

              Serial.print(F("\n  8-MFR Commands\n"));
              
              Serial.print(F("  0-Mai Menu\n"));
              Serial.println(F("\nEnter command:"));
              delay(500);
    }

void mfr_menu_commands(){
        uint8_t user_command;
        
  do{  
    Serial.print(F("  1-Read MFR Info \n"));
    Serial.print(F("  2-Read MFR FW REVISION\n"));
    Serial.print(F("  3-Read MFR_BLACKBOX\n"));
    Serial.print(F("  4-Clear the 05&06 Cal \n"));
//    Serial.print(F("  5-Margin High\n"));
    Serial.print(F("  6-Margin Low\n"));
    Serial.print(F("  7-Mass Erase the PFlash\n"));
    Serial.print(F("  8-Reads UCD3138 Pflash At 2k Checksum \n"));
    Serial.print(F("  9-Erase&Write UCD3138 Pflash At 2k Checksum \n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));
  
    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                         // Print user command

    switch (user_command)
    {
      case 1:
      getId();
      getModel();
      getRevision();
      getLocation();
      getDate();
      getSerial();
      getFwRev();
      Serial.println(F(" "));
        break;
      case 2:
    //  print_all_sensors();
        break;
      case 3:

        break;
      case 4:
//       Serial.println(F(" "));
//       Serial.println(F("Clear the 05&06 Cal "));
//       Serial.print(F("Press button to continue"));
//       while(digitalRead(kButtonPin) != 0);
//       factoryMode();
//       reset05_06Cal();
//       Serial.println(F("Reset OK "));
//       exitFactoryMode(); 
        Serial.println(F("No Test "));      
        break;
      case 5:

      break;
      
      case 6:
         Serial.println(F(" "));
         Serial.println(F("Erase the PFlash Customize For 1K"));
         Serial.print(F("Press button to continue"));
         while(digitalRead(kButtonPin) != 0);
         Serial.println(F(" "));
        //  for( int i = 3; i<31; i++)
        //  {
        //   ucd3138PageEraseFlashCus(i);
        //  }
        // // ucd3138PageEraseFlashCus(7);
          ucd3138FlashDisplay(0x2000);
          Serial.println(F(" "));
          buzzing();
          ucd3138FlashDisplay(0x8000);
          Serial.println(F(" "));
          buzzing();
          ucd3138FlashDisplay(0xC000);
          Serial.println(F(" "));
          buzzing();
        break;
      case 7:
        Serial.println(F(" "));
        Serial.println(F("Mass Erase the PFlash 0x048000 to 0x04FFFF For 32K"));
        Serial.print(F("Press button to continue"));
        while(digitalRead(kButtonPin) != 0);
        Serial.println(F(" "));
//         ucd3138MassEraseFlash(); 
         buzzing();     
        break;
        
      case 8:
        ucd3138ConfReaAddr(RAEDADDR);
        Serial.println(F(" "));
        Serial.print(F("Configure Read Address at : 0x"));
        Serial.println(RAEDADDR, HEX);
        Serial.println(F(" "));
        delay(10);
        ucd3138Reads();
        break;
        
      case 9:
         Serial.println(F(" "));
         Serial.println(F("Erase&Write the PFlash 0x040400 For 1K"));
         Serial.print(F("Press button to continue"));
          while(digitalRead(kButtonPin) != 0);
          Serial.println(F(" "));
          ucd3138PageEraseFlash();
          delay(100);
          ucd3138Write1k();
          buzzing();
        break;
        
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}

void print_all_volt_curr()
{
  uint16_t w_val;
  float   current,voltage,temp;
     voltage = pmbus->readVin(ps_i2c_address, false);
     current = pmbus->readIin(ps_i2c_address, false);
    Serial.println(F("INPUT: "));
    Serial.print(F("Vol : "));
    Serial.print(voltage, 2);
    Serial.print(F("V"));
    Serial.print(F("     Cur : "));
    Serial.print(current, 2);
    Serial.println(F("A"));
    Serial.println(F(" "));
    voltage = pmbus->readVout(ps_i2c_address, false);
    current = pmbus->readIout(ps_i2c_address, false);
    //Serial.println(F(" "));
    Serial.println(F("OUTPUT: "));
    Serial.print(F("  12V_Vol: "));
    Serial.print(voltage, 3);
    Serial.print(F("V"));
    Serial.print(F("     Cur: "));
    Serial.print(current, 2);
    Serial.println(F("A"));
//    pmbus->setPage(ps_i2c_address,1);    //set Page to 1, read 12Vsb 
//    voltage = pmbus->readVout(ps_i2c_address, false);
//    current = pmbus->readIout(ps_i2c_address, false);
//    Serial.print(F("12Vsb_Vol: "));
//    Serial.print(voltage, 3);
//    Serial.print(F("V"));
//    Serial.print(F("     Cur: "));
//    Serial.print(current, 2);
//    Serial.println(F("A"));
//    pmbus->setPage(ps_i2c_address,0);    
    voltage = pmbus->readPin(ps_i2c_address, false);
    current = pmbus->readPout(ps_i2c_address, false);
    Serial.print(F("Pin: "));
    Serial.print(voltage, 2);
    Serial.print(F("W"));
    Serial.print(F("  Pout: "));
    Serial.print(current, 2);
    Serial.println(F("W"));
    Serial.println(F(" "));
          temp = pmbus->readOtemp(ps_i2c_address);           //temp sensor 0x8D  
          voltage = pmbus->readItemp(ps_i2c_address);        //temp sensor 0x8E  
          current = pmbus->readtemp3(ps_i2c_address);        //temp sensor 0x8F  
          Serial.print(F("Temperature   8D: "));
          Serial.print(temp, 0);
          Serial.print(F("C"));
          Serial.print(F("    8E: "));
          Serial.print(voltage, 0);
          Serial.print(F("C"));
          Serial.print(F("    8F: "));
          Serial.print(current, 0);
          Serial.println(F("C"));
          
          temp = pmbus->readFanSpeed1(ps_i2c_address);
          Serial.print(F("  Fan1 Speed: "));
          Serial.print(temp, 1);
          Serial.println(F(" rpm"));
          Serial.println(F(" "));
    w_val = pmbus->readStatusWord(ps_i2c_address);
    Serial.print(F("STATUS WORD 0x"));
    Serial.println(w_val, HEX);
    
    Serial.print(F("  0B "));
    printBits((w_val & 0xFF));
    Serial.print(F("   LOW: 0x"));
    Serial.println((w_val & 0xFF), HEX);

    Serial.print(F("  0B "));
    printBits((w_val >> 8));
    Serial.print(F("   HIGH: 0x"));
    Serial.println((w_val >> 8), HEX);

    Serial.println(F(" "));
    Serial.println(F(" ************ READ ALL ************ "));
    Serial.println(F(" "));    
}

void print_all_status()
{
    uint16_t w_val;
    uint8_t msb,lsb,io,in,tm,fa,vo;
     Serial.println(F("========= READ ALL STATUS =========="));
     Serial.println(F(" "));
      w_val = pmbus->readStatusWord(ps_i2c_address);
     Serial.print(F("STATUS WORD 0x"));
    Serial.println(w_val, HEX);
    msb = w_val >> 8;
    lsb = w_val & 0xFF;
    Serial.print(F(" 0B "));
    printBits(msb);
    Serial.print(F("  HIGH: 0x"));
    Serial.print(msb, HEX);

    Serial.print(F("     0B "));
    printBits(lsb);
    Serial.print(F("   LOW: 0x"));
    Serial.println(lsb, HEX);
    if(msb & 0x80){
        vo = pmbus -> readStatusVout(ps_i2c_address);
        Serial.print(F("STATUS_VOUT 0B "));
        printBits(vo);
        Serial.print(F("    : 0x"));
        Serial.println(vo, HEX);
        if(vo & 0x80)
        Serial.println(F("STATUS_VOUT_OV_FAULT !! "));
        if(vo & 0x10)
        Serial.println(F("STATUS_VOUT_UV_FAULT !! "));
     
    }

    if(msb & 0x40){
        io = pmbus -> readStatusIout(ps_i2c_address);
        Serial.print(F("STATUS_IOUT 0B "));
        printBits(io);
        Serial.print(F("    : 0x"));
        Serial.println(io, HEX);
        if(io & 0x80)
        Serial.println(F("STATUS_IOUT_OC_FAULT !! "));
        if(io & 0x20)
        Serial.println(F("STATUS_IOUT_OC_WARNING !! "));
        if(io & 0x02)
        Serial.println(F("STATUS_POUT_OP_FAULT !! "));
        if(io & 0x01)
        Serial.println(F("STATUS_POUT_OP_WARNING !! ")); 
    }

    if(msb & 0x20){
        in = pmbus -> readStatusInput(ps_i2c_address);
        Serial.print(F("STATUS_INPUT 0B "));
        printBits(in);
        Serial.print(F("    : 0x"));
        Serial.println(in, HEX);
        if(in & 0x20)
        Serial.println(F("STATUS_VIN_UV_WARNING !! "));
        if(in & 0x10)
        Serial.println(F("STATUS_VIN_UV_FAULT !! "));
        if(in & 0x08)
        Serial.println(F("STATUS_UNIT_OFF_FOR_INSUFFICIENT_INPUT !! "));
        if(in & 0x02)
        Serial.println(F("STATUS_IIN_OVER_CURRENT_WARNING !! ")); 
        if(in & 0x01)
        Serial.println(F("STATUS_PIN_OVER_POWER_WARNING !! ")); 
    }

    if (msb & 0x08)  Serial.println(F("STATUS_POWER_GOOD#_FAULT !! "));

    if(msb & 0x04){
      fa = pmbus -> readStatusFan(ps_i2c_address);
        Serial.print(F("STATUS_FAN 0B "));
        printBits(fa);
        Serial.print(F("    : 0x"));
        Serial.println(fa, HEX);
        if(fa & 0x80)
        Serial.println(F("STATUS_FAN_1_FAULT !! "));
        if(fa & 0x40)
        Serial.println(F("STATUS_FAN_2_FAULT !! "));
        if(fa & 0x20)
        Serial.println(F("STATUS_FAN_1_WARNING !! "));
        if(fa & 0x10)
        Serial.println(F("STATUS_FAN_1_WARNING !! "));

    }

    if(lsb & 0x40)  Serial.println(F("STATUS_PS_OFF !! "));

    if(lsb & 0x10){
        io = pmbus -> readStatusIout(ps_i2c_address);
        Serial.print(F("STATUS_IOUT 0B "));
        printBits(io);
        Serial.print(F("    : 0x"));
        Serial.println(io, HEX);
        if(io & 0x80)
        Serial.println(F("STATUS_IOUT_OC_FAULT !! "));
        if(io & 0x20)
        Serial.println(F("STATUS_IOUT_OC_WARNING !! "));
        if(io & 0x02)
        Serial.println(F("STATUS_POUT_OP_FAULT !! "));
        if(io & 0x01)
        Serial.println(F("STATUS_POUT_OP_WARNING !! ")); 

    }
    
    if(lsb & 0x08){
        in = pmbus -> readStatusInput(ps_i2c_address);
        Serial.print(F("STATUS_INPUT 0B "));
        printBits(in);
        Serial.print(F("    : 0x"));
        Serial.println(in, HEX);
        if(in & 0x20)
        Serial.println(F("STATUS_VIN_UV_WARNING !! "));
        if(in & 0x10)
        Serial.println(F("STATUS_VIN_UV_FAULT !! "));
        if(in & 0x08)
        Serial.println(F("STATUS_UNIT_OFF_FOR_INSUFFICIENT_INPUT !! "));
        if(in & 0x02)
        Serial.println(F("STATUS_IIN_OVER_CURRENT_WARNING !! ")); 
        if(in & 0x01)
        Serial.println(F("STATUS_PIN_OVER_POWER_WARNING !! ")); 

    }

    if(lsb & 0x04){
        tm = pmbus -> readStatusTemp(ps_i2c_address);
        Serial.print(F("STATUS_TEMPERATURE 0B "));
        printBits(tm);
        Serial.print(F("    : 0x"));
        Serial.println(tm, HEX);
        if(tm & 0x80)
        Serial.println(F("STATUS_OT_FAULT !! "));
        if(tm & 0x40)
        Serial.println(F("STATUS_OT_WARNING !! "));
    }

    if(lsb & 0x02)
    Serial.println(F("STATUS_CML_FAULT !! "));

    Serial.println(F(" "));
}

void print_all_sensors()
  {
      float temp,fspeed;
      Serial.println(F("========= READ ALL SENSOR =========="));
      Serial.println(F(" "));
        temp = pmbus->readOtemp(ps_i2c_address);           //temp sensor 0x8D  Inlet Air
          Serial.print(F("Inlet Air Temp: "));
          Serial.print(temp, 0);
          Serial.println(F(" C"));
           temp = pmbus->readItemp(ps_i2c_address);            //0x8E Hotspot1 Sensor located on secondary side of DC/DC stage
            Serial.print(F("DC-DC stage Temp: "));
            Serial.print(temp, 0);
            Serial.println(F(" C"));
              temp = pmbus->readtemp3(ps_i2c_address);          // 0x8F Hotspot2 Sensor located on primary heat sink
                 Serial.print(F("P-heatsink Temp: "));
                 Serial.print(temp, 0);
                 Serial.println(F(" C"));
                  temp = pmbus->readtemp1(ps_i2c_address);
                    Serial.print(F("MFR-D3 Temp: "));
                    Serial.print(temp, 0);
                    Serial.print(F(" C"));
                      temp = pmbus->readtemp2(ps_i2c_address);
                        Serial.print(F("    MFR-D4 Temp: "));
                        Serial.print(temp, 0);
                        Serial.println(F(" C"));
                          fspeed = pmbus->readFanSpeed1(ps_i2c_address);
                            Serial.print(F("Fan1 Speed:  "));
                            Serial.print(fspeed, 0);
                            Serial.println(F(" rpm"));
                             Serial.println(F(" "));
          }

void buzzing(){
         tone(kBuzzerPin, 2200);
          delay(50);
           noTone(kBuzzerPin);
    }

void alarm(){
      for (int i = 0; i < 50; i++){
              tone(kBuzzerPin, 2100);
              delay(100);
              noTone(kBuzzerPin);
              delay(200);            
        }
}

float readeout(){
  uint8_t data[6];
 // unsigned int m,b,r;
  unsigned long maxValue,lastEnergyCount,lastSampleCount,currentEnergyCount,currentSampleCount;
  float eoutResult;
//  smbus -> readBlock(ps_i2c_address, 0x30, data, 5);
//      m = data[1] * 256 + data[0];
//      b = data[3] * 256 + data[2];
//      r = data[4];
//      maxValue = (m * 32767 + b)* pow(10,r);
      maxValue = 32767;
    smbus -> readBlock(ps_i2c_address, 0x87, data, 6);             //EOUT 0x87
    lastEnergyCount = data[0] + data[1]*0x100 + data[2]*maxValue;
    lastSampleCount = data[3] + data[4]*0x100 + data[5]*0x10000;
      delay(1000);
      smbus -> readBlock(ps_i2c_address, 0x87, data, 6);
      currentEnergyCount = data[0] + data[1]*0x100 + data[2]*maxValue;
      currentSampleCount = data[3] + data[4]*0x100 + data[5]*0x10000;
        Serial.print(F("SampleCount "));
        Serial.println(currentSampleCount - lastSampleCount);
    eoutResult = (float)(currentEnergyCount - lastEnergyCount)/(float)(currentSampleCount - lastSampleCount);
    return eoutResult; 
}

void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}

void i2cdetects(uint8_t first, uint8_t last) {
  uint8_t i, address, error;
  char buff[10];
  // table header
  Serial.print("   ");
  for (i = 0; i < 16; i++) {
    //Serial.printf("%3x", i);
    sprintf(buff, "%3x", i);
    Serial.print(buff);
  }

  // table body
  // addresses 0x00 through 0x77
  for (address = 0; address <= 127; address++) {
    if (address % 16 == 0) {
      //Serial.printf("\n%#02x:", address & 0xF0);
      sprintf(buff, "\n%02x:", address & 0xF0);
      Serial.print(buff);
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (error == 0) {
        // device found
        //Serial.printf(" %02x", address);
        sprintf(buff, " %02x", address);
        Serial.print(buff);
      } else if (error == 4) {
        // other error
        Serial.print(" XX");
      } else {
        // error = 2: received NACK on transmit of address
        // error = 3: received NACK on transmit of data
        Serial.print(" --");
      }
    } else {
      // address not scanned
      Serial.print("   ");
    }
  }
  Serial.println("\n");
}

void printFru(uint8_t first, uint8_t last, uint8_t *values) {
        int i,address;
        Serial.print("    ");
        for (i = 0; i < 16; i++) {
                Serial.printf("%3x", i);
//                sprintf(buff, "%3x", i);
//                Serial.print(buff);
            }
        for (address = 0; address <= 255; address++) {   
          if (address % 16 == 0) {
//              Serial.printf("\n%#02x:", address & 0xF0);
                Serial.printf("\n%02xh:", address & 0xF0);
//              sprintf(buff, "\n%02x:", address & 0xF0);
//              Serial.print(buff);
              }
              
            if (address >= first && address <= last)
                Serial.printf(" %02x", values[address]);
//                sprintf(buff, " %02x", address);
//                Serial.print(buff);
           else Serial.print("   ");
            }
      Serial.println("\n");
}

void iOutFan()
        {
           float fspeed, current;
            current = pmbus->readIout(ps_i2c_address, false);
            fspeed = pmbus->readFanSpeed1(ps_i2c_address);
            Serial.print(F("Fan1 Speed:  "));
            Serial.print(fspeed, 0);
            Serial.println(F(" rpm"));
            Serial.println(F(" ")); 
            Serial.print(F("IOUT: "));
            Serial.print(current, 2);
            Serial.println(F("A")); 
            Serial.println(F(" "));        
        }
