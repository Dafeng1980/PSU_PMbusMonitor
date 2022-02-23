void readSensors(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= pmInterval){
        previousMillis = currentMillis;
        if(scani2c) i2cdetects(0x00, 0x7F);
        if(readpmbusdata()){
            if(0 != pd.statusWord && statusflag) pmbusStatus();      
            if(0 == count%5) printpmbusData(pd);
            if(key == 1){
                monitorstatus();
                key = 0;
            }
            else if(key == 2){
                standbystatus();
                key = 0;
            }
            else if(key == 3){
               Serial.println(F("TBD "));
               delay(100);
               key = 0;
            }
            else if(key == 4){
               Serial.println(F("TBD "));
               delay(100);
               key = 0;                
              }       
          }
          count++;
          buttonflag = true;                     
     } 
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

void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}

void printchar(uint8_t *values, uint8_t bsize){
  for(int i = 0; i < bsize; i++){
    Serial.printf(" %02x", values[i]);
  }
   Serial.println(F(" "));
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

bool readpmbusdata()
{   
      bool ret = true;
      if(!pmbusflag) return ret = false;
      if(smbus_waitForAck(ps_i2c_address, 0x00) == 0) {  //0x00 PAGE read
          Serial.println(F("PMBUS Polling Fail, Type 'h' To Help\n"));      
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
     return ret;     
}

void ledflash(){
  ledstatus = !ledstatus;
  if(ledstatus) digitalWrite(kLedPin, HIGH);
  else digitalWrite(kLedPin, LOW);
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

void mromChecksum(){
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
