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
#include "Wire.h"

#define PS_I2C_ADDRESS 0x58         // PMbus Power Supply address 0x58/B0;
#define PS_PARTNER_ADDRESS 0x5F

LT_I2CBus i2c_bus;

 volatile int key;           
 boolean pec = true;        // PEC(Packet Errot Code) support 
 //boolean pec = false; 
const uint8_t kBuzzerPin = 15;
const uint8_t kButtonPin = 2;
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

void setup()
{
  pinMode(kButtonPin, INPUT_PULLUP);
    Serial.begin(38400);              //! Initialize the serial port to the PC 38400
    print_title();
    ps_i2c_address = PS_I2C_ADDRESS;
    ps_patner_address = PS_PARTNER_ADDRESS;
        if (pec)
        {
         // pmbus->enablePec(ps_i2c_address);
          delete smbus;
          delete pmbus;
          smbus = new LT_SMBusPec();
          pmbus = new LT_PMBus(smbus);
          displayPecOn();
        }
        else 
        {
          //pmbus->disablePec(ps_i2c_address);
          delete smbus;
          delete pmbus;
          smbus = new LT_SMBusNoPec();
          pmbus = new LT_PMBus(smbus);
          displayPecOff();
        }
            print_memu();
            key  = 1;
            uint8_t result;
            if(i2c_bus.readByteData(ps_i2c_address, 0x00, &result))  //read  Page and check PMbus available
            {
              Serial.println(F("PMbus is not responding"));
              Serial.print(F("press button to continue"));
            //  while(digitalRead(kButtonPin) != 0);
            }
            Serial.println(F(" "));
            Serial.print(F("PAGE= "));
            Serial.println(result);
  }

void loop()
{
//    int n;
    if (Serial.available())                //! Checks for user input
  {
      key = read_int();                     //! Reads the user command
      Serial.print("key=");
      Serial.println(key);
  }
  switch (key)
    {
      case 1:
     print_all_volt_curr();
       
        break;
      
      case 2:
      print_all_sensors();
      
        break;
               
      case 3:
      print_all_status();
      break;

      case 4:
        
      break;

       case 5:
        i2cdetects(0x03, 0x77);
        break;

      case 6:
      if(i2c_bus.writeByteData(ps_patner_address, 0x00, 00))  //set Page 0, and check Crbus Patner vailable
        {
          Serial.println(F("Error!! No Crbus patner vailable"));
          Serial.println(F("Press the Button to return Main memu"));
          while(digitalRead(kButtonPin) != 0);
          key = 0;
           break;
          }
        setCrbus(); //set Crbus two PS in 0x01 & 0x02 Mode
        key = 7;    //read two PS share Current 
        break;
//                        
      case 7:
        displayCrbusCur();        
        break;            
        case 8:
          menu_commands();
          key = 0;
          break;           
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
    delay(1000);
    checkButton();
}


void checkButton(){
  if (digitalRead(kButtonPin) == 0){
    delay(10);
    if(digitalRead(kButtonPin) == 0){
      sound();
      key = 0;
    }
  }
}
