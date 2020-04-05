#include "Pmbus_config.h"

void displayPecOn(){
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_8x13_tr);    
      u8g2.setCursor(30, 30);
      u8g2.print(F(" PEC On"));  
      u8g2.sendBuffer();
      Serial.println(F("PEC On"));
}

void displayPecOff(){
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_8x13_tr);
      u8g2.setCursor(30,30);
      u8g2.print(F(" PEC Off"));
      u8g2.setCursor(0, 40);
      u8g2.sendBuffer();
      Serial.println(F("PEC Off"));
}

void sound(){
          tone(kBuzzerPin, 2100);
            delay(100);
             noTone(kBuzzerPin);
}

void alarm(){     
         for (int i = 0; i < 16; i++){
              tone(kBuzzerPin, 2100);
              delay(100);
              noTone(kBuzzerPin);
              delay(200);         
          }
}

void print_title()
{
  Serial.print(F("\n*****************************************\n"));
  Serial.print(F("* PSU-ADP1015 PMbus control demo        *\n"));
  Serial.print(F("* This program demonstrates how to sent *\n"));
  Serial.print(F("* and receive data form PSU      *\n"));
  Serial.print(F("*****************************************\n"));
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.setCursor(0,10);
      u8g2.print(F("PMbus control Setting "));
      u8g2.setCursor(0,30);
      u8g2.print(F("Baud Rate is 115200"));
      u8g2.sendBuffer();
        delay(1500); 
}

void print_memu()
{
       u8g2.clearBuffer();
       u8g2.setFontMode(1);
       u8g2.setFont(u8g2_font_6x10_tr);    
       u8g2.setCursor(0,10);
       u8g2.print(F("1-Read Vol&Cur&Tem"));
       u8g2.setCursor(0, 30);
       u8g2.print(F("2-Read Status"));
       u8g2.setCursor(0, 60);
       u8g2.print(F("0-Main Menu:"));
       u8g2.sendBuffer();
       Serial.print(F("\n  1-Read Volt and Curr and Temp\n"));
       Serial.print(F("  2-Read Status\n"));
       Serial.print(F("  0-Mai Menu\n"));
       Serial.print(F("\nEnter SONY-IR command:"));
            delay(500);
}


void print_all_volt_curr_temp()
{
    float current,voltage,temp;
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.setCursor(0,10);
      voltage = pmbus->readVin(ps_i2c_address, false);
      current = pmbus->readIin(ps_i2c_address, false);
            u8g2.print(F("IN: "));
            u8g2.setCursor(24,10);
            u8g2.print(F("V="));
            u8g2.print(voltage, 1);
            u8g2.setCursor(78,10);
            u8g2.print(F("C="));
            u8g2.print(current, 2);
                  voltage = pmbus->readVout(ps_i2c_address, false);
                  current = pmbus->readIout(ps_i2c_address, false);
                        u8g2.setCursor(0,30);
                        u8g2.print(F("OT: "));
                        u8g2.setCursor(24,30);
                        u8g2.print(F("V="));
                        u8g2.print(voltage, 3);
                        u8g2.setCursor(78,30);
                        u8g2.print(F("C="));
                        u8g2.print(current, 2);
                        u8g2.setCursor(0,60);
                              temp = pmbus->readOtemp(ps_i2c_address);           //temp sensor 0x8D  Inlet Air
                              u8g2.print(F("Primary Temp "));
                              u8g2.print(temp, 1);
                              Serial.print(F("Primary Temp "));
                              Serial.println(temp, 1);
                              u8g2.sendBuffer();
}

void print_all_status()
{
    uint16_t w_val;
    uint8_t io,in,tm,fa,vo;
    float temp;
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x10_tr);  
      w_val = pmbus->readStatusWord(ps_i2c_address);
          u8g2.setCursor(0,10);
          u8g2.print(F("STATUS WORD 0x"));
          u8g2.print(w_val, HEX);
          u8g2.setCursor(0,20);
          u8g2.print(F("LOW  "));      
          u8g2.print((w_val & 0xFF), BIN);
          u8g2.setCursor(0,30);
          u8g2.print(F("HIGH  "));
          u8g2.print((w_val >> 8), BIN);

            // io = pmbus -> readStatusIout(ps_i2c_address);
            // in = pmbus -> readStatusInput(ps_i2c_address);
            // tm = pmbus -> readStatusTemp(ps_i2c_address);
            // fa = pmbus -> readStatusFan(ps_i2c_address);
            //  vo = pmbus -> readStatusVout(ps_i2c_address);
          u8g2.sendBuffer();
}

void ReadMfrRev(){
  int address;
  uint8_t values[6];
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_8x13_tr); 
      smbus -> writeWord(ps_i2c_address, 0xD7, 0xFFFF);
      smbus -> writeWord(ps_i2c_address, 0xD7, 0xFFFF);
      smbus -> writeByte(ps_i2c_address, 0xD2, 0x06);
      smbus -> writeWord(ps_i2c_address, 0xD3, 0x0016);
            address = ps_i2c_address;
            LT_Wire.beginTransmission(address);
            LT_Wire.write(0xB4);
            LT_Wire.endTransmission(false);
            LT_Wire.beginTransmission(address);
            LT_Wire.requestFrom(address, values, 0x06);
            u8g2.setCursor(0,13);
            u8g2.print(F("MFR_REV"));
            u8g2.setCursor(0,26);
            u8g2.print(values[1]);
            u8g2.print(values[2]);
            u8g2.print(values[3]);
            u8g2.print(values[4]);
            u8g2.print(values[5]);
            u8g2.print(values[6]);
            u8g2.sendBuffer();
                  delay(6000);
                  smbus -> writeWord(ps_i2c_address, 0xD7, 0x0055);
}

void SetVout2_46V(){
   float current,voltage;
   u8g2.clearBuffer();
   u8g2.setFontMode(1);
   u8g2.setFont(u8g2_font_8x13_tr); 
      smbus -> writeWord(ps_i2c_address, 0x21, 0x5C00);
      delay(2000);
      voltage = pmbus->readVout(ps_i2c_address, false);
      current = pmbus->readIout(ps_i2c_address, false);
        u8g2.setCursor(0,13);
        u8g2.print(F("SET2_46V:"));
        u8g2.setCursor(0,26);
        u8g2.print(F("VOUT= "));
        u8g2.print(voltage, 3);
        u8g2.setCursor(0,39);
        u8g2.print(F("COUT= "));
        u8g2.print(current, 2);
        u8g2.sendBuffer();
}

void SetVout2_50V(){
   float current,voltage;
   u8g2.clearBuffer();
   u8g2.setFontMode(1);
   u8g2.setFont(u8g2_font_8x13_tr); 
      smbus -> writeWord(ps_i2c_address, 0x21, 0x6400);
      delay(2000);
      voltage = pmbus->readVout(ps_i2c_address, false);
      current = pmbus->readIout(ps_i2c_address, false);
        u8g2.setCursor(0,13);
        u8g2.print(F("SET2_50V:"));
        u8g2.setCursor(0,26);
        u8g2.print(F("VOUT= "));
        u8g2.print(voltage, 3);
        u8g2.setCursor(0,39);
        u8g2.print(F("COUT= "));
        u8g2.print(current, 2);
        u8g2.sendBuffer();
}

void SetVout2_54V(){
   float current,voltage;
   u8g2.clearBuffer();
   u8g2.setFontMode(1);
   u8g2.setFont(u8g2_font_8x13_tr); 
      smbus -> writeWord(ps_i2c_address, 0x21, 0x6C00);
      delay(2000);
      voltage = pmbus->readVout(ps_i2c_address, false);
      current = pmbus->readIout(ps_i2c_address, false);
        u8g2.setCursor(0,13);
        u8g2.print(F("SET2_54V:"));
        u8g2.setCursor(0,26);
        u8g2.print(F("VOUT= "));
        u8g2.print(voltage, 3);
        u8g2.setCursor(0,39);
        u8g2.print(F("COUT= "));
        u8g2.print(current, 2);
        u8g2.sendBuffer();
}

