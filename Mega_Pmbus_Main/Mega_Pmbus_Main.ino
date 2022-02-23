 /*!
 * The Smbus.ino Pmbus.ino inspired By below link
 * https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-development-platforms/linduino.html
 * #include <Wire.h>
 * Need to modify "Wire.h", Wire.cpp", added the Wire.requestFromS to Support SMbus Wire.requestFrom.
 * in folder "%USERPROFILE%\AppData\Local\Arduino15\packages\MegaCore\hardware\avr\2.1.3\libraries\Wire\src"
 * add the link to boards Manager URLs:  https://mcudude.github.io/MegaCore/package_MCUdude_MegaCore_index.json and intalled the MegaCore Boaeds.
 * Board ATmega128L;  External Clock@7.3728Mhz upload baud-rate:115200;
 * Author: Dafeng 2020
*/
#include <Wire.h>
#define TWI_BUFFER_SIZE 128
#define PEC_ENABLE  1          //Smbus PEC(Packet Error Code) support. 1 = Enabled, 0 = Disabled.
#define UI_BUFFER_SIZE 100
#define I2C_NOSTOP 0
#define PS_I2C_ADDRESS 0x58         // PMbus Power Supply address 0x58/B0;
#define PS_PARTNER_ADDRESS 0x5B

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

static uint8_t ver[6];
uint8_t smbus_data[32];
static uint8_t eepbuffer[256];
static uint8_t key = 0;
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static int eeprom_address;
static uint8_t unitname;
static uint16_t pmInterval = 1500;  //PMbus refresh rate (miliseconds) 

static bool Protocol = true;   // If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
static bool buttonflag = true;
static bool scani2c = true;    //initialze i2c address, 
static bool ledstatus = true;
static bool statusflag = true;
static bool pecflag = true;
static bool pmbusflag = true;
static bool standbyflag = false;
static bool eepromsize = true;   // true (eeprom data size > 0x100). 

char ui_buffer[UI_BUFFER_SIZE];
unsigned long previousMillis = 0;
long count = 0;
uint8_t n = 0 ;
        
const uint8_t kBuzzerPin = 15;
const uint8_t kButtonPin = 2;
const uint8_t kLedPin = 13;

void setup()
{
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, LOW); 
  Serial.begin(38400);    //! Initialize the serial port to the PC 38400
  Wire.begin();
//  Wire.setClock(50000);    // Set the I2C clock, default(100kHz);   
  ps_i2c_address = PS_I2C_ADDRESS;
  ps_patner_address = PS_PARTNER_ADDRESS;
  pecflag = PEC_ENABLE;      
  print_memu();
  pmbus_devices_detect();
  eeprom_address = 0x50 + (ps_i2c_address & 0x07);
  key  = 0;
  unitname = 0;
 }

void loop()
{  
  checkSerial();
  readSensors();   
  checkButton();
}
