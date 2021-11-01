
void print_memu()
        {                   
           Serial.print(F("PSU PMbus Debug Tools (Smbus Protocol)\n"));
           Serial.print(F("Monitoring PSU Status etc. and Detect i2c Devices\n"));
           Serial.print(F("Send the Smbus Protocol Command.\n"));
           Serial.println(F("\nEnter command:"));
           printhelp();
           Serial.println(F(" "));
        }

void printhelp(){
      Serial.print(F("\n Here are commands can be used.\r\n "));
      Serial.print(F(" 1 > Moitor the PSU Status Word \r\n "));
      Serial.print(F(" 2 > Set Standby/(Other Page) \r\n "));
      Serial.print(F(" 3 > TBD \r\n "));
      Serial.print(F(" 4 > TBD \r\n "));
      Serial.print(F(" 5 > Detect PSU Address in PMbus. \r\n "));
      Serial.print(F(" 6 > TBD \r\n "));
      Serial.print(F(" 7 > TBD \r\n "));
      Serial.print(F(" 8 > Enter MFR Commands \r\n "));
      Serial.print(F(" 9 > Enable/Disable PEC \r\n "));
      Serial.print(F(" 0 > Set All to Default \r\n "));
      Serial.print(F(" a > Set the PUS Model \r\n "));
      Serial.print(F(" e > Set the PMbus Interval Timing \r\n "));
      Serial.print(F(" c > Enter The SMbus Protocol Commands & Read EEPROM \r\n "));
      Serial.print(F(" r > Set the PMbus/SMbus Address \r\n "));
      Serial.print(F(" h > Help \r\n "));
      Serial.println("");
      Serial.print("Enter a command: ");      
      delay(500);  
}

void mfr_menu_commands(){
        uint8_t user_command;       
  do{  
    Serial.print(F("  1-Read MFR Info \n"));
    Serial.print(F("  2-Clear The Faults \n"));
    Serial.print(F("  3-TBD \n"));
    Serial.print(F("  4-TBD \n"));
    Serial.print(F("  5-Margin High \n"));
    Serial.print(F("  6-Margin Low \n"));
    Serial.print(F("  7-Mass Erase the PFlash \n"));
    Serial.print(F("  8-Reads UCD3138 Pflash At 2k_32K_64K Checksum \n"));
    Serial.print(F("  9-Erase&Write UCD3138 Pflash At 2k Checksum \n"));
    Serial.print(F("  m-Main Menu \n"));
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
       
    if(standbyflag){
    Serial.print(F("V_Standby: "));
    Serial.print(busData.outputVsb, 3);
    Serial.print(F("V"));
    Serial.print(F("    C_Standby: "));
    Serial.print(busData.outputAsb, 3);
    Serial.println(F("A"));
    Serial.println(F(" "));
    }
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
    
    if(unitname == 1) {
    Serial.print(F("TEMP_8D: "));
    Serial.print(busData.temp1, 0);
    Serial.println(F("C"));      
    }
    else if(unitname == 2) {
      Serial.print(F("TEMP_8D: "));
      Serial.print(busData.temp1, 0);
      Serial.print(F("C")); 
      Serial.print(F("TEMP_8D: "));
      Serial.print(F("TEMP_8D: "));
      Serial.print(busData.temp1, 0);
      Serial.print(F("C"));
      Serial.print(F("  TEMP_8E: "));
      Serial.print(busData.temp2, 0);
      Serial.print(F("C"));
      Serial.print(F("  TEMP_8F: "));
      Serial.print(busData.temp3, 0);
      Serial.println(F("C"));
      Serial.print(F("FANSPEED_0x90: "));
      Serial.print(busData.fanSpeed, 0);
      Serial.println(F("rpm"));
      Serial.println(F(" "));
    }

    else if(unitname == 3) {
      
      Serial.print(F("TEMP_8D: "));
      Serial.print(busData.temp1, 0);
      Serial.print(F("C")); 
      Serial.print(F("TEMP_8D: "));
      Serial.print(F("TEMP_8D: "));
      Serial.print(busData.temp1, 0);
      Serial.print(F("C"));
      Serial.print(F("  TEMP_8E: "));
      Serial.print(busData.temp2, 0);
      Serial.print(F("C"));
      Serial.print(F("  TEMP_8F: "));
      Serial.print(busData.temp3, 0);
      Serial.println(F("C"));
      Serial.print(F("FANSPEED_0x90: "));
      Serial.print(busData.fanSpeed, 0);
      Serial.println(F("rpm"));
      Serial.println(F(" "));
    }
    
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
  Serial.print("   ");            // table header
  for (i = 0; i < 16; i++) {
    Serial.printf("%3x", i);
//    sprintf(buff, "%3x", i);
//    Serial.print(buff);
  }
  for (address = 0; address <= 127; address++) {             // addresses 0x00 through 0x77
    if (address % 16 == 0) {                            // table body
      Serial.printf("\n0x%02x:", address & 0xF0);
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      delay(10);
      if (error == 0) {                           // device found
        Serial.printf(" %02x", address);
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
   Serial.println(F(" "));
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
    else if ((char)readval == '5') i2cdetectsstatus();    
    else if ((char)readval == '6') key = 6;
    else if ((char)readval == '7') key = 7;
    else if ((char)readval == '8') mfr_menu_commands();
    else if ((char)readval == '9') pecstatus();
    else if ((char)readval == 'a'  || (char)readval == 'A') setModel(); 
    else if ((char)readval == 'c'  || (char)readval == 'C') smbus_commands();
    else if ((char)readval == 'e'  || (char)readval == 'E') setIntervaltime();   
    else if ((char)readval == 'r'  || (char)readval == 'R') reset_address();
    else if ((char)readval == 'h'  || (char)readval == 'H') {
      printhelp();
      delay(1500);
      while (!digitalRead(kButtonPin)) {};  // wait for button unpress
    }
    else {
      Serial.println(F("unknown command"));
      Serial.println(F("type 'h' for help"));
      key = 0;       
    }
    Serial.printf("\nKey= %#01d:\n", key);
 }
}

void pmbusdetects() {
  uint8_t i, address, error;
  Serial.print("   ");                   // table header
  for (i = 8; i < 16; i++) {
    Serial.printf("%3x", i);
  }
      Serial.printf("\n%#02x:", 0x50);
  for (address = 88; address <= 95; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
    //  if (address == 92) error = 0;
      if (error == 0) {                  // device found        
        n++;
        if(n == 1) ps_i2c_address = address;
        else ps_i2c_address = PS_I2C_ADDRESS;
        if(n == 2) ps_patner_address = address;
        else ps_patner_address = PS_PARTNER_ADDRESS;
        Serial.printf(" %02x", address);       
      } else if (error == 4) {         // other error        
        Serial.print(" XX");
      } else {                             // error = 2: received NACK on transmit of address        
        Serial.print(" --");              // error = 3: received NACK on transmit of data
        delay(20);
      } 
  }
  Serial.println("\n");
  Serial.println(n);
}

bool readpmbusdata()
{   
      bool ret = true;
      if(!pmbusflag) return ret = false;
      if(smbus_waitForAck(ps_i2c_address, 0x00) == 0) {  //0x00 PAGE read
          Serial.println("PMBUS Polling Fail, Type \'h\' for Help \n");      
          return ret = false;
        }
     pd.i2cAddr = ps_i2c_address;
     pd.inputV = pmbus_readVin(ps_i2c_address);
     pd.inputA = pmbus_readIin(ps_i2c_address);
     pd.outputV = pmbus_readVout(ps_i2c_address);
     pd.outputA = pmbus_readIout(ps_i2c_address);
     pd.inputP = pmbus_readPin(ps_i2c_address);
     pd.outputP = pmbus_readPout(ps_i2c_address);
     pd.statusWord = pmbus_readStatusWord(ps_i2c_address);
     
     if(unitname > 0)  {
     pd.temp1 = pmbus_readOtemp(ps_i2c_address);           //temp sensor 0x8D
     if(unitname == 2) {  
     pd.temp2 = pmbus_readItemp(ps_i2c_address);        //temp sensor 0x8E  
     pd.temp3 = pmbus_readMtemp(ps_i2c_address);        //temp sensor 0x8F  
     pd.fanSpeed = pmbus_readFanSpeed1(ps_i2c_address);
     }    
     if(standbyflag){
     pmbus_setPage(ps_i2c_address,1);                    //set Page to 1, read 12Vsb 
     pd.outputVsb = pmbus_readVout(ps_i2c_address);
     pd.outputAsb = pmbus_readIout(ps_i2c_address);
     pmbus_setPage(ps_i2c_address,0);
     }
   }      
     delay(20);     
}

void checkButton(){
    if(digitalRead(kButtonPin) == 0 && buttonflag){
          delay(10);
        if(digitalRead(kButtonPin) == 0){
              buzzing();
              key++;
              if (key >= 5) key = 0;
              if(scani2c) {
                scani2c = false;
                Serial.print(F("I2C Detect Device Disable\n"));
                pmbusflag = true;
                key = 0;
                delay(500);
              }
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
    if(pecflag) Serial.print(F("PEC Enable\n"));
    else Serial.print(F("PEC Disable\n"));
    delay(1000);
}

void monitorstatus(){
  statusflag = !statusflag;
  if(statusflag) Serial.print(F("Status Monitor Enable\n"));
  else Serial.print(F("Status Monitor Disable\n"));
  delay(500);
  }

void standbystatus(){
  standbyflag = !standbyflag;
  if(standbyflag) Serial.print(F("Standby Enable\n"));
  else Serial.print(F("Standby Disable\n"));
  delay(500);
  }

void i2cdetectsstatus(){
  scani2c = !scani2c;
  if(scani2c) {
    Serial.print(F("I2C Detect Device Enable\n"));
    pmbusflag = false;
  }
  else {
    Serial.print(F("I2C Detect Device Disable\n"));
    pmbusflag = true;
  }
  delay(500);
}

void pmbus_devices_detect(){
  i2cdetects(0x00, 0x7F);  
  while(scani2c){
    digitalWrite(kLedPin, HIGH);
    pmbusdetects();
    delay(50);
    digitalWrite(kLedPin, LOW);
    delay(210);
    if(n > 0) scani2c = false;
    if (digitalRead(kButtonPin) == 0){
          delay(10);
          if(digitalRead(kButtonPin) == 0){
              buttonflag = false;
              buzzing();                            
              scani2c = false;
              Serial.print(F("PMbus Detect Device Disable\n"));
              delay(100);
          }
       }
    }
    Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);
    delay(100);
 }

void reset_address(){
      Serial.printf("Current PSU Address:%#02X; Patner PSU Address:%#02X.\n", ps_i2c_address, ps_patner_address);
      Serial.println(F("Input PSU address: (Can recognize Hex, Decimal, Octal, or Binary)"));
      Serial.println(F("Example: Hex: 0x11 (0x prefix) Octal: O21 (letter O prefix) Binary: B10001" ));
      ps_i2c_address = read_int();     
      Serial.printf("New PSU address: %#02X\n", ps_i2c_address);
      Serial.println(F("Input Patner PSU address:"));
      ps_patner_address = read_int();
      Serial.printf("New Patner PSU address: %#02X\n", ps_patner_address);
      delay(1000);
}

void eepromChecksum(){
    uint16_t checksum;
    eepromreadbytes(eeprom_address, 0, 128, eepbuffer);
    eepromreadbytes(eeprom_address, 128, 128, eepbuffer+128);       
    checksum = calcCheckSum(eepbuffer, 192);
    Serial.printf("EEPROM_CALC_CheckSum: 0x%04x \n", checksum);
    Serial.printf("EEPROM_READ_CheckSum: 0x%02x%02x \n", eepbuffer[190], eepbuffer[191]);
    printFru(0, 0xFF, eepbuffer);   
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
 
void setModel() {
      Serial.printf("Current PSU Model is %#02X \n", unitname);
      Serial.println(F("PSU Model Numbers: ()"));
      Serial.println(F("0 - Standard " ));
      Serial.println(F("1 - ADP1051 " ));
      Serial.println(F("2 - Redundancy " ));
      Serial.println(F("3 - Others " ));
      Serial.println(F("Input New PSU Model:"));
      unitname = read_int();     
      Serial.printf("New PSU Model is: %02x \n", unitname);
      delay(1000);
}
//
//void readEeprom(){
//      uint16_t offset;
//      int count;
//      int32_t serialread;
//      Serial.printf("EEPORM ADDRESS %#02X \n", eeprom_address);
//      Serial.println(F("Set EEPROM ADDRESS: (7 bit) such as:0x50 = 0xA0(8 bit)"));
//      serialread = read_int();
//      if(serialread) eeprom_address = (int)serialread;
//      Serial.printf("NEW ADDRESS 0x%02X \n", eeprom_address);
//      delay(100);
//      Serial.println(F("Is the EEPROM Data Size Lower than 0x100; 1 Yes, 0 No" ));
//      serialread = read_int();
//      if(serialread == 1)  eepromsize = false;
//      else eepromsize = true;
//      Serial.printf("eepromsize = %01d \n", eepromsize);
//      delay(100);
//      Serial.println(F("Set the Read Start Address(offset < 0x10000):" ));
//      offset = read_int();
//      Serial.printf("offset= %02d \n", offset);
//      delay(100);
//      Serial.println(F("Set the Read count Data( Count <= 32 ):" ));
//      count = read_int();
//      Serial.printf("count= %02d \n", count);
//      delay(100);
//      eepromreadbytes(eeprom_address, offset, count, eepbuffer);
//      printFru(0, count-1, eepbuffer); 
//      delay(3000);
//      while (!digitalRead(kButtonPin)) {};  // wait for button unpress
//}

uint8_t smbus_sent(){
  uint8_t count;
  bool martflag = false;
  Serial.println(F(" " ));
  read_data();
  if (ui_buffer[0] == 'm') return('m');
  if (ui_buffer[0] != '['){
    Serial.println(F("Smbus Invaild format" ));
    delay(100);
    return 0xff;
  }
  smbus_data[0] = tohex(ui_buffer[1])*16 + tohex(ui_buffer[2]);
//  Serial.printf("Data[0] is: %02x \n", data[0]);
//  delay(100);
  for(int i = 0; i < 32; i++){
    if (ui_buffer[3*i+3] == ']') break;
    smbus_data[i+1] = tohex(ui_buffer[3*i+4])*16 + tohex(ui_buffer[3*i+5]);
    count = i + 2;
    if (i == 31) {
      Serial.println(F("Smbus Invalid format" ));
      martflag = true;
      delay(100);
      break; 
    }
  }
  if (martflag) return 0xff;
  Serial.print(F("[" ));
  Serial.printf("%02x", smbus_data[0]);
  for(int i = 1; i < count; i++){
    Serial.printf(" %02x", smbus_data[i]);
  }
  Serial.println(F("]" ));
  if (smbus_data[0] >= 0 && smbus_data[0] <=9) return smbus_data[0];
  return 0;
}

uint8_t tohex(uint8_t val){
  uint8_t hex;
  if ( val - '0' >= 0 && val - '0' <=9){
     hex = val - '0';
  }
  else if( val == 'a' || val == 'A') hex = 10;
  else if( val == 'b' || val == 'B') hex = 11;
  else if( val == 'c' || val == 'C') hex = 12;
  else if( val == 'd' || val == 'D') hex = 13;
  else if( val == 'e' || val == 'E') hex = 14;
  else if( val == 'f' || val == 'F') hex = 15;
//  else if( val == 'm' || val == 'M') return('m');
  else {
    Serial.println(F("Invalid Hex Data" ));
    delay(100);
    return 0;
  }
  return hex;
}

void smbus_commands(){
      uint8_t user_command;
//      uint8_t size_n;
//      uint8_t lsb;
      uint16_t offset;
      int count;
      struct smbusCommand
        {      
            uint8_t commands;
            uint8_t databyte;
            uint16_t dataword;
            uint8_t datablock[32];
            uint16_t blocksize;
            uint8_t datablock_b[32];
            uint16_t blocksize_b;              
        }sm;
  Serial.println(F("Input Smbus Commands: (Hex Data Format)"));
  Serial.println(F("Example: [03 58 00 01] MSB:03(Smbus Command Protocol) Wirte Byte"));
  Serial.println(F("Smbus Command Protocol (0 to 7) Info:"));
  Serial.print(F("  0-Read Byte (RB)\n"));
  Serial.print(F("  1-Read Word (RW)\n"));
  Serial.print(F("  2-Read Block (RBL)\n"));
  Serial.print(F("  3-Write Byte (WB)\n"));
  Serial.print(F("  4-Write Word (WW)\n"));
  Serial.print(F("  5-Write Block (WBL)\n"));
  Serial.print(F("  6-Write Read Block (WRB)\n"));
  Serial.print(F("  7-Sent Byte (SBY)\n"));
  Serial.print(F("  8-EEPROM Read Byte (ER)\n"));
  Serial.print(F("  9-EEPROM Read Bytes (ERS)\n"));
  Serial.println(F("[03 58 00 01]: 58 Address; 00 Command Code; 01 Data Byte;"));
  Serial.println(F("Send (03)writeByte: smbus_writeByte(0x58, 0x00, 0x01)."));
//  Serial.println(F(" "));
  delay(100);             
  do{  
      Serial.print(F("\n0-RB; 1-RW; 2-RBL; 3-WB; 4-WW; 5-WBL;\n"));
      Serial.print(F("6-WRB; 7-SByte; 8-ER; 9-ERS\n"));
      Serial.print(F("Enter Smbus Protoccol: [xx xx xx xx xx] or Command:'m' to Main Menu\n"));  
      user_command = smbus_sent();                              //! Reads the user command
      if (user_command == 'm')   Serial.print(F("m\n"));        // Print m if it is entered
//      else  Serial.println(user_command);                       // Print user command         
    switch (user_command)
    {
      case 0:
        Serial.println(F("Smbus Read Byte:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.databyte = smbus_readByte(ps_i2c_address, sm.commands);
        Serial.printf("%02X: [%02X]\n", sm.commands, sm.databyte);
        delay(1000);       
      break;
      
      case 1:
        Serial.println(F("Smbus Read Word:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.dataword = smbus_readWord(ps_i2c_address, sm.commands);
        Serial.printf("%02X: [%02X %02X]\n", sm.commands, sm.dataword >> 8, (uint8_t)sm.dataword);
        delay(1000);
      break;
        
      case 2:
        Serial.println(F("Smbus Read Block:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.blocksize = smbus_data[3]; //Max size 32 block
        smbus_readBlock(ps_i2c_address, sm.commands, sm.datablock, sm.blocksize);
        Serial.printf(" %02X:", sm.commands);
        for (int n = 0; n < sm.blocksize; n++){
          Serial.printf(" %02X", sm.datablock[n]);
          delay(100);
        }
        Serial.println(F(" "));
        delay(1000);
      break;
            
      case 3:
        Serial.println(F("Smbus Write Byte:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.databyte = smbus_data[3];
        smbus_writeByte(ps_i2c_address, sm.commands, sm.databyte);
        Serial.println(F("Done."));
        delay(1000); 
        break;
                
      case 4:
        Serial.println(F("Smbus Write Word:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.dataword = smbus_data[3] << 8 | smbus_data[4];
        smbus_writeWord(ps_i2c_address, sm.commands, sm.dataword);
        Serial.println(F("Done."));
        delay(1000);
        break;
                
      case 5:
        Serial.println(F("Smbus Write Block:"));
        delay(100);
          ps_i2c_address = smbus_data[1];
          sm.commands = smbus_data[2];
          sm.blocksize = smbus_data[3];   // size Max 256 
          for(int i = 0; i < sm.blocksize; i++) {
            sm.datablock[i] = smbus_data[4+i];;
            Serial.printf("Block n=%02X Data:%02X\n", i, sm.datablock[i]);
            delay(100);     
          }
          smbus_writeBlock(ps_i2c_address, sm.commands, sm.datablock, sm.blocksize);
          Serial.println(F("Done."));
          delay(1000);     
        break;
      
       case 6:
        Serial.println(F("Smbus Write Read Blocks:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.blocksize = smbus_data[3];
        for(int i = 0; i < sm.blocksize; i++) {
          sm.datablock[i] = smbus_data[i+4];
//          Serial.printf("Block n=%02X Data:%02X\n", i, sm.datablock[i]);
//          delay(100);     
        }
        sm.blocksize_b = smbus_data[sm.blocksize+4];
//        Serial.printf("Read Block Size: %02X \n", sm.blocksize_b);
//        delay(100);       
        smbus_writeReadBlock (ps_i2c_address, sm.commands, sm.datablock, sm.blocksize, sm.datablock_b, sm.blocksize_b);      
        Serial.printf("%02X:", sm.blocksize_b);              
        for (int n = 0; n < sm.blocksize_b; n++){
          Serial.printf(" %02X", sm.datablock_b[n]);
          delay(100);
        }
        Serial.println(F(" "));
        delay(1000);     
        break;

       case 7:
        Serial.println(F("Smbus Sent Byte:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        smbus_sendByte(ps_i2c_address, sm.commands);
        Serial.println(F("Done."));
        delay(1000);       
          break;
         
      case 8:
        Serial.println(F("EEPROM Read Byte"));
        eeprom_address = smbus_data[1];
        offset = smbus_data[2] << 8 | smbus_data[3];
        eepbuffer[0] = eepromreadbyte(eeprom_address, offset);
        Serial.printf("offset0x%04X: %02X\n", offset, eepbuffer[0]);
        delay(1000);
        break;
      
      case 9:
        Serial.println(F("EEPROM Read Bytes"));
        eeprom_address = smbus_data[1];
        offset = smbus_data[2] << 8 | smbus_data[3];
        count = smbus_data[4];
        eepromreadbytes(eeprom_address, offset, count, eepbuffer);
        Serial.printf("offset0x%04X:\n", offset);
        printFru(0, count-1, eepbuffer); 
        delay(1000);
        break;
                
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}

void setIntervaltime() {
      Serial.printf("Current Interval Time(ms):%d\n", pmInterval);
      Serial.println(F("Input New Interval Time(ms) < 60S :"));
      pmInterval = read_int();     
      Serial.printf("New Interval Time(ms):%d\n", pmInterval);
      delay(1000);
}
