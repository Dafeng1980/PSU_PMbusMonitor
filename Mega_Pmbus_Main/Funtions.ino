
void print_memu()
        {                   
           Serial.print(F("\n Pmbus Program For CRPS "));
           Serial.print(F(" To Read The Voltage Current Sensor Status \n"));
           Serial.print(F(" Set the CRbus Mode, Detect PMbus Device etc.\n"));
           Serial.println(F("\nEnter command:"));
           printhelp();
           delay(500);
        }

void printhelp(){
      Serial.print(F("Here are commands can be used For XXX Model.\r\n"));
      Serial.print(F(" 1 > Moitor All Status and Show \r\n "));
      Serial.print(F(" 2 > Set Fan Full Speed  \r\n "));
      Serial.print(F(" 3 > Set CRbus Mode \r\n "));
      Serial.print(F(" 4 > Read MFR Revisions \r\n "));
      Serial.print(F(" 5 > Detect Devices in PMbus \r\n "));
      Serial.print(F(" 6 > TBD \r\n "));
      Serial.print(F(" 7 > TBD \r\n "));
      Serial.print(F(" 8 > Enter MFR Commands \r\n "));
      Serial.print(F(" 9 > Enable/Disable PEC \r\n "));
      Serial.print(F(" 0 > Set to Default \r\n "));
      Serial.print(F(" h > Help Display \r\n "));
      delay(2000);  
}


void mfr_menu_commands(){
        uint8_t user_command;       
  do{  
    Serial.print(F("  1-Read MFR Info \n"));
    Serial.print(F("  2-Read MFR FW REVISION\n"));
    Serial.print(F("  3-Read MFR_BLACKBOX\n"));
    Serial.print(F("  4-Clear the 05&06 Cal \n"));
    Serial.print(F("  5-Margin High\n"));
    Serial.print(F("  6-Margin Low\n"));
    Serial.print(F("  7-Mass Erase the PFlash\n"));
    Serial.print(F("  8-Reads UCD3138 Pflash At 2k_32K_64K Checksum \n"));
    Serial.print(F("  9-Erase&Write UCD3138 Pflash At 2k Checksum \n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));  
    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')   Serial.print(F("m\n"));     // Print m if it is entered
    else  Serial.println(user_command);                    // Print user command     
    switch (user_command)
    {
      case 1:
//      getId();
//      getModel();
//      getRevision();
//      getLocation();
//      getDate();
//      getSerial();
      Serial.println(F(" "));
        break;
      case 2:
      Serial.println(F("TBD "));
        break;
      case 3:
      Serial.println(F("TBD "));
        break;
      case 4:
      Serial.println(F("TBD "));     
        break;
      case 5:
      Serial.println(F("TBD "));
      break;      
      case 6:
      Serial.println(F("No Test "));
        break;        
      case 7:
        Serial.println(F(" "));
        Serial.println(F("Mass Erase all the PFlash 0x040000 to 0x04FFFF For 64K"));
        Serial.print(F("Press button to continue"));
        while(digitalRead(kButtonPin) != 0);
        Serial.println(F(" "));
         ucd3138MassEraseFlash(0x01);     //Pflash Block0 32k
         delay(30);
         ucd3138MassEraseFlash(0x02);     //Pflash Block1 32k
         delay(30); 
         buzzing();     
        break;        
      case 8:
         Serial.println(F(" "));
         Serial.println(F("Display the PFlash checksum_2k_32k_64k"));
         Serial.println(F(" "));
          ucd3138FlashDisplay(0x0700);
          Serial.println(F(" "));
          buzzing();
          ucd3138FlashDisplay(0x7F00);
          Serial.println(F(" "));
          buzzing();
          ucd3138FlashDisplay(0xFF00);
          Serial.println(F(" "));
          buzzing();
        break;        
      case 9:
         Serial.println(F(" "));
         Serial.println(F("Erase&Write the PFlash Block0 Page 1(0x040400) For 2K checksum "));
         Serial.print(F("Press button to continue"));
          while(digitalRead(kButtonPin) != 0);
          Serial.println(F(" "));
          ucd3138Blocl0PageErase(0x01);  //Erase Pflash Block0 Page 1;
          delay(25);
          ucd3138Write2kChecksum();
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


void printpmbusData(struct PowerPmbus busData)
{
    ledflash();
    Serial.println(F("========== PMBUS DATA =========="));
    Serial.println(" ");
    Serial.print(F("PMBUS ADDRESS: 0x"));    //F function enable to decrease sram usage
    Serial.println(busData.i2cAddr, HEX);
    Serial.println(F(" "));
    Serial.print(F("V_IN: "));
    Serial.print(busData.inputV, 2);
    Serial.print(F("V"));
    Serial.print(F("      C_IN: "));
    Serial.print(busData.inputA, 3);
    Serial.print(F("A"));
    Serial.print(F("    P_IN: "));
    Serial.print(busData.inputP, 2);
    Serial.println(F("W"));
    Serial.println(F(" "));     
    Serial.print(F("V_OT: "));
    Serial.print(busData.outputV, 3);
    Serial.print(F("V"));
    Serial.print(F("   C_OT: "));
    Serial.print(busData.outputA, 3);
    Serial.print(F("A"));
    Serial.print(F("   P_OT: "));
    Serial.print(busData.outputP, 2);
    Serial.println(F("W"));
    Serial.println(F(" "));   
    
    Serial.print(F("TEMP_8D: "));
    Serial.print(busData.temp1, 0);
    Serial.println(F("C"));
    Serial.println(F(" "));
          
    Serial.print(F("STATUS WORD: 0x"));
   // Serial.println(busData.statusWord, HEX);
    Serial.printf("%04x\n", busData.statusWord);    
    Serial.print(F(" 0B "));
    printBits((busData.statusWord & 0xFF));
    Serial.print(F("    LOW: 0x"));
    Serial.printf("%02x\n", busData.statusWord & 0xFF);
    Serial.print(F(" 0B "));
    printBits((busData.statusWord >> 8));
    Serial.print(F("   HIGH: 0x"));
    Serial.printf("%02x\n", busData.statusWord >> 8);
    Serial.println(F(" "));  
}

void pmbusStatus()
{
    uint16_t w_val;
    uint8_t msb,lsb,io,in,tm,fa,vo,cm;
    Serial.println(F("========= READ ALL STATUS =========="));
    Serial.println(F(" "));
    w_val = pd.statusWord;
    Serial.print(F("STATUS WORD: 0x"));
    Serial.printf("%04x\n", w_val);
    msb = w_val >> 8;
    lsb = w_val & 0xFF;
    Serial.print(F("0B "));
    printBits(msb);
    Serial.print(F("  HIGH: 0x"));
    Serial.printf("%02x", msb);
    Serial.print(F("     0B "));
    printBits(lsb);
    Serial.print(F("   LOW: 0x"));
    Serial.printf("%02x\n", lsb);
   
    if(msb & 0x80){
        vo = pmbus_readStatusVout(ps_i2c_address);
        Serial.print(F("STATUS_VOUT 0B "));
        printBits(vo);
        Serial.print(F("    : 0x"));
        Serial.printf("%02x\n", vo);
        if(vo & 0x80)
        Serial.println(F("STATUS_VOUT_OV_FAULT !! "));
        if(vo & 0x10)
        Serial.println(F("STATUS_VOUT_UV_FAULT !! "));     
    }

    if(msb & 0x40){
        io = pmbus_readStatusIout(ps_i2c_address);
        Serial.print(F("STATUS_IOUT 0B "));
        printBits(io);
        Serial.print(F("    : 0x"));
        Serial.printf("%02x\n", io);
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
        in = pmbus_readStatusInput(ps_i2c_address);
        Serial.print(F("STATUS_INPUT 0B "));
        printBits(in);
        Serial.print(F("    : 0x"));
        Serial.printf("%02x\n", in);
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

    if(msb & 0x08)  Serial.println(F("STATUS_POWER_GOOD#_FAULT !! "));

    if(msb & 0x04){
      fa = pmbus_readStatusFan(ps_i2c_address);
        Serial.print(F("STATUS_FAN 0B "));
        printBits(fa);
        Serial.print(F("    : 0x"));
        Serial.printf("%02x\n", fa);
        if(fa & 0x80)
        Serial.println(F("STATUS_FAN_1_FAULT !! "));
        if(fa & 0x40)
        Serial.println(F("STATUS_FAN_2_FAULT !! "));
        if(fa & 0x20)
        Serial.println(F("STATUS_FAN_1_WARNING !! "));
        if(fa & 0x10)
        Serial.println(F("STATUS_FAN_1_WARNING !! "));
    }

    if(msb & 0x02)  Serial.println(F("STATUS_OTHERS_WARNING !! "));   
    if(lsb & 0x40)  Serial.println(F("STATUS_PS_OFF !! "));
    if(lsb & 0x20)  Serial.println(F("STATUS_MAIN_OUTPUT_OV_FAULT !! "));

    if(lsb & 0x10){
        io = pmbus_readStatusIout(ps_i2c_address);
        Serial.print(F("STATUS_IOUT 0B "));
        printBits(io);
        Serial.print(F("    : 0x"));
        Serial.printf("%02x\n", io);
        if(io & 0x80)
        Serial.println(F("STATUS_IOUT_OC_FAULT !! "));
        if(io & 0x40)
        Serial.println(F("STATUS_IOUT_OC_LV_FAULT !! "));  //When low voltage due to OC is detected.
        if(io & 0x20)
        Serial.println(F("STATUS_IOUT_OC_WARNING !! "));
        if(io & 0x08)
        Serial.println(F("STATUS_ISHARE_FAULT !! "));
        if(io & 0x02)
        Serial.println(F("STATUS_POUT_OP_FAULT !! "));
        if(io & 0x01)
        Serial.println(F("STATUS_POUT_OP_WARNING !! ")); 

    }
    
    if(lsb & 0x08){
        in = pmbus_readStatusInput(ps_i2c_address);
        Serial.print(F("STATUS_INPUT 0B "));
        printBits(in);
        Serial.print(F("    : 0x"));
        Serial.printf("%02x\n", in);
        if(in & 0x80)
        Serial.println(F("STATUS_VIN_OV_FAULT !! "));
        if(in & 0x40)
        Serial.println(F("STATUS_VIN_OV_WARNING !! "));
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
        tm = pmbus_readStatusTemp(ps_i2c_address);
        Serial.print(F("STATUS_TEMPERATURE 0B "));
        printBits(tm);
        Serial.print(F("    : 0x"));
        Serial.printf("%02x\n", tm);
        if(tm & 0x80)
        Serial.println(F("STATUS_OT_FAULT !! "));
        if(tm & 0x40)
        Serial.println(F("STATUS_OT_WARNING !! "));
    }
    
    if(lsb & 0x02){
      cm = pmbus_readStatusCml(ps_i2c_address);
      Serial.print(F("STATUS_CML 0B "));
      printBits(cm);
      Serial.print(F("    : 0x"));
      Serial.printf("%02x\n", cm);
      if(cm & 0x80)
      Serial.println(F("STATUS_CML_InvalidCMD !! "));
      if(cm & 0x40)
      Serial.println(F("STATUS_CML_InvalidData !! "));
      if(cm & 0x20)
      Serial.println(F("STATUS_CML_PEC_Fault !! "));
      if(cm & 0x08) 
      Serial.println(F("STATUS_CML_Processor_Fault !! "));
      if(cm & 0x02) 
      Serial.println(F("STATUS_CML_COMM_Fault !! "));
      if(cm & 0x01) 
      Serial.println(F("STATUS_CML_MEM_Logic_Fault !! "));
    }
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
  //char buff[10];
  // table header
  Serial.print("   ");
  for (i = 0; i < 16; i++) {
    Serial.printf("%3x", i);
//    sprintf(buff, "%3x", i);
//    Serial.print(buff);
  }
  // table body
  // addresses 0x00 through 0x77
  for (address = 0; address <= 127; address++) {
    if (address % 16 == 0) {
      Serial.printf("\n0x%02x:", address & 0xF0);
//      sprintf(buff, "\n%02x:", address & 0xF0);
//      Serial.print(buff);
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      delay(5);
      if (error == 0) {
        // device found
        Serial.printf(" %02x", address);
//        sprintf(buff, " %02x", address);
//        Serial.print(buff);
      } else if (error == 4) {    // other error      
        Serial.print(" XX");
      } else {                   // error = 2: received NACK on transmit of address              
        Serial.print(" --");    // error = 3: received NACK on transmit of data
      }
    } else {                 // address not scanned      
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
            }
        for (address = 0; address <= 255; address++) {   
          if (address % 16 == 0) {
//              Serial.printf("\n%#02x:", address & 0xF0);
                Serial.printf("\n%02xh:", address & 0xF0);
              }
              
            if (address >= first && address <= last)
                Serial.printf(" %02x", values[address]);
           else Serial.print("   ");
            }
      Serial.println("\n");
}

void printchar(uint8_t *values, uint8_t bsize){
  for(int i = 0; i < bsize; i++){
    Serial.printf(" %02x", values[i]);
  }
   Serial.println();
}

void serialread(){
  char readval;
  if (Serial.available())                //! Serial input read
  {
      readval = read_char();                     //! Reads the user command
    if ((char)readval == '0')      key = 0;
    else if ((char)readval == '1') key = 1;
    else if ((char)readval == '2') key = 2;
    else if ((char)readval == '3') key = 3;
    else if ((char)readval == '4') key = 4;
    else if ((char)readval == '5') i2cdetects(0x00, 0x7F);    
    else if ((char)readval == '6') key = 6;
    else if ((char)readval == '7') key = 7;
    else if ((char)readval == '8') mfr_menu_commands();
    else if ((char)readval == '9') pecstatus();
    else if ((char)readval == 'h'  || (char)readval == 'H') printhelp();
    else {
      Serial.println(F("unknown command"));
      Serial.println(F("type \'h\' for help"));
      key = 0;       
    }
    Serial.printf("\n Key= %#01d:\n", key);
 }
}

void pmbusdetects(){
  uint8_t i, address, error;
  // table header
  Serial.print("   ");
  for (i = 8; i < 16; i++) {
    Serial.printf("%3x", i);
  }
      Serial.printf("\n%#02x:", 0x50);
  for (address = 88; address <= 95; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
    //  if (address == 92) error = 0;
      if (error == 0) {
        // device found
        n++;
        if(n == 1) ps_i2c_address = address;
        else ps_i2c_address = PS_I2C_ADDRESS;
        if(n == 2) ps_patner_address = address;
        else ps_patner_address = PS_PARTNER_ADDRESS;
        Serial.printf(" %02x", address);       
      } else if (error == 4) {
        // other error
        Serial.print(" XX");
      } else {
        // error = 2: received NACK on transmit of address
        // error = 3: received NACK on transmit of data
        Serial.print(" --");
        delay(20);
      } 
  }
  Serial.println("\n");
  Serial.println(n);
}

bool readpmbusdata()
{   
      bool ret = true;
  if(smbus_waitForAck(ps_i2c_address, 0x00) == 0) {  //0x00 PAGE read
      Serial.println("PMBUS Polling Fail \n");      
      return ret = false;
    }
     pd.i2cAddr = ps_i2c_address;
     pd.inputV = pmbus_readVin(ps_i2c_address);
     pd.inputA = pmbus_readIin(ps_i2c_address);
     pd.outputV = pmbus_readVout(ps_i2c_address);
     pd.outputA = pmbus_readIout(ps_i2c_address);
     pd.inputP = pmbus_readPin(ps_i2c_address);
     pd.outputP = pmbus_readPout(ps_i2c_address);
     pd.temp1 = pmbus_readOtemp(ps_i2c_address);           //temp sensor 0x8D  
     pd.temp2 = pmbus_readItemp(ps_i2c_address);        //temp sensor 0x8E  
     pd.temp3 = pmbus_readMtemp(ps_i2c_address);        //temp sensor 0x8F  
     pd.fanSpeed = pmbus_readFanSpeed1(ps_i2c_address);
     pd.statusWord = pmbus_readStatusWord(ps_i2c_address); 
     pmbus_setPage(ps_i2c_address,1);                    //set Page to 1, read 12Vsb 
     pd.outputVsb = pmbus_readVout(ps_i2c_address);
     pd.outputAsb = pmbus_readIout(ps_i2c_address);
     pmbus_setPage(ps_i2c_address,0); 
     delay(20);     
}

void checkButton(){
    if(digitalRead(kButtonPin) == 0 && buttonflag){
          delay(10);
        if(digitalRead(kButtonPin) == 0){
              buzzing();
              key++;
              if (key >= 5) key = 0;
              Serial.printf("\n Key= %#01d:\n", key);
              buttonflag = false;
        }
    }
}

void ledflash(){
  ledstatus = !ledstatus;
  if(ledstatus) digitalWrite(kLedPin, HIGH);
  else digitalWrite(kLedPin, LOW);
}

void pecstatus(){
    pecflag = !pecflag;
    if(pecflag) Serial.print(F("PEC ENABLE\n"));
    else Serial.print(F("PEC Disable\n"));
    delay(200);
}

void monitorstatus(){
  statusflag = !statusflag;
  if(pecflag) Serial.print(F("Status Monitor Enable\n"));
  else Serial.print(F("Status Monitor Disable\n"));
  }

 void pmbus_devices_detect(){
   // scani2c = false;
  i2cdetects(0x00, 0x7F);
  if (digitalRead(kButtonPin) == 0){
        delay(100);
        if(digitalRead(kButtonPin) == 0){
          buzzing();
          scani2c = false;
    }
  }
 while(scani2c){
    digitalWrite(kLedPin, HIGH);
    pmbusdetects();
    delay(50);
    digitalWrite(kLedPin, LOW);
    delay(210);
    if(n > 0) break;
    }
 }

void m24c32Checksum(){
    uint16_t checksum;
    eepromreadbytes(m24c32_address, 0, 128, eepbuffer);
    eepromreadbytes(m24c32_address, 128, 128, eepbuffer+128);       
    checksum = calcCheckSum(eepbuffer, 192);
    Serial.printf("EEPROM_CALC_CheckSum: 0x%04x \n", checksum);
    Serial.printf("EEPROM_READ_CheckSum: 0x%02x%02x \n", eepbuffer[190], eepbuffer[191]);
    printFru(0, 0xFF , eepbuffer);   
}

uint16_t calcCheckSum (uint8_t *pBuffer, uint16_t len)
 {
     uint16_t sum;
     int i;  
     sum = 0;
     for (i = 0; i < len; i++) {
         sum += pBuffer[i];
     }  
     sum = 0x00FF & (~sum + 1);  
     return (sum);
 }
