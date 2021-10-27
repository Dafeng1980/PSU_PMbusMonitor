/*!
 * * The Smbus.ino Pmbus.ino inspired By below link
 * https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-development-platforms/linduino.html
 * #include <Wire.h>
 * Need to modify "Wire.h", Wire.cpp", added the Wire.requestFromS to Support SMbus Wire.requestFrom.
 * in folder "%USERPROFILE%\AppData\Local\Arduino15\packages\MegaCore\hardware\avr\2.1.3\libraries\Wire\src"
 *Board ATmega128L;  External 7.3728Mhz@upload baud-rate:115200;
*/

#include <Wire.h>
#include <U8g2lib.h>
#include <IRremote.h>

#define IRPIN 22                     //Nano board pin D22
#define PS_I2C_ADDRESS 0x58         // PMbus Power Supply address 0x58/B0;
#define PS_PARTNER_ADDRESS 0x5F
#define TWI_BUFFER_SIZE 128
#define PEC_ENABLE  1          //Smbus PEC(Packet Error Code) support. 1 = Enabled, 0 = Disabled.
#define UI_BUFFER_SIZE 64
#define I2C_NOSTOP 0

IRrecv irrecv(IRPIN);
decode_results results;
U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 20, 21, /* reset=*/ U8X8_PIN_NONE);   //Nano board Pin D20 SCL, D21, SDA 

const uint8_t kBuzzerPin = 15;
const uint8_t kButtonPin = 24;
const uint8_t kLedPin = 13;
const uint16_t kPmInterval = 1500;  //PMbus refresh rate (miliseconds) 

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
static int eeprom_address;
static uint8_t unitname;
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

void setup()
{
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, LOW); 
    u8g2.begin();
    Serial.begin(38400);         //! Initialize the serial port to the PC 115200
    irrecv.enableIRIn();
    print_title();
    ps_i2c_address = PS_I2C_ADDRESS;
    ps_patner_address = PS_PARTNER_ADDRESS;
            print_memu();
            key  = 0;
  }

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= kPmInterval){
        previousMillis = currentMillis;
        if(readpmbusdata()){
//            if(0 != pd.statusWord && statusflag) pmbusStatus();      
//            if(0 == count%5) printpmbusData(pd);
            if(key == 1){
//                monitorstatus();
                key = 0;
            }
            else if(key == 2){
                key = 0;
            }
            else if(key == 3){
//               standbystatus();
               key = 0;
            }
            else if(key == 4){
               Serial.println(F("TBD "));
                key = 0;
                delay(100);
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

bool readpmbusdata()
{   
      bool ret = true;
      if(!pmbusflag) return ret = false;
      if(smbus_waitForAck(ps_i2c_address, 0x00) == 0) {  //0x00 PAGE read
          Serial.println("PMBUS Polling Fail, Type \'h\' for Help \n");      
          return ret = false;
        }
     pd.i2cAddr = ps_i2c_address;
     pd.inputV = pmbus_readVin(ps_i2c_address);
     pd.inputA = pmbus_readIin(ps_i2c_address);
     pd.outputV = pmbus_readVout(ps_i2c_address);
     pd.outputA = pmbus_readIout(ps_i2c_address);
     pd.inputP = pmbus_readPin(ps_i2c_address);
     pd.outputP = pmbus_readPout(ps_i2c_address);
     pd.statusWord = pmbus_readStatusWord(ps_i2c_address);
     
     if(unitname > 0)  {
     pd.temp1 = pmbus_readOtemp(ps_i2c_address);           //temp sensor 0x8D
     if(unitname == 2) {  
     pd.temp2 = pmbus_readItemp(ps_i2c_address);        //temp sensor 0x8E  
     pd.temp3 = pmbus_readMtemp(ps_i2c_address);        //temp sensor 0x8F  
     pd.fanSpeed = pmbus_readFanSpeed1(ps_i2c_address);
     }    
     if(standbyflag){
     pmbus_setPage(ps_i2c_address,1);                    //set Page to 1, read 12Vsb 
     pd.outputVsb = pmbus_readVout(ps_i2c_address);
     pd.outputAsb = pmbus_readIout(ps_i2c_address);
     pmbus_setPage(ps_i2c_address,0);
     }
   }      
     delay(20);     
}
