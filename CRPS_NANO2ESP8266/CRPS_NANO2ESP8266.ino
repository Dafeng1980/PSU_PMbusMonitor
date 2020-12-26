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
 * Board ATmega128L;  External 8Mhz@upload baud-rate:38400;
*/
#include <Arduino.h>
#include <stdint.h>
#include <UserInterface.h>
#include <LT_Wire.h>
#include <LT_PMBus.h>
#include <LT_SMBusPec.h>
#include <LT_PMBusMath.h>
#include <LT_SMBus.h>
#include <LT_I2CBus.h>
#include <LT_SMBusGroup.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusBase.h>
#include "Wire.h"

#define PS_I2C_ADDRESS 0x58         // PMbus Power Supply address 0x58/B0;
#define PS_PARTNER_ADDRESS 0x5F

LT_I2CBus i2c_bus;

static struct PowerPmbus
{
  float inputV;
  float inputA;
  float inputP;
  float outputV;
  float outputVsb;
  float outputA;
  float outputAsb;
  float outputP;
  float fanSpeed;
  float temp1;
  float temp2;
  float temp3;
  uint16_t statusWord;
  uint8_t i2cAddr;  
}pd;
        
const uint8_t kBuzzerPin = 15;
const uint8_t kButtonPin = 2;
const uint8_t kLedPin = 13;
const uint8_t kInterruptPin = 22;

static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static LT_SMBus *smbus = new LT_SMBusPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);
uint8_t n = 0 ;
long seq = 0;
boolean scani2c = true;

void setup()
{
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  pinMode(kInterruptPin, OUTPUT);
  digitalWrite(kLedPin, LOW);
  digitalWrite(kInterruptPin, HIGH);
  Serial.begin(38400);              //! Initialize the serial port to the PC 38400
  Serial1.begin(38400);             //Using the Serial1 port to the ESP8266    
  ps_i2c_address = PS_I2C_ADDRESS;
//  ps_patner_address = PS_PARTNER_ADDRESS;
if (digitalRead(kButtonPin) == 0){
    delay(10);
    if(digitalRead(kButtonPin) == 0){
      buzzing();
      scani2c = false;
    }
  }
while(scani2c){
  digitalWrite(kLedPin, HIGH);
  pmbusdetects();
  //delay(30);
  digitalWrite(kLedPin, LOW);
  delay(230);
  if(n > 0) break;
}
Serial.printf("\n%#02x:", ps_i2c_address);
delay(500);
  }

void loop()
{
    if(readpmbusdata()){
      Serial.print(F("Read Data: fail.\n"));
    }
    else{
          sent2esp8266();
          printpmbusData(pd);         
    }

    if(seq%2) digitalWrite(kLedPin, HIGH);
    seq++;
    delay(1000);
    digitalWrite(kLedPin, LOW);
}

void buzzing(){
         tone(kBuzzerPin, 2200);
          delay(50);
           noTone(kBuzzerPin);
    }

void sent2esp8266(){
    uint8_t foo[sizeof(struct PowerPmbus)];
      //struct myStruct x; /* populate */
    memcpy(foo, &pd, sizeof pd);
    digitalWrite(kInterruptPin, LOW);
    digitalWrite(kInterruptPin, HIGH);
    Serial1.write(foo, 51);
    for (int i = 0; i < 51; i++){
          Serial.printf(" %02x", foo[i]);
          delay(2);
      }
   Serial.println(" ");
   Serial.println("Data Sent to ESP8266.");
}


void pmbusdetects(){
  uint8_t i, address, error;
  // table header
  Serial.print("   ");
  for (i = 8; i < 16; i++) {
    Serial.printf("%3x", i);
  }
  Serial.printf("\n%#02x:", 0x50);
  for (address = 88; address <= 95; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
     // if (address == 91) error = 0;
      if (error == 0) {
        // device found
        n++;
        if(n == 1) ps_i2c_address = address;
        else ps_i2c_address = PS_I2C_ADDRESS;
        Serial.printf(" %02x", address);       
      } else if (error == 4) {
        // other error
        Serial.print(" XX");
      } else {
        // error = 2: received NACK on transmit of address
        // error = 3: received NACK on transmit of data
        Serial.print(" --");
      }
      delay(15); 
    }
  Serial.println("\n");
  Serial.println(n);
}

int8_t readpmbusdata()
  {
      int8_t ret = 1;
      LT_Wire.beginTransmission(ps_i2c_address);
      LT_Wire.write(0x00);
      ret = LT_Wire.endTransmission(false);
    
     pd.i2cAddr = ps_i2c_address;
     pd.inputV = pmbus->readVin(ps_i2c_address, false);
     pd.inputA = pmbus->readIin(ps_i2c_address, false);
     pd.outputV = pmbus->readVout(ps_i2c_address, false);
     pd.outputA = pmbus->readIout(ps_i2c_address, false);
     pd.inputP = pmbus->readPin(ps_i2c_address, false);
     pd.outputP = pmbus->readPout(ps_i2c_address, false);
     pd.temp1 = pmbus->readOtemp(ps_i2c_address);           //temp sensor 0x8D  
     pd.temp2 = pmbus->readItemp(ps_i2c_address);        //temp sensor 0x8E  
     pd.temp3 = pmbus->readtemp3(ps_i2c_address);        //temp sensor 0x8F  
     pd.fanSpeed = pmbus->readFanSpeed1(ps_i2c_address);
     pd.statusWord = pmbus->readStatusWord(ps_i2c_address); 
     pmbus->setPage(ps_i2c_address,1);                    //set Page to 1, read 12Vsb 
     pd.outputVsb = pmbus->readVout(ps_i2c_address, false);
     pd.outputAsb = pmbus->readIout(ps_i2c_address, false);
     pmbus->setPage(ps_i2c_address,0); 
     delay(20);
     return ret;    
}

void printpmbusData(struct PowerPmbus busData)
{
    Serial.print(F("PMBUS ADDRESS: 0x"));
    Serial.println(busData.i2cAddr, HEX);
    Serial.println(F("INPUT: "));
    Serial.print(F("Volt : "));
    Serial.print(busData.inputV, 2);
    Serial.print(F("V"));
    Serial.print(F("     Curr : "));
    Serial.print(busData.inputA, 3);
    Serial.println(F("A"));
    Serial.println(F(" "));
    Serial.println(F("OUTPUT: "));
    
    Serial.print(F("Vmain_Volt: "));
    Serial.print(busData.outputV, 3);
    Serial.print(F("V"));
    Serial.print(F("    Curr: "));
    Serial.print(busData.outputA, 3);
    Serial.println(F("A"));
    
    Serial.print(F("Vsb_Volt: "));
    Serial.print(busData.outputVsb, 3);
    Serial.print(F("V"));
    Serial.print(F("    Sb_Curr: "));
    Serial.print(busData.outputAsb, 3);
    Serial.println(F("A"));
    
    Serial.print(F("Pin: "));
    Serial.print(busData.inputP, 2);
    Serial.print(F("W"));
    Serial.print(F("    Pout: "));
    Serial.print(busData.outputP, 2);
    Serial.println(F("W"));
    Serial.println(F(" "));
    
          Serial.print(F("Temperature   8D: "));
          Serial.print(busData.temp1, 0);
          Serial.print(F("C"));
          Serial.print(F("    8E: "));
          Serial.print(busData.temp2, 0);
          Serial.print(F("C"));
          Serial.print(F("    8F: "));
          Serial.print(busData.temp3, 0);
          Serial.println(F("C"));
          Serial.print(F("  Fan1 Speed: "));
          Serial.print(busData.fanSpeed, 1);
          Serial.println(F(" rpm"));
          Serial.println(F(" "));
    Serial.print(F("STATUS WORD 0x"));
    Serial.println(busData.statusWord, HEX);
    
//    Serial.print(F("  0B "));
//    printBits((busData.statusWord & 0xFF));
//    Serial.print(F("   LOW: 0x"));
//    Serial.println((busData.statusWord & 0xFF), HEX);
//
//    Serial.print(F("  0B "));
//    printBits((busData.statusWord >> 8));
//    Serial.print(F("   HIGH: 0x"));
//    Serial.println((busData.statusWord >> 8), HEX);

    Serial.println(F(" ************ PMBUS DATA  ************ "));
    Serial.println(F(" "));    
}
