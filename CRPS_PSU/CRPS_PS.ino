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
#define PS_I2C_ADDRESS 0x58         // PMbus Power Supply address
#define PS_PARTNER_ADDRESS 0x5F       
IRrecv irrecv(IRPIN);
decode_results results;
U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 20, 21, /* reset=*/ U8X8_PIN_NONE);   //Nano board Pin D20 SCL, S21, SDA 
// Global variables
volatile int key;           
 //boolean pec = true;        // PEC(Packet Errot Code) support 
 boolean pec = false; 

const int kBuzzerPin = 15;
const int kButtonPin = 24;
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

void setup()
{
  pinMode(kButtonPin, INPUT_PULLUP);
  u8g2.begin();
  Serial.begin(115200);         //! Initialize the serial port to the PC 115200
  irrecv.enableIRIn();
     print_title();
      ps_i2c_address = PS_I2C_ADDRESS;
      ps_patner_address = PS_PARTNER_ADDRESS;
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
    checkButton();
    detectIR();
        switch (key)
          {
            case 1:
            print_all_volt_curr();
              break;
            case 2:
           // print_all_sensors();
            displaycurrent();
              break;
            case 3:
            CrbusSetA();
            key = 2;
           // print_all_status();
              break;
            case 4:
            key = 0;
              break;
            case 5:
            CrbusSet();
            key = 2;
              break;
            case 0:
              print_memu();
              break;
            default:
              key = 0;
              break;
          }
            delay(100);
  }

void print_title()
{
  Serial.print(F("\n*****************************************\n"));
  Serial.print(F("* PSU     PMbus control demo        *\n"));
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
          u8g2.print(F("1-Read Volt&Curr"));
          u8g2.setCursor(0, 20);
          u8g2.print(F("2-Read AllSensor"));
          u8g2.setCursor(0, 30);
          u8g2.print(F("3-Read Status"));
          u8g2.setCursor(0, 60);
          u8g2.print(F("0-Main Menu:"));
          u8g2.sendBuffer();
              Serial.print(F("\n  1-Read Voltages and Currents\n"));
              Serial.print(F("  2-Read All Sensors\n"));
              Serial.print(F("  3-Read Status\n"));
              Serial.print(F("  0-Mai Menu\n"));
              Serial.print(F("\nEnter SONY-IR command:"));
                delay(500);
    }
    
void print_all_volt_curr()
{
  float   current,voltage;
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
        u8g2.setCursor(0,20);
        u8g2.print(F("OT: "));
        u8g2.setCursor(24,20);
        u8g2.print(F("V="));
        u8g2.print(voltage, 3);
        u8g2.setCursor(78,20);
        u8g2.print(F("C="));
        u8g2.print(current, 2);
     voltage = pmbus->readPin(ps_i2c_address, false);
     current = pmbus->readPout(ps_i2c_address, false);
        u8g2.setCursor(0,60);
        u8g2.print(F("Pin="));
        u8g2.print(voltage, 1);
        u8g2.setCursor(66,60);
        u8g2.print(F("Pout="));
        u8g2.print(current, 1);
        u8g2.sendBuffer();
}

void print_all_status()
{
    uint16_t w_val;
    uint8_t io,in,tm,fa,vo;
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
  void print_all_sensors()
  {
      float temp,fspeed;
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x10_tr);    
      u8g2.setCursor(0,10);
        temp = pmbus->readOtemp(ps_i2c_address);           //temp sensor 0x8D  Inlet Air
          u8g2.print(F("Inlet Air  Temp "));
          u8g2.print(temp, 1);
          Serial.print(F("Inlet Air Temp "));
          Serial.println(temp, 1);
           temp = pmbus->readItemp(ps_i2c_address);            //0x8E Hotspot1 Sensor located on secondary side of DC/DC stage
            u8g2.setCursor(0,20);
            u8g2.print(F("DC/DC stag Temp "));
            u8g2.print(temp, 1);
            Serial.print(F("DC/DC stage Temp "));
            Serial.println(temp, 1);
              temp = pmbus->readtemp3(ps_i2c_address);          // 0x8F Hotspot2 Sensor located on primary heat sink
                 u8g2.setCursor(0,30);
                 u8g2.print(F("P heatsink Temp "));
                 u8g2.print(temp, 1);
                 Serial.print(F("P heatsink Temp "));
                 Serial.println(temp, 1);
                  temp = pmbus->readtemp1(ps_i2c_address);
                    u8g2.setCursor(0,40);
                    u8g2.print(F("MFR D3 Temp "));
                    u8g2.print(temp, 1);
                    Serial.print(F("MFR D3 Temp "));
                    Serial.println(temp, 1);
                      temp = pmbus->readtemp2(ps_i2c_address);
                        u8g2.setCursor(0,50);
                        u8g2.print(F("MFR D4 Temp "));
                        u8g2.print(temp, 1);
                        Serial.print(F("MFR D4 Temp "));
                        Serial.println(temp, 1);
                          fspeed = pmbus->readFanSpeed1(ps_i2c_address);
                            u8g2.setCursor(0,60);
                            u8g2.print(F("Fan1 Speed  "));
                            u8g2.print(fspeed, 1);
                            u8g2.sendBuffer();
                            Serial.print(F("Fan1 Speed  "));
                            Serial.println(fspeed, 1);
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
<<<<<<< HEAD
void checkButton(){
=======

void CrbusSetA(){
 // smbus -> writeByte(ps_i2c_address, 0xD0, 0x00);
  smbus -> writeByte(ps_i2c_address, 0xD0, 0x01);
  delay(10);
  smbus -> writeByte(ps_patner_address, 0xD0, 0x02);
  // delay(50);
   u8g2.clearBuffer();
   u8g2.setFontMode(1);
   u8g2.setFont(u8g2_font_8x13_tr);    
   u8g2.setCursor(0,26);
   u8g2.print(F("CRBUS_SET_OK"));
   u8g2.sendBuffer();
}

void CrbusSet(){
 // smbus -> writeByte(ps_i2c_address, 0xD0, 0x00);
  smbus -> writeByte(ps_i2c_address, 0xD0, 0x00);
  delay(10);
 smbus -> writeByte(ps_patner_address, 0xD0, 0x02);
  // delay(50);
   u8g2.clearBuffer();
   u8g2.setFontMode(1);
   u8g2.setFont(u8g2_font_8x13_tr);    
   u8g2.setCursor(0,26);
   u8g2.print(F("CRBUS_SET_OK"));
   u8g2.sendBuffer();
}

void displaycurrent(){
    float current;
     u8g2.clearBuffer();
     u8g2.setFontMode(1);
     u8g2.setFont(u8g2_font_8x13_tr);
        
  current = pmbus->readIout(ps_i2c_address, false);
          u8g2.setCursor(0,13);
          u8g2.print(F("B0_Cur= "));
        u8g2.print(current, 2);
  current = pmbus->readIout(ps_patner_address, false);
        u8g2.setCursor(0,39);
        u8g2.print(F("BE_Cur= "));
        u8g2.print(current, 2);
        u8g2.sendBuffer(); 
}
void ButtonDetect(){
>>>>>>> 096465924f78bb14f8b749760788ff8334128d50
  if (digitalRead(kButtonPin) == 0){
    delay(10);
    if(digitalRead(kButtonPin) == 0){
      sound();
      key++;
    }
  }
}
void detectIR(){
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
