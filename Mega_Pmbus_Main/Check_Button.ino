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
    n = 0;
    delay(100);
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
      } 
      else if (error == 4) {              // other error        
        Serial.print(" XX");
      } 
      else {                             // error = 2: received NACK on transmit of address        
        Serial.print(" --");             // error = 3: received NACK on transmit of data        
      }
      delay(20);
  }
  Serial.println("\n");
  Serial.println(n);
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

void print_memu()
        {                   
           Serial.print(F("PSU PMbus Debug Tools (Smbus Protocol)\n"));
           Serial.print(F("Monitoring PSU Status etc. and Detect i2c Devices\n"));
           Serial.print(F("Send the Smbus Protocol Command.\n"));
           Serial.println(F("\nEnter command:"));
           printhelp();
           Serial.println(F(" "));
        }
