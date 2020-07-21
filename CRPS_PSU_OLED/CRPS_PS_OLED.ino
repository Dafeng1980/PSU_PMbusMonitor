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

#define IRPIN 22                     //Nano board pin D22
#define PS_I2C_ADDRESS 0x58         // PMbus Power Supply address 0x58/B0;
#define PS_PARTNER_ADDRESS 0x5F
IRrecv irrecv(IRPIN);
decode_results results;
U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 20, 21, /* reset=*/ U8X8_PIN_NONE);   //Nano board Pin D20 SCL, D21, SDA 
 volatile int key;           
 boolean pec = true;        // PEC(Packet Errot Code) support 
 //boolean pec = false; 
 int calval;
const uint8_t kBuzzerPin = 15;
const uint8_t kButtonPin = 24;
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
        else 
        {
          // pmbus->disablePec(PS_i2c_address);
          delete smbus;
          delete pmbus;
          smbus = new LT_SMBusNoPec();
          pmbus = new LT_PMBus(smbus);
          displayPecOff();
        }
            print_memu();
            calval = 0;
            key  = 1;
  }

void loop()
{
  int n;
  switch (key)
    {
      case 1:
      //print_all_status();
      print_all_volt_curr();
       // print_all_volt_curr();
//        while(digitalRead(kButtonPin) != 0);
//        sound();
//        delay(500);
//        print_all_status();
//        while(digitalRead(kButtonPin) != 0);
//        sound();
//        delay(500);
        break;
      
      case 2:

//      displaycurrent();
//     while(digitalRead(kButtonPin) != 0);
//     sound();
//     delay(500); 
//     setCrbus();
//     while(digitalRead(kButtonPin) != 0);
//     sound();
//     delay(500); 
//      displaycurrent();
//     while(digitalRead(kButtonPin) != 0);
//     sound();
//     delay(500); 
//     key = 0;
      print_all_status();
        break;
               
      case 3:
//        float val;
//        val = readeout();
//      u8g2.clearBuffer();
//      u8g2.setFontMode(1);
//      u8g2.setFont(u8g2_font_8x13_tr);    
//      u8g2.setCursor(00,26);
//      u8g2.print(F("EOUT= "));
//      u8g2.print(val, 2);
//      u8g2.sendBuffer();
//      while(digitalRead(kButtonPin) != 0);
//      sound();
//        pmbus->clearFaults(ps_i2c_address);
//      u8g2.clearBuffer();
//      u8g2.setFontMode(1);
//      u8g2.setFont(u8g2_font_8x13_tr);    
//      u8g2.setCursor(0, 30);
//      u8g2.print(F("ClearFaults"));  
//      u8g2.sendBuffer();
//        while(digitalRead(kButtonPin) != 0);
//        sound();
        getSerial();
        while(digitalRead(kButtonPin) != 0);
        sound();
        delay(500);      
        getId();
        while(digitalRead(kButtonPin) != 0);
        sound();
        delay(500);
        key = 1;
           // print_all_status();
        break;
//           case 3:
//        factoryMode();
//        setwritefru();
//        writeserial();
//        //setvoltagecal(true);   // TRUE is Vmain ; FALSE is Vsb  
//        while(digitalRead(kButtonPin) != 0);
//        sound();
//        delay(100);
//        exitFactoryMode();
//        key = 0;              
//        break;
//      case 3:
//        factoryMode();
//        setvoltagecal(true);   // TRUE is Vmain ; FALSE is Vsb  
//        delay(100);
//        key = 7;              
//        break;
//                        
//      case 7:
//        u8g2.clearBuffer();
//        u8g2.setCursor(0,13);
//        u8g2.print(F("Up_Step"));
//        u8g2.sendBuffer(); 
//        calval = readir();
//        calVoltage(calval);
//          sound();       
//          u8g2.clearBuffer();    
//          u8g2.setCursor(8,13);
//          u8g2.print(F("Up_Step Frist"));
//          u8g2.sendBuffer();
//          while(digitalRead(kButtonPin) != 0);  // 
//            sound();
//            calval = readir();
//            calVoltage(calval);
//              sound();
//              u8g2.clearBuffer();   
//              u8g2.setCursor(8,13);
//              u8g2.print(F("Up_Step Second"));
//              u8g2.sendBuffer();
//              while(digitalRead(kButtonPin) != 0);  //           
//                sound();
//                u8g2.clearBuffer();
//                u8g2.setCursor(0,13);
//                u8g2.print(F("choose 7 to up"));        
//                u8g2.setCursor(0,38);
//                u8g2.print(F("choose 8 to down"));
//                u8g2.setCursor(0,64);
//                u8g2.print(F("choose 9 to exit"));
//                u8g2.sendBuffer();
//                delay(500);
//                  n = readir();
//                    if( n == 7 || n == 8)
//                      key = n;
//                    else 
//                      key = 9;                    
//            break;
//            
//        case 8:
//          u8g2.clearBuffer();
//          u8g2.setCursor(0,13);
//          u8g2.print(F("Down_Step"));
//          u8g2.sendBuffer();
//          calval = 0 - readir();
//          calVoltage(calval);
//            sound();
//            u8g2.clearBuffer(); 
//            u8g2.setCursor(0,13);
//            u8g2.print(F("Down_Step Frist"));
//            u8g2.sendBuffer();
//            while(digitalRead(kButtonPin) != 0);
//              sound();
//              calval = 0 - readir();
//              calVoltage(calval);
//                sound();
//                u8g2.clearBuffer();
//                u8g2.setCursor(0,13);
//                u8g2.print(F("Down_Step Second"));
//                u8g2.sendBuffer();
//                while(digitalRead(kButtonPin) != 0);  //           
//                  sound();
//                  u8g2.clearBuffer();
//                  u8g2.setCursor(0,13);
//                  u8g2.print(F("choose 7 to up"));        
//                  u8g2.setCursor(0,38);
//                  u8g2.print(F("choose 8 to down"));
//                  u8g2.setCursor(0,64);
//                  u8g2.print(F("choose 9 to exit"));
//                  u8g2.sendBuffer();
//                  delay(500);
//                    n = readir();
//                      if( n == 7 || n == 8)
//                        key = n;
//                      else 
//                        key = 9;  
//            break;
            
        case 9:
          //  exitFactoryMode();
              key = 0;
            break;           
          case 0:
              print_memu();
              break;             
          default:
              key = 0;
              break;
      }
    delay(500);
    checkButton();
    detectIR();
}


void checkButton(){
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
         case 0xA10:
            key = 6;         
            break;
         case 0x610:
            key = 7;         
            break;
         case 0xE10:
            key = 8;         
            break;
         case 0x110:
            key = 9;         
            break;                        
         case 0x910:
            key = 0;         
            break;
         }
            irrecv.resume();
         }  
}
