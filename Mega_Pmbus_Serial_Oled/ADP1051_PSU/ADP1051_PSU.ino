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
#include "Pmbus_config.h"

void setup()
{
  pinMode(kButtonPin, INPUT_PULLUP);
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
    checkButton();
    detectIR();
        switch (key)
          {
            case 1:
            print_all_volt_curr_temp();
              break;
            case 2:
            ReadMfrRev();
            key = 1;
           // print_all_status();
              break;
            case 3:
           // print_all_status();
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
         case 0x910:
            key = 0;         
            break;
         }
            irrecv.resume();
         }  
}
