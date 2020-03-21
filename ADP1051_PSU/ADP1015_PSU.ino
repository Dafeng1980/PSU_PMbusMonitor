/*!
Copyright 2018(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.
/*! @file
    @ingroup DC1962
*/
#include <Arduino.h>
#include <U8g2lib.h>
#include <stdint.h>
#include <Linduino.h>
#include <UserInterface.h>
#include <LT_Wire.h>
#include <LT_PMBus.h>
#include <LT_SMBusPec.h>
#include <LT_PMBusMath.h>
#include <LT_SMBus.h>
#include <LT_I2CBus.h>
#include <LT_SMBusGroup.h>
#include <LT_FaultLog.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusBase.h>
#include <IRremote.h>

#define IRPIN 22                     //Nano board pin 22
#define PS_I2C_ADDRESS 0x70         // PMbus Power Supply address

IRrecv irrecv(IRPIN);
decode_results results;
U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 20, 21, /* reset=*/ U8X8_PIN_NONE);   //Nano board Pin D20 SCL, S21, SDA 
// Global variables
volatile int key;           
boolean pec = true;        // PEC(Packet Errot Code) support 

const int kBuzzerPin = 15;
static uint8_t ps_i2c_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

void setup()
{
  u8g2.begin();
  Serial.begin(115200);         //! Initialize the serial port to the PC 115200
  irrecv.enableIRIn();
     print_title();
      ps_i2c_address = PS_I2C_ADDRESS;
          if (pec)
       {
         //pmbus->enablePec(PS_i2c_address);
            delete smbus;
            delete pmbus;
            smbus = new LT_SMBusPec();
            pmbus = new LT_PMBus(smbus);
            displayPecOn();
        }
          else {
            // pmbus->disablePec(PS_i2c_address);
                delete smbus;
                delete pmbus;
                smbus = new LT_SMBusNoPec();
                pmbus = new LT_PMBus(smbus);
                displayPecOff();
                }
            delay(500);
            print_memu();
            key  = 1;
  }

void loop()
{
    IrDetect();
        switch (key)
          {
            case 1:
            print_all_volt_curr_temp();
              break;
            case 2:
            print_all_status();
              break;
            case 3:
            print_all_status();
              break;
            case 4:
              break;
            case 5:
              break;
            case 0:
              print_memu();
              break;
            default:
              key = 0;
              break;
          }
            delay(1000);
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
//          u8g2.setCursor(0, 20);
//          u8g2.print(F("2-Read AllSensor"));
          u8g2.setCursor(0, 30);
          u8g2.print(F("2-Read Status"));
          u8g2.setCursor(0, 60);
          u8g2.print(F("0-Main Menu:"));
          u8g2.sendBuffer();
              Serial.print(F("\n  1-Read Volt and Curr and Temp\n"));
             // Serial.print(F("  2-Read All Sensors\n"));
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
//    voltage = pmbus->readVin(ps_i2c_address, false);
//    current = pmbus->readIin(ps_i2c_address, false);
//        u8g2.print(F(" INPUT: "));
//        u8g2.setCursor(0,20);
//        u8g2.print(F("Volt "));
//        u8g2.print(voltage, 1);
//        u8g2.setCursor(72,20);
//        u8g2.print(F("Cur "));
//        u8g2.print(current, 2);
//    voltage = pmbus->readVout(ps_i2c_address, false);
//    current = pmbus->readIout(ps_i2c_address, false);
//        u8g2.setCursor(0,30);
//        u8g2.print(F(" OUTPUT: "));
//        u8g2.setCursor(0,40);
//        u8g2.print(F("Volt "));
//        u8g2.print(voltage, 3);
//        u8g2.setCursor(66,40);
//        u8g2.print(F(" Cur "));
//        u8g2.print(current, 2);
//     voltage = pmbus->readPin(ps_i2c_address, false);
//     current = pmbus->readPout(ps_i2c_address, false);
//        u8g2.setCursor(0,50);
//        u8g2.print(F("Pin "));
//        u8g2.print(voltage, 1);
//        u8g2.setCursor(66,50);
//        u8g2.print(F("Pout "));
//        u8g2.print(current, 1);
            u8g2.setCursor(0,60);
            temp = pmbus->readOtemp(ps_i2c_address);           //temp sensor 0x8D  Inlet Air
            u8g2.print(F("Secondary Temp "));
            u8g2.print(temp, 1);
            Serial.print(F("Secondary Temp "));
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

      io = pmbus -> readStatusIout(ps_i2c_address);
      in = pmbus -> readStatusInput(ps_i2c_address);
      tm = pmbus -> readStatusTemp(ps_i2c_address);
      fa = pmbus -> readStatusFan(ps_i2c_address);
      vo = pmbus -> readStatusVout(ps_i2c_address);
          u8g2.sendBuffer();
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
      u8g2.setCursor(0,10);
      u8g2.setCursor(30,30);
      u8g2.print(F(" PEC Off"));
      u8g2.setCursor(0, 40);
      u8g2.sendBuffer();
      Serial.println(F("PEC Off"));
}
void IrDetect(){
  if (irrecv.decode(&results)) {
         sound();
   // Serial.println(results.value, HEX);
       switch (results.value) {
         case 0x010:
            key = 1;
            break;           
         case 0x810:
            key = 2;         
            break;
         case 0x410:
            key = 3;         
            break;
         case 0xC10:
            key = 4;         
            break;
         case 0x210:
            key = 5;         
            break;
         case 0x910:
            key = 0;         
            break;
         }
            irrecv.resume();
         }  
}
