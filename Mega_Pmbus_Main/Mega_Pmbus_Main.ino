/*!
 * The Smbus.ino Pmbus.ino inspired By below link
 * https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-development-platforms/linduino.html
 *  #include <Wire.h>
 * Need to modify "Wire.h", Wire.cpp", added the Wire.requestFromS to Support SMbus Wire.requestFrom.
 * in folder "%USERPROFILE%\AppData\Local\Arduino15\packages\MegaCore\hardware\avr\2.1.3\libraries\Wire\src"
 * 
 * 
 * Board ATmega128L;  External Clock@7.3728Mhz upload baud-rate:115200;
*/
#include <Wire.h>

#define TWI_BUFFER_SIZE 128
#define PEC_ENABLE  1          //Smbus PEC(Packet Error Code) support. 1 = Enabled, 0 = Disabled.
#define UI_BUFFER_SIZE 64
#define I2C_NOSTOP 0
#define PS_I2C_ADDRESS 0x58         // PMbus Power Supply address 0x58/B0;
//#define PS_I2C_ADDRESS 0x29       // PMbus Power Supply address 0x29/52;
//#define PS_I2C_ADDRESS 0x70
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
static uint8_t eepbuffer[256];
static uint8_t key = 0;
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static int m24c32_address;
static bool Protocol = true;   // If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
static bool buttonflag = true;
static bool scani2c = true;    //initialze i2c address, 
static bool ledstatus = true;
static bool statusflag = true;
static bool pecflag = true;

char ui_buffer[UI_BUFFER_SIZE];
unsigned long previousMillis = 0;
long count = 0;
uint8_t n = 0 ;
        
const uint8_t kBuzzerPin = 15;
const uint8_t kButtonPin = 2;
const uint8_t kLedPin = 13;
const uint16_t kPmInterval = 333;  //PMbus refresh rate (miliseconds) 

void setup()
{
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  Serial.begin(38400);   //! Initialize the serial port to the PC 38400
  Wire.begin();
  //Wire.setClock(50000);    // Define the I2C clock, default is 100kHz;
  digitalWrite(kLedPin, LOW);  
  ps_i2c_address = PS_I2C_ADDRESS;
  ps_patner_address = PS_PARTNER_ADDRESS;
  pecflag = PEC_ENABLE;      
  print_memu();
  pmbus_devices_detect();
  Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);
    key  = 0;
  }

void loop()
{  
  serialread();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= kPmInterval){
        previousMillis = currentMillis;
        if(readpmbusdata()){
            if(0 != pd.statusWord && statusflag) pmbusStatus();      
            if(0 == count%5) printpmbusData(pd);
            if(key == 1){
                monitorstatus();
                key = 0;
            }
            else if(key == 2){
              //  SetVout2_54V();
                key = 0;
            }
            else if(key == 3){
               // SetVout2_50V();
                key = 0;
            }
            else if(key == 4){
               // ReadMfrRev();
                key = 0;
                delay(5000);
              }
            else if(key == 5){
                key = 0;
              }
            else if(key == 6){
                key = 0;
              }
            else if(key == 7){
                key = 0;
              }
            else if(key == 8){
                key = 0;
              }
            else if(key == 9){
                key = 0;
              }        
          }
          buttonflag = true; 
          count++;
          if(scani2c) i2cdetects(0x00, 0x7F);  
        }
     checkButton();
}
