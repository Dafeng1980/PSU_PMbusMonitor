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
//#define PS_I2C_ADDRESS 0x5B 
#define PS_PARTNER_ADDRESS 0x59

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
 int key;
 unsigned long previousMillis = 0;
 uint8_t n = 0 ;
 long seq = 0;           
 boolean pec = true;        // PEC(Packet Errot Code) support 
 //boolean pec = false; 
 boolean scani2c = true;    //initialze i2c address, 
// boolean scani2c = false;
boolean ledstatus = true;
const uint8_t kBuzzerPin = 15;
const uint8_t kButtonPin = 2;
const uint8_t kLedPin = 13;
const uint16_t kPmInterval = 600;  //PMbus refresh rate (miliseconds) 

static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

void setup()
{
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  Serial.begin(38400);              //! Initialize the serial port to the PC 38400
  // Serial1.begin(38400);             //Using the Serial1 port to the ESP8266  
  digitalWrite(kLedPin, LOW);  
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
          Serial.println(F("PEC On"));
        }
      else 
        {
          //pmbus->disablePec(ps_i2c_address);
          delete smbus;
          delete pmbus;
          smbus = new LT_SMBusNoPec();
          pmbus = new LT_PMBus(smbus);
          Serial.println(F("PEC Off"));
         }       
  print_memu();
  delay(100);
 // scani2c = false;
  if (digitalRead(kButtonPin) == 0){
    delay(10);
    if(digitalRead(kButtonPin) == 0){
      buzzing();
      scani2c = false;
    }
  }
//   uint8_t result;
// if(i2c_bus.readByteData(ps_i2c_address, 0x00, &result))  //read  Page and check PMbus available
//            {
//              Serial.println(F("PMbus is not responding"));
//            }

 while(scani2c){
    digitalWrite(kLedPin, HIGH);
    pmbusdetects();
    delay(50);
    digitalWrite(kLedPin, LOW);
    delay(210);
    if(n > 0) break;
    }
    Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);
    key  = 1;
  }

void loop()
{  
  if (Serial.available())                //! Checks for user input
  {
      key = read_int();                     //! Reads the user command
      Serial.print("key=");
      Serial.println(key);
  }

 unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= kPmInterval){
  previousMillis = currentMillis;
  readpmbusdata();    
  switch (key)
    {
      case 1:
      printpmbusData( pd );       
      break;
      
      case 2:
      printpowerSensors();
      
      break;
               
      case 3:
      printpowerStatus();
      break;

      case 4:
       //iOutFan();
      // setFanFullSpeed();
       //calibration_data_read();
      // calibration_write_counter();
       Serial.println(F("READ_OK"));
       delay(100);
       Serial.print(F("Press button to continue"));
       while(digitalRead(kButtonPin) != 0);
       Serial.println(F(" "));
       key = 1;
       break;

       case 5:
       i2cdetects(0x03, 0x7F);
       break;

      case 6:
      if(i2c_bus.writeByteData(ps_patner_address, 0x00, 00))  //set Page 0, and check Crbus Patner vailable
        {
          Serial.println(F("Error!! No Crbus patner vailable"));
          Serial.println(F("Press the Button to return Main memu"));
          while(digitalRead(kButtonPin) != 0);
          buzzing();
          key = 0;
           break;
          }
        setCrbus(); //set Crbus two PS in 0x01 & 0x02 Mode
        key = 7;    //read two PS share Current 
        break;                       
        case 7:
       // displayCrbusCur();        
        break;            
        case 8:
        mfr_menu_commands();
          key = 0;
          break;           
        case 9:
          key = 0;
            break;           
          case 0:
              print_memu();
              break;             
          default:
              key = 0;
              break;
      }
      seq++;
      if(seq%3==0) {
        if(ledstatus){
        digitalWrite(kLedPin, HIGH);
        ledstatus = false;
        }
        else{
          digitalWrite(kLedPin, LOW);
          ledstatus = true;
        }
      }
  }
    checkButton();
}

void checkButton(){
  if (digitalRead(kButtonPin) == 0){
    delay(10);
    if(digitalRead(kButtonPin) == 0){
      buzzing();
      key = 0;
    }
  }
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
    //  if (address == 92) error = 0;
      if (error == 0) {
        // device found
        n++;
        if(n == 1) ps_i2c_address = address;
        else ps_i2c_address = PS_I2C_ADDRESS;
        if(n == 2) ps_patner_address = address;
        else ps_patner_address = PS_PARTNER_ADDRESS;
        Serial.printf(" %02x", address);       
      } else if (error == 4) {
        // other error
        Serial.print(" XX");
      } else {
        // error = 2: received NACK on transmit of address
        // error = 3: received NACK on transmit of data
        Serial.print(" --");
        delay(20);
      } 
  }
  Serial.println("\n");
  Serial.println(n);
}

void readpmbusdata()
{   
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
}
