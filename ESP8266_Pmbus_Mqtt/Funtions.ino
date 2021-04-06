
void i2cdetects(uint8_t first, uint8_t last) {
  uint8_t i, address, error;
  char buff[10];
  Serial.print("   ");   // table header
  for (i = 0; i < 16; i++) {
    //Serial.printf("%3x", i);
    sprintf(buff, "%3x", i);
    Serial.print(buff);
  }
  // table body   
  for (address = 0; address <= 127; address++) {  // addresses 0x00 through 0x77
    if (address % 16 == 0) {
      //Serial.printf("\n%#02x:", address & 0xF0);
      sprintf(buff, "\n%02x:", address & 0xF0);
      Serial.print(buff);
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (error == 0) {  // device found       
        //Serial.printf(" %02x", address);
        sprintf(buff, " %02x", address);
        Serial.print(buff);
      } else if (error == 4) {  // other error        
        Serial.print(" XX");
      } else {  // error = 2: received NACK on transmit of address
                // error = 3: received NACK on transmit of data        
        Serial.print(" --");
      }
    } else {      
      Serial.print("   "); // address not scanned
    }
  }
  Serial.println("\n");
}

void printpmbusData(struct PowerPmbus busData)
{
    Serial.print(F("PMBUS ADDRESS: 0x"));    //F function enable to decrease sram usage
    Serial.println(busData.i2cAddr, HEX);   
    Serial.println(F("INPUT: "));
    Serial.print(F("Volt : "));
    Serial.print(busData.inputV, 2);
    Serial.print(F("V"));
    Serial.print(F("     Curr : "));
    Serial.print(busData.inputA, 3);
    Serial.println(F("A"));
    Serial.println(F(" "));
    Serial.println(F("OUTPUT: "));
    
    Serial.print(F("Vmain_Volt: "));
    Serial.print(busData.outputV, 3);
    Serial.print(F("V"));
    Serial.print(F("    Curr: "));
    Serial.print(busData.outputA, 3);
    Serial.println(F("A"));
    
    Serial.print(F("Vsb_Volt: "));
    Serial.print(busData.outputVsb, 3);
    Serial.print(F("V"));
    Serial.print(F("    Sb_Curr: "));
    Serial.print(busData.outputAsb, 3);
    Serial.println(F("A"));
    
    Serial.print(F("Pin: "));
    Serial.print(busData.inputP, 2);
    Serial.print(F("W"));
    Serial.print(F("    Pout: "));
    Serial.print(busData.outputP, 2);
    Serial.println(F("W"));
    Serial.println(F(" "));
    
          Serial.print(F("Temperature   8D: "));
          Serial.print(busData.temp1, 0);
          Serial.print(F("C"));
          Serial.print(F("    8E: "));
          Serial.print(busData.temp2, 0);
          Serial.print(F("C"));
          Serial.print(F("    8F: "));
          Serial.print(busData.temp3, 0);
          Serial.println(F("C"));
          Serial.print(F("  Fan1 Speed: "));
          Serial.print(busData.fanSpeed, 1);
          Serial.println(F(" rpm"));
          Serial.println(F(" "));
    Serial.print(F("STATUS WORD 0x"));
    Serial.println(busData.statusWord, HEX);
    
    Serial.print(F("  0B "));
    printBits((busData.statusWord & 0xFF));
    Serial.print(F("   LOW: 0x"));
    Serial.println((busData.statusWord & 0xFF), HEX);

    Serial.print(F("  0B "));
    printBits((busData.statusWord >> 8));
    Serial.print(F("   HIGH: 0x"));
    Serial.println((busData.statusWord >> 8), HEX);

    Serial.println(F(" ************ PMBUS DATA  ************ "));
    Serial.println(F(" "));    
}

void publishPmbusData(struct PowerPmbus busData){
     
 if (count%10 == 0) {
      ++value;     
      snprintf (msg, MSG_BUFFER_SIZE, "PSU_Addr: 0x%02x Refresh#%ld", busData.i2cAddr, value );
      client.publish("pmbus/status", msg);
//      snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", pmbus_readVbulk(ps_i2c_address));
//      client.publish("pmbus/output/Vbulk", msg);
//      Serial.print("Vbluk= ");
//      Serial.println(msg);
      Serial.printf("\nPMBUS_PUBLISH_REFRESH  %#01d \n", value);
    }
  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.inputV);
  client.publish("pmbus/input/Volt", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", busData.inputA);
  client.publish("pmbus/input/Curr", msg);
  
  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.inputP);
  client.publish("pmbus/input/Power", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%5.4f", busData.outputV);
  client.publish("pmbus/output/Volt", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%5.4f", busData.outputVsb);
  client.publish("pmbus/output/Vsb", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", busData.outputA);
  client.publish("pmbus/output/Curr", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", busData.outputAsb);
  client.publish("pmbus/output/Csb", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.outputP);
  client.publish("pmbus/output/Power", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.fanSpeed);
  client.publish("pmbus/sensor/fanSpeed", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp1);
  client.publish("pmbus/sensor/temp1", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp2);
  client.publish("pmbus/sensor/temp2", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp3);
  client.publish("pmbus/sensor/temp3", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%02x%02x", busData.statusWord >> 8, busData.statusWord & 0xFF);
//  Serial1.print("Publish message: ");
//  Serial1.println(msg);
  client.publish("pmbus/status/word", msg);
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
   Serial.println();
}

void pmbusdetects(){
  uint8_t i, address, error; 
  Serial.print("   ");  // table header
  for (i = 8; i < 16; i++) {
    Serial.printf("%3x", i);
  }
      Serial.printf("\n%#02x:", 0x50);
  for (address = 88; address <= 95; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();  //  if (address == 92) error = 0;   
      if (error == 0) {  // device found       
        n++;
        if(n == 1) ps_i2c_address = address;
        else ps_i2c_address = PSU_ADDRESS;
        if(n == 2) ps_patner_address = address;
        else ps_patner_address = PSU_PARTNER_ADDRESS;
        Serial.printf(" %02x", address);       
      } 
      else if (error == 4) {  // other error       
        Serial.print(" XX");
      } 
      else {// error = 2: received NACK on transmit of address
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
    if(smbus_waitForAck(ps_i2c_address, 0x00) == 0) {
      Serial.println("PMBUS Polling Fail \n");
      return ret = false;
    }    
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
     pd.i2cAddr = ps_i2c_address;
     return ret;  
}
