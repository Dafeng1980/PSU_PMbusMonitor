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

#ifndef pmbus_config_h
#define pmbus_config_h

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

volatile int key;           
// boolean pec = true;        // PEC(Packet Errot Code) support
boolean pec = false; 

const int kBuzzerPin = 15;
const int kButtonPin = 24;
static uint8_t ps_i2c_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);




#endif