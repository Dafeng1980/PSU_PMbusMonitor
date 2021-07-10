
void setWifiMqtt(){
  delay(10);
  wifistatus = true;
  mqttflag = false;
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    k++;
    Serial.print(".");
    if( k >= 30){
    wifistatus = false;
    break;
    }
  }
  if(wifistatus){
      randomSeed(micros());
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      client.setCallback(callback);
      if(client.connect(clientID, mqtt_user, mqtt_password)) mqttflag = true;
      Serial.println("MQTT Broker Connected. \n "); 
    } 
  else{
    Serial.println("");
    Serial.println("WiFi connect Failed");
    }
  k = 0; 
}

void reconnect() {
  // Loop until we're reconnected
  //client.connect(clientID, mqtt_user, mqtt_password);
  while (!client.connected()) {
     Serial.print("Attempting MQTT connection..."); // Attempt to connect   
     String clientId = "ESP8266Client-";
     clientId += String(random(0xffff), HEX);
    if (client.connect((clientId.c_str()), mqtt_user, mqtt_password)) {
      Serial.println("connected to broker");
      client.publish("outTopic", "hello world reconnected");  // Once connected, publish an announcement...
      mqttflag = true;   
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      k++;
      delay(2000);   // Wait 2 seconds before retrying
        if( k >= 5){
            mqttflag = false;
            if( k >= 15){
                wifistatus = false;
                Serial.println(F("WiFi connect Failed!!"));
                delay(1000);
                k = 0;
                break;
              }
            break;
         }                   
      }
   }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
    if ((char)payload[0] == '0')      key = 0;
    else if ((char)payload[0] == '1') key = 1;
    else if ((char)payload[0] == '2') key = 2;
    else if ((char)payload[0] == '3') key = 3;
    else if ((char)payload[0] == '4') key = 4;
    else if ((char)payload[0] == '5') key = 5;
    else if ((char)payload[0] == '6') key = 6;
    else if ((char)payload[0] == '7') key = 7;
    else if ((char)payload[0] == '8') key = 8;
    else if ((char)payload[0] == '9') key = 9;
    else if ((char)payload[0] == 'o' || (char)payload[0] == 'O')
    {
            if(!psustatus){
                  psOn();
                  digitalWrite(kLedPin, HIGH);
                   psustatus = true;
                  Serial.println("PSU ON. \n ");
                  delay(100);
            }
        }
    else if ((char)payload[0] == 'R' || (char)payload[0] == 'r')
    {
           if(psustatus){
                  psustatus = false;
                  digitalWrite(kLedPin, LOW);                  
                  Serial.println("PSU RESTAR ON. \n ");
                  delay(100);
                }
                count = 0;
    }
    else key = 0;
    Serial.printf("\n Key= %#01d:\n", key); 
}

void mqttLoop(){
  if (!client.connected()) {
            reconnect();            
    }
    if (mqttflag){    
      client.loop();
      client.subscribe("aws/pmbus/set");
    }
}

void buttoncheck(){
  if (digitalRead(kButtonPin) == 0 && buttonflag){
      delay(10);
        if(digitalRead(kButtonPin) == 0){         
            if(!psustatus){
                psOn();
                digitalWrite(kLedPin, HIGH);
                psustatus = true;
                Serial.println("PSU ON. \n ");
                delay(200);
              }
            else{
              Serial.println("PSU ON STATUS. \n ");
              key++;
              if (key >= 10) key = 0;
              Serial.printf("\n Key= %#01d:\n", key);
            }
          buttonflag = false;            
       }
    }
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
    else if ((char)readval == '5') key = 5;
    else if ((char)readval == '6') key = 6;
    else if ((char)readval == '7') key = 7;
    else if ((char)readval == '8') key = 8;
    else if ((char)readval == '9') key = 9;
    else if ((char)readval == 'o' || (char)readval == 'O')
    {
            if(!psustatus){
                  psOn();
                  digitalWrite(kLedPin, HIGH);
                  psustatus = true;
                  Serial.println("PSU ON. \n ");
                  delay(100);
            }
        }
    else if ((char)readval == 'R' || (char)readval == 'r')
      {
           if(psustatus){
                  psustatus = false;
                  digitalWrite(kLedPin, LOW);                  
                  Serial.println("PSU RESTAR ON. \n ");
                  delay(100);
                }
                count = 0;
      }
    else if ((char)readval == 'h'  || (char)readval == 'H') printhelp();
    else {
      Serial.println("unknown command");
      Serial.println("type \'h\' for help");
      key = 0;       
    }
    Serial.printf("\n Key= %#01d:\n", key);
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
      delay(5);
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

void printhelp(){
      Serial.print(F("Here are commands can be used for AWS: \r\n "));
      Serial.print(F(" 1 > ATS Read & Check \r\n "));
      Serial.print(F(" 2 > Read Sensor Raw Data \r\n "));
      Serial.print(F(" 3 > Read BlackBox Data \r\n "));
      Serial.print(F(" 4 > TBD \r\n "));
      Serial.print(F(" 5 > TBD \r\n "));
      Serial.print(F(" o > PMbus Enabled PS-ON \r\n "));
      Serial.print(F(" r > Reset PS-ON \r\n "));
      Serial.print(F(" h > HELP Shown  \r\n "));
      Serial.print(F(" 0 > Set Default \r\n "));
      delay(2000);  
}

void printpmbusData(struct PowerPmbus busData)
{
    Serial.println(F("========== PMBUS DATA =========="));
    Serial.println(" ");
    Serial.print(F("PMBUS ADDRESS: 0x"));    //F function enable to decrease sram usage
    Serial.println(busData.i2cAddr, HEX);
    Serial.println(F(" "));
    Serial.print(F("Vs1_IN : "));
    Serial.print(busData.inputVs1, 2);
    Serial.print(F("V"));
    Serial.print(F("     Vs2_IN : "));
    Serial.print(busData.inputVs2, 2);
    Serial.println(F("V")); 
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
    Serial.print(F("    8E: "));
    Serial.print(busData.temp2, 0);
    Serial.print(F("C"));
    Serial.print(F("    8F: "));
    Serial.print(busData.temp3, 0);
    Serial.println(F("C"));
    Serial.print(F("TEMP_T4: "));
    Serial.print(busData.temp4, 0);
    Serial.print(F("C"));
    Serial.print(F("    T5: "));
    Serial.print(busData.temp5, 0);
    Serial.print(F("C"));
    Serial.print(F("    T6: "));
    Serial.print(busData.temp6, 0);
    Serial.println(F("C"));
    Serial.print(F("  Fan1 Speed: "));
    Serial.print(busData.fanSpeed, 1);
    Serial.println(F(" rpm"));
    Serial.println(F(" "));
          
    Serial.print(F("STATUS WORD: 0x"));
    Serial.printf("%04x\n", busData.statusWord);    
    Serial.print(F(" 0B "));
    printBits((busData.statusWord & 0xFF));
    Serial.print(F("    LOW: 0x"));
    Serial.printf("%02x\n", busData.statusWord & 0xFF);
    Serial.print(F(" 0B "));
    printBits((busData.statusWord >> 8));
    Serial.print(F("   HIGH: 0x"));
    Serial.printf("%02x\n", busData.statusWord >> 8);
   
    Serial.print(F("ATS STATUS WORD 0x"));
    Serial.printf("%04x\n", busData.atsStatusWord);    
    Serial.print(F("  0B "));
    printBits((busData.atsStatusWord & 0xFF));
    Serial.print(F("   LOW: 0x"));
    Serial.printf("%02x\n", busData.atsStatusWord & 0xFF);
    Serial.print(F("  0B "));
    printBits((busData.atsStatusWord >> 8));
    Serial.print(F("   HIGH: 0x"));
    Serial.printf("%02x\n", busData.atsStatusWord >> 8);

    Serial.print(F("CHG STATUS WORD 0x"));
    Serial.printf("%04x\n", busData.chgStatusWord);   
    Serial.print(F("  0B "));
    printBits((busData.chgStatusWord & 0xFF));
    Serial.print(F("   LOW: 0x"));
    Serial.printf("%02x\n", busData.chgStatusWord & 0xFF);
    Serial.print(F("  0B "));
    printBits((busData.chgStatusWord >> 8));
    Serial.print(F("   HIGH: 0x"));
    Serial.printf("%02x\n", busData.chgStatusWord >> 8);

    Serial.print(F("DCH STATUS WORD 0x"));
    Serial.printf("%04x\n", busData.dchStatusWord);   
    Serial.print(F("  0B "));
    printBits((busData.dchStatusWord & 0xFF));
    Serial.print(F("   LOW: 0x"));
    Serial.printf("%02x\n", busData.dchStatusWord & 0xFF);
    Serial.print(F("  0B "));
    printBits((busData.dchStatusWord >> 8));
    Serial.print(F("   HIGH: 0x"));
    Serial.printf("%02x\n", busData.dchStatusWord >> 8);
 
    Serial.println(F(" ")); 
    ledflash();  
}

void publishPmbusData(struct PowerPmbus busData){
     
 if (count%10 == 0) {
      ++value;     
      snprintf (msg, MSG_BUFFER_SIZE, "PMBUS_Addr: 0x%02x Refresh#%ld", busData.i2cAddr, value );
      client.publish("aws/pmbus/status", msg);
      Serial.printf("\nPMBUS_PUBLISH_REFRESH  %#01d \n", value);
      snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.outputVcap);
      client.publish("aws/pmbus/output/vbulk", msg);
    }
  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.inputV);
  client.publish("aws/pmbus/input/volt", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.inputVs1);
  client.publish("aws/pmbus/input/vs1", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.inputVs2);
  client.publish("aws/pmbus/input/vs2", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", busData.inputA);
  client.publish("aws/pmbus/input/curr", msg);
  
  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.inputP);
  client.publish("aws/pmbus/input/power", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%5.4f", busData.outputV);
  client.publish("aws/pmbus/output/volt", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", busData.outputA);
  client.publish("aws/pmbus/output/curr", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", busData.outputP);
  client.publish("aws/pmbus/output/power", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.fanSpeed);
  client.publish("aws/pmbus/sensor/fanspeed", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.fanSpeed2);
  client.publish("aws/pmbus/sensor/fanspeed2", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp1);
  client.publish("aws/pmbus/sensor/temp1", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp2);
  client.publish("aws/pmbus/sensor/temp2", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp3);
  client.publish("aws/pmbus/sensor/temp3", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp4);
  client.publish("aws/pmbus/sensor/temp4", msg);
  
  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp5);
  client.publish("aws/pmbus/sensor/temp5", msg);
  
  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", busData.temp6);
  client.publish("aws/pmbus/sensor/temp6", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%02x%02x", busData.statusWord >> 8, busData.statusWord & 0xFF);
  client.publish("aws/pmbus/status/word", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%02x%02x", busData.atsStatusWord >> 8, busData.atsStatusWord & 0xFF);
  client.publish("aws/pmbus/status/ats", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%02x%02x", busData.chgStatusWord >> 8, busData.chgStatusWord & 0xFF);
  client.publish("aws/pmbus/status/chg", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%02x%02x", busData.dchStatusWord >> 8, busData.dchStatusWord & 0xFF);
  client.publish("aws/pmbus/status/dch", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%04x", busData.ioutrawData);
  client.publish("aws/pmbus/rawdata/iout", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%04x", busData.vsenrawData);
  client.publish("aws/pmbus/rawdata/vsensor", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%04x", busData.vorirawData);
  client.publish("aws/pmbus/rawdata/vor", msg);
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

void pmbusdetects(){
  uint8_t i, address, error; 
  Serial.print("   ");   // table header
  for (i = 8; i < 16; i++) {
    Serial.printf("%3x", i);
  }
      Serial.printf("\n%#02x:", 0x50);
  for (address = 88; address <= 95; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
    //  if (address == 92) error = 0;
      if (error == 0) {  // device found        
        n++;
        if(n == 1) ps_i2c_address = address;
        else ps_i2c_address = PS_ADDRESS;
        if(n == 2) ps_patner_address = address;
        else ps_patner_address = PS_PARTNER_ADDRESS;
        Serial.printf(" %02x", address);       
      } else if (error == 4) { // other error
        Serial.print(" XX");
      } else { // error = 2: received NACK on transmit of address
               // error = 3: received NACK on transmit of data        
        Serial.print(" --");
        delay(20);
      } 
  }
  Serial.println("\n");
//  Serial.println(n);
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
    if(wifistatus && mqttflag){
       tm = pmbus_readStatusTemp(ps_i2c_address);
       fa = pmbus_readStatusFan(ps_i2c_address);
       cm = pmbus_readStatusCml(ps_i2c_address);
       snprintf (msg, MSG_BUFFER_SIZE, "Fan:0x%02x Temp:0x%02x Cml:0x%02x", fa, tm, cm);
       client.publish("aws/pmbus/status/fanTempCml", msg);
       vo = pmbus_readStatusVout(ps_i2c_address);
       io = pmbus_readStatusIout(ps_i2c_address);
       in = pmbus_readStatusInput(ps_i2c_address);
       snprintf (msg, MSG_BUFFER_SIZE, "Vout:0x%02x Iout:0x%02x Input:0x%02x", vo, io, in);
       client.publish("aws/pmbus/status/voutIoutInput", msg);
    }
}


bool readpmbusdata()
{   
    bool ret = true;
  if(smbus_waitForAck(ps_i2c_address, 0x00) == 0) {
      Serial.println("PMBUS Polling Fail \n");
      if(wifistatus  && mqttflag){
        if(count%6 == 0){
            ++value;     
            snprintf (msg, MSG_BUFFER_SIZE, "PMBUS Polling Fail  Loop#%ld", value);
            client.publish("pmbus/status", msg);
        }
      }
      return ret = false;
    } 
     pd.inputV = pmbus_readVin(ps_i2c_address);     
     pd.inputA = pmbus_readIin(ps_i2c_address);
     pd.inputP = pmbus_readPin(ps_i2c_address);      
     pd.outputV = pmbus_readVout(ps_i2c_address);
     pd.outputA = pmbus_readIout(ps_i2c_address);    
     pd.outputP = pmbus_readPout(ps_i2c_address);
     pd.outputVcap = psu_readVcap(ps_i2c_address);
     pd.temp1 = pmbus_readOtemp(ps_i2c_address);           //temp sensor 0x8D  
     pd.temp2 = pmbus_readItemp(ps_i2c_address);        //temp sensor 0x8E  
     pd.temp3 = pmbus_readMtemp(ps_i2c_address);        //temp sensor 0x8F
     pd.temp4 = psu_readTemp4(ps_i2c_address);
     pd.temp5 = psu_readTemp5(ps_i2c_address);
     pd.temp6 = psu_readTemp6(ps_i2c_address);
     pd.fanSpeed = pmbus_readFanSpeed1(ps_i2c_address);
     pd.fanSpeed2 = pmbus_readFanSpeed2(ps_i2c_address);
     pd.statusWord = pmbus_readStatusWord(ps_i2c_address);
     pd.atsStatusWord = pmbus_readStatusWord(ats_i2c_address);
     pd.chgStatusWord = psu_readStatusChg(ps_i2c_address);
     pd.dchStatusWord = psu_readStatusDch(ps_i2c_address);
     delay(2);
     pd.inputVs1 = ats_readVin_S1(ats_i2c_address);
     pd.inputVs2 = ats_readVin_S2(ats_i2c_address);
     pd.atsStatusWord = pmbus_readStatusWord(ats_i2c_address);
     pd.i2cAddr = ps_i2c_address;
     return ret;  
}

void m24c32Checksum(){
    uint16_t checksum;
    eepromreadbytes(m24c32_address, 0, 256, eepbuffer);       
    checksum = calcCheckSum(eepbuffer, 192);
    Serial.printf("EEPROM_CALC_CheckSum: 0x%04x \n", checksum);
    Serial.printf("EEPROM_READ_CheckSum: 0x%02x%02x \n", eepbuffer[190], eepbuffer[191]);
      if(wifistatus  && mqttflag){
        snprintf (msg, MSG_BUFFER_SIZE, "Ca: 0x%04x Re: 0x%02x%02x", checksum, eepbuffer[190], eepbuffer[191]);
        client.publish("pmbus/eeprom/checksum", msg);      
       } 
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


void ledflash(){
  ledstatus = !ledstatus;
  if(ledstatus) digitalWrite(kLedPin, HIGH);
  else digitalWrite(kLedPin, LOW);
}


void deviceAddrset(){
    scani2c = false;
    i2cdetects(0x03, 0x7F); 
    while(scani2c){
    pmbusdetects();
    delay(10);
    if(n > 0) break;
    } 
}
