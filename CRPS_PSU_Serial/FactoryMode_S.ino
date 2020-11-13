
void calVoltage(int caidata){
  uint16_t caival;
  caival = (uint16_t)caidata;
  smbus -> writeWord(ps_i2c_address, 0xFB, caival); 
}

void factoryMode(){
  smbus -> writeWord(ps_i2c_address, 0xF9, 0x0000);
  smbus -> writeWord(ps_i2c_address, 0xF8, 0xABCD);
  delay(50);
  Serial.println(F("Enter_Factory_Mode*"));
}

void exitFactoryMode(){
  smbus -> writeWord(ps_i2c_address, 0xF9, 0x0000);
  smbus -> writeWord(ps_i2c_address, 0xF8, 0x0000);
    delay(50);
  Serial.println(F("Exit_Factory_Mode*"));
}

void setCrbus(){
 // smbus -> writeByte(ps_i2c_address, 0xD0, 0x00);
 smbus -> writeByte(ps_i2c_address, 0xD0, 0x01);
  delay(50);
 smbus -> writeByte(ps_patner_address, 0xD0, 0x02);  //
  delay(50);
  Serial.println(F("CRBUS_SET_OK"));
}

void getFwRev(){
  uint8_t ver[3];
  smbus -> readBlock(ps_i2c_address, 0xD9, ver, 3);
         Serial.print(F("MFR_REV: "));
         Serial.print(ver[0]);
         Serial.print(" ");
         Serial.print(ver[1]);
         Serial.print(" ");
         Serial.print(ver[2]);
         Serial.println(" ");
            delay(100);
}

void getSerial(){
  uint8_t ver[14];
  char dataString[15];
  smbus -> readBlock(ps_i2c_address, 0x9E, ver, 14);
        for (byte i =0; i < 14; i++){
          dataString[i] = ver [i];
        }
        dataString[14] = '\0';
        Serial.print(F("    MFR_SERIAL: "));
        Serial.println(dataString);
          delay(100);
}

void getModel(){
  uint8_t ver[15];
  char dataString[16];
  smbus -> readBlock(ps_i2c_address, 0x9A, ver, 15);
        for (byte i =0; i < 15; i++){
          dataString[i] = ver [i];
        }
        dataString[15] = '\0';
        Serial.print(F("      MFR_MOSEL: "));
        Serial.println(dataString);
          delay(100);
}

void getRevision(){
  uint8_t ver[2];
  char dataString[3];
  smbus -> readBlock(ps_i2c_address, 0x9B, ver, 2);
        for (byte i =0; i < 2; i++){
          dataString[i] = ver [i];
        }
        dataString[2] = '\0';
        Serial.print(F(" MFR_REVISION: "));
        Serial.println(dataString);
          delay(100);
}

void getLocation(){
  uint8_t ver[16];
  char dataString[17];
  smbus -> readBlock(ps_i2c_address, 0x9C, ver, 16);
        for (byte i =0; i < 16; i++){
          dataString[i] = ver [i];
        }
        dataString[16] = '\0';
        Serial.print(F("MFR_LOCATION: "));
        Serial.println(dataString);
          delay(100);
}

void getDate(){
  uint8_t ver[8];
  char dataString[9];
  smbus -> readBlock(ps_i2c_address, 0x9D, ver, 8);
        for (byte i =0; i < 8; i++){
          dataString[i] = ver [i];
        }
        dataString[8] = '\0';
        Serial.print(F("        MFR_DATE: "));
        Serial.println(dataString);
          delay(100);
}


void writeserial(){
  uint8_t data[14] = {0x01,0x05,0x05,0x06,0x06,0x07,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E};
  smbus -> writeBlock(ps_i2c_address, 0x9E, data, 14);
  delay(100);
}
void getId(){
  uint8_t ver[14];
  char dataString[15];
  smbus -> readBlock(ps_i2c_address, 0x99, ver, 14);
 
      for (byte i =0; i < 14; i++){
          dataString[i] = ver [i];
        }
        dataString[14] = '\0';
        Serial.print(F("              MFR_ID: "));
        Serial.println(dataString);
        delay(1000);
}

void setwritefru(){
      smbus -> writeWord(ps_i2c_address, 0xF9, 0x0200);
      delay(10);
     Serial.println(F("FruWrite"));
}

void setvoltagecal(bool mors){
  if (mors){
    smbus -> writeWord(ps_i2c_address, 0xF9, 0x0002);
     Serial.print(F("12Vmain"));
  } 
  else{
     smbus -> writeWord(ps_i2c_address, 0xF9, 0x0013);

     Serial.print(F("12Vsb"));
  }
  Serial.println(F("  FactoyVoltageCal"));
}

void displayCrbusCur(){
    float current; 
        Serial.println(F("  "));       
  current = pmbus->readIout(ps_i2c_address, false);
        Serial.print(F("B0_Cur= "));
        Serial.print(current, 2);
  current = pmbus->readIout(ps_patner_address, false);
        Serial.print(F("    BE_Cur= "));
        Serial.println(current, 2);
}

void reset05_06Cal()
    {
      smbus -> writeWord(ps_i2c_address, 0xF9, 0x0005);
      smbus -> writeWord(ps_i2c_address, 0xFA, 0x0400);
      smbus -> writeWord(ps_i2c_address, 0xFB, 0x0000);
      delay(50);
      smbus -> writeWord(ps_i2c_address, 0xF9, 0x0006);
      smbus -> writeWord(ps_i2c_address, 0xFA, 0x0400);
      smbus -> writeWord(ps_i2c_address, 0xFB, 0x0000);
      delay(50);
    }
