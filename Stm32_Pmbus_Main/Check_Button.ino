void checkButton() {
  if(buttonflag) { 
      key = 0;         
      buttonVal = digitalRead(kButtonPin);
                // Button pressed down
     if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce) {
        downTime = millis();
        ignoreUp = false;
        waitForUp = false;
        singleOK = true;
        holdEventPast = false;
        longHoldEventPast = false;
    if ((millis() - upTime) < DCgap && DConUp == false && DCwaiting == true) DConUp = true;
    else DConUp = false;
    DCwaiting = false;
  }
  // Button released
  else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce) {
    if (not ignoreUp) {
      upTime = millis();
      if (DConUp == false) DCwaiting = true;
      else {
        key = 2;
        DConUp = false;
        DCwaiting = false;
        singleOK = false;
      }
    }
  }
  // Test for normal click event: DCgap expired
  if (buttonVal == HIGH && (millis() - upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true) {
    key = 1;
    ledflash();
    DCwaiting = false;
  }
  // Test for hold
  if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
    // Trigger "normal" hold
    if (not holdEventPast) {
      key = 3;
      waitForUp = true;
      ignoreUp = true;
      DConUp = false;
      DCwaiting = false;
      //downTime = millis();
      holdEventPast = true;
    }
    // Trigger "long" hold
    if ((millis() - downTime) >= longHoldTime) {
      if (not longHoldEventPast) {
        key = 4;
        longHoldEventPast = true;
      }
    }
  }
  buttonLast = buttonVal;
 }
}

void pmbus_devices_init(){
  ps_i2c_address = PS_I2C_ADDRESS;
  ps_patner_address = PS_PARTNER_ADDRESS;
  pecflag = PEC_DISABLE;
  i2cdetects(0x03, 0x7F);
  int i = 3;
  int n = 0;
  bool scanpsu = true;    
  while(scanpsu){
    ledflash();
    pmbusdetects();
    delay(100);
    i--;
    if(n > 1)  scanpsu = false;
    if(i <= 0) scanpsu = false;
    }
    Log.noticeln("PMBUSADDRESS 0x%x:", ps_i2c_address);
    n = 0;
    scanpsu = false;
    delay(100);
 }

void pmbusdetects() {
  uint8_t n = 0, address, rerror;
  char c[6];
  Log.notice("   ");                   // table header
  for (int i = 8; i < 16; i++) {
    sprintf(c, "%3x",  i);
    Log.notice("%s", c);
  }
  Log.notice(CR "%x:", 0x50);
  for (address = 88; address <= 95; address++) {
        Wire.beginTransmission(address);
        rerror = Wire.endTransmission();
        if (rerror == 0) {                  // device found        
          n++;
          if(n == 1) ps_i2c_address = address;
          else ps_i2c_address = PS_I2C_ADDRESS;
          if(n == 2) ps_patner_address = address;
          else ps_patner_address = PS_PARTNER_ADDRESS;
          sprintf(c, " %02x", address);
          Log.notice("%s", c);       
        } 
        else if (rerror == 4) {              // other error        
            Log.notice(" XX");
        } 
        else {                             // error = 2: received NACK on transmit of address        
            Log.notice(" --");             // error = 3: received NACK on transmit of data        
        }
        delay(10);
    }
    Log.noticeln(CR"");
    Log.noticeln("%x",n);
}

void i2cdetects(uint8_t first, uint8_t last) {
  uint8_t i, address, rerror;
  int q = 0;
  char c[6];
  char addr[35];
  Log.notice("   ");            // table header
  for (i = 0; i < 16; i++) {
    sprintf(c, "%3x",  i);
    Log.notice("%s", c);
  }
  for (address = 0; address <= 127; address++) {             // addresses 0x00 through 0x77
    if (address % 16 == 0) {                            // table body
          sprintf(c, "0x%02x:", address & 0xF0);
          Log.notice(CR "%s", c);
      }
    if (address >= first && address <= last) {
        Wire.beginTransmission(address);
        rerror = Wire.endTransmission();
        delay(10);
        if (rerror == 0) {                           // device found
          sprintf(c, " %02x", address);
          Log.notice("%s", c);
          addr[3*q] = hex_table[address >> 4];
          addr[3*q + 1] = hex_table[address & 0x0f];
          addr[3*q + 2] = ' ';
          q++;
      } else if (rerror == 4) {    // other error      
        Log.notice(" XX");
      } else {                   // error = 2: received NACK on transmit of address              
        Log.notice(" --");    // error = 3: received NACK on transmit of data
      }
    } else {                 // address not scanned      
      Log.notice("   ");
    }
    if(q > 10) break;
  }
  addr[3*q] = '\0';
  Log.noticeln("" CR);
  snprintf (msg, MSG_BUFFER_SIZE, "Scan addr at:0x%s", addr);
//  if(mqttflag) client.publish("rrh/pmbus/set/info", msg); 
}

void pmbusflagset(uint8_t val){
  if(val == 0) {
    pmbusflag = false;
    Log.noticeln(F("pmbusflag Disable"));
//    if(mqttflag) client.publish("rrh/pmbus/enable", "0");
  }
  else {
    pmbusflag = true;
    Log.noticeln(F("pmbusflag Enable"));
//    if(mqttflag) client.publish("rrh/pmbus/enable", "1");
  }
  delay(100);
}

void pecstatus(){
    pecflag = !pecflag;
    if(pecflag) {
      Log.noticeln(F("PEC Enable"));
//      if(mqttflag) client.publish("rrh/pmbus/pec", "1");
    }
    else {
      Log.noticeln(F("PEC Disable"));
//      if(mqttflag) client.publish("rrh/pmbus/pec", "0");
    }
    delay(100);
}

void i2cdetectsstatus(){
  scani2c = !scani2c;
  if(scani2c) {
    Log.noticeln(F("I2C Detect Device Enable"));
//    if(mqttflag) client.publish("rrh/pmbus/scan", "1");
     pmbusflag = false;
  }
  else {
    Log.noticeln(F("I2C Detect Device Disable"));
//    if(mqttflag) client.publish("rrh/pmbus/scan", "0");
    pmbusflag = true;
  }
  delay(100);
}

void setIntervaltime() {
      Log.noticeln("Current Interval Time(ms):%d", pmInterval);
      Log.noticeln(F("Input New Interval Time(ms) < 60S :"));
      pmInterval = read_int();     
      Log.noticeln("New Interval Time(ms):%d", pmInterval);
      delay(100);
}

void reset_address(){
      Log.noticeln("Current PSU Address:0x%x; Patner PSU Address:0x%x.", ps_i2c_address, ps_patner_address);
      Log.noticeln(F("Input PSU address: (Can recognize Hex, Decimal, Octal, or Binary)"));
      Log.noticeln(F("Example: Hex: 0x11 (0x prefix) Octal: O21 (letter O prefix) Binary: B10001" ));
      ps_i2c_address = read_int();     
      Log.noticeln("New PSU address: 0x%x", ps_i2c_address);
      Log.noticeln(F("Input Patner PSU address:"));
      ps_patner_address = read_int();
      Log.noticeln("New Patner PSU address: 0x%x", ps_patner_address);
      delay(200);
}

void ledflash(){
  ledstatus = !ledstatus;
  if(ledstatus) digitalWrite(kLedPin, HIGH);
  else digitalWrite(kLedPin, LOW);
}

void monitorstatus(){
  statusflag = !statusflag;
  if(statusflag) {
    Log.noticeln(F("Status Monitor Enable"));
//    if(mqttflag) client.publish("rrh/pmbus/monitor", "1");
  }
  else {
      Log.noticeln(F("Status Monitor Disable"));
//      if(mqttflag) client.publish("rrh/pmbus/monitor", "0");
  }
  delay(100);
 }

void standbystatus(){
  standbyflag = !standbyflag;
  if(standbyflag) {
    Log.noticeln(F("Standby Enable"));
//    if(mqttflag) client.publish("rrh/pmbus/standby", "1");
  }
  else {
    Log.noticeln(F("Standby Disable"));
//    if(mqttflag) client.publish("rrh/pmbus/standby", "0");
  }
  delay(100);
 }

void printhelp(){
      Log.noticeln(F("************* WELCOME TO PMbus & SCPI Tools **************"));
      Log.notice(F("Here are commands can be used." CR));
      Log.notice(F(" 1 > Moitor PSU All Status On/Off" CR));
      Log.notice(F(" 2 > Add sensors Detect" CR));
      Log.notice(F(" 3 > Add engery Detect" CR));
      Log.notice(F(" 4 > Add Standby(Page 1)" CR));     
      Log.notice(F(" 5 > Scan PSU Address At PMbus" CR));      
      Log.notice(F(" 9 > Enable/Disable PEC" CR));
      Log.notice(F(" 0 > Set All to Default" CR));
      Log.notice(F(" e > Set the PMbus Interval Timing" CR));
      Log.notice(F(" c > Enter The SMbus Command Function & Read EEPROM" CR));
      Log.notice(F(" r > Modify the PMbus Device Address" CR));
      Log.notice(F(" z > Reset the ESP device" CR));
      Log.notice(F(" h > Display This Help" CR));
      Log.notice("" CR);
      Log.notice("Enter a command: " CR);      
      delay(100);  
}

void defaultint(){
            expandengery = false;
            expandsensor = false;
            standbyflag = false;
            pecflag = false;
            scani2c = false;
            pmbusflag = true;
            statusflag = true;
            buttonflag = true;            
            pmInterval = 1000;
            key = 0;
            Log.noticeln("Set to Default");
}
