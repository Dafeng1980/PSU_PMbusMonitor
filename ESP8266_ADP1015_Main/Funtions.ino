
void setupWifi(){
  delay(10);
  wiset = true;
  wifistatus = true;
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    k++;
    Serial.print(".");
    if( k >= 3){
    wiset = false;
    wifistatus = false;
    Serial.printf("\n Wi-Fi Not Connect.. \n");
    break;
    }
  }
  if(wiset){
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  }
  else{
    Serial.println("");
    Serial.println("WiFi connect Failed");
  }
}

void i2cdetects(uint8_t first, uint8_t last) {
  uint8_t i, address, error;
  char buff[10];
  Serial.print("   ");    // table header
  for (i = 0; i < 16; i++) {
    //Serial.printf("%3x", i);
    sprintf(buff, "%3x", i);
    Serial.print(buff);
  }
  // table body
  for (address = 0; address <= 127; address++) {    // addresses 0x00 through 0x77
    if (address % 16 == 0) {
      //Serial.printf("\n%#02x:", address & 0xF0);
      sprintf(buff, "\n%02x:", address & 0xF0);
      Serial.print(buff);
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      delay(2);
      if (error == 0) {  // device found       
        //Serial.printf(" %02x", address);
        sprintf(buff, " %02x", address);
        Serial.print(buff);
      } else if (error == 4) { // other error       
        Serial.print(" XX");
      } else {  // error = 2: received NACK on transmit of address
                // error = 3: received NACK on transmit of data        
        Serial.print(" --");
      }
    } else {  // address not scanned     
      Serial.print("   ");
    }
  }
  Serial.println("\n");
}

void printpmbusData(struct PowerPmbus busData)
{
    Serial.println(" ");
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
      snprintf (msg, MSG_BUFFER_SIZE, "PMBUS_Addr: 0x%02x Refresh#%ld", busData.i2cAddr, value );
      client.publish("pmbus/status", msg);
      Serial.printf("\nPMBUS_PUBLISH_REFRESH  %#01d \n", value);
      snprintf (msg, MSG_BUFFER_SIZE, "Pri_REV: %02x%02x%02x",ver[0],ver[1],ver[2]);
      client.publish("pmbus/fru/PRversion", msg);
      snprintf (msg, MSG_BUFFER_SIZE, "Sec_REV: %02x%02x%02x",ver[3],ver[4],ver[5]);
      client.publish("pmbus/fru/SEversion", msg);
    }
  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.inputV);
  client.publish("pmbus/input/Volt", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", busData.inputA);
  client.publish("pmbus/input/Curr", msg);
  
  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.inputP);
  client.publish("pmbus/input/Power", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%5.4f", busData.outputV);
  client.publish("pmbus/output/Volt", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", busData.outputA);
  client.publish("pmbus/output/Curr", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.outputP);
  client.publish("pmbus/output/Power", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp1);
  client.publish("pmbus/sensor/temp1", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%02x%02x", busData.statusWord >> 8, busData.statusWord & 0xFF);
  client.publish("pmbus/status/word", msg);
}

void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte) Serial.print('1');
   else Serial.print('0');
  }
}

void printchar(uint8_t *values, uint8_t bsize){
  for(int i = 0; i < bsize; i++){
    Serial.printf(" %02x", values[i]);
  }
   Serial.println();
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
          if (address >= first && address <= last) {
            Serial.printf(" %02x", values[address]);
            delay(10);
          }
           else Serial.print("   ");
            }
      Serial.println("\n");
}

void pmbusStatus()
{
    uint16_t w_val;
    uint8_t msb,lsb,io,in,tm,fa,vo,cm;
    Serial.println(F("========= READ ALL STATUS =========="));
    Serial.println(F(" "));
    w_val = pd.statusWord;
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
        vo = pmbus_readStatusVout(ps_i2c_address);
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
        io = pmbus_readStatusIout(ps_i2c_address);
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
        in = pmbus_readStatusInput(ps_i2c_address);
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

    if(msb & 0x08)  Serial.println(F("STATUS_POWER_GOOD#_FAULT !! "));

    if(msb & 0x04){
      fa = pmbus_readStatusFan(ps_i2c_address);
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

    if(msb & 0x02)  Serial.println(F("STATUS_OTHERS_WARNING !! "));
    
    if(lsb & 0x40)  Serial.println(F("STATUS_PS_OFF !! "));
    if(lsb & 0x20)  Serial.println(F("STATUS_MAIN_OUTPUT_OV_FAULT !! "));

    if(lsb & 0x10){
        io = pmbus_readStatusIout(ps_i2c_address);
        Serial.print(F("STATUS_IOUT 0B "));
        printBits(io);
        Serial.print(F("    : 0x"));
        Serial.println(io, HEX);
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
        Serial.println(in, HEX);
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
        Serial.println(tm, HEX);
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
      Serial.println(cm, HEX);
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
    if(wifistatus){
       tm = pmbus_readStatusTemp(ps_i2c_address);
       fa = pmbus_readStatusFan(ps_i2c_address);
       cm = pmbus_readStatusCml(ps_i2c_address);
       snprintf (msg, MSG_BUFFER_SIZE, "Fan:0x%02x Temp:0x%02x Cml:0x%02x", fa, tm, cm);
       client.publish("pmbus/status/fanTempCml", msg);
       vo = pmbus_readStatusVout(ps_i2c_address);
       io = pmbus_readStatusIout(ps_i2c_address);
       in = pmbus_readStatusInput(ps_i2c_address);
       snprintf (msg, MSG_BUFFER_SIZE, "Vout:0x%02x Iout:0x%02x Input:0x%02x", vo, io, in);
       client.publish("pmbus/status/voutIoutInput", msg);
    }
}

bool readpmbusdata()
{   
    bool ret = true;
  if(smbus_waitForAck(ps_i2c_address, 0x00) == 0) {
      Serial.println("PMBUS Polling Fail \n");
      if(wifistatus){
        if(count%6 == 0){
            ++value;     
            snprintf (msg, MSG_BUFFER_SIZE, "PMBUS Polling Fail  Loop#%ld", value);
            client.publish("pmbus/status", msg);
        }
      }
      return ret = false;
    } 
     pd.inputV  = pmbus_readVin(ps_i2c_address);     
     pd.inputA  = pmbus_readIin(ps_i2c_address);
     pd.inputP  = pmbus_readPin(ps_i2c_address);      
     pd.outputV = pmbus_readVout(ps_i2c_address);
     pd.outputA = pmbus_readIout(ps_i2c_address);    
     pd.outputP = pmbus_readPout(ps_i2c_address);
     pd.temp1   = pmbus_readOtemp(ps_i2c_address);           //temp sensor 0x8D  
     pd.statusWord = pmbus_readStatusWord(ps_i2c_address);
     pd.i2cAddr = ps_i2c_address;
     return ret;  
}
