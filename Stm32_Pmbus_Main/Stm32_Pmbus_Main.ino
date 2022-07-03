/*!
 * The Smbus.ino Pmbus.ino inspired By below link
 * https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-development-platforms/linduino.html
 * #include <Wire.h>
 * add the link to boards Manager URLs: https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json.
 * Board Stm32nucleo MB1136 Stm32F3/ Stm32G4
 * Author: Dafeng 2022
*/
#include <Wire.h>
#include <ArduinoLog.h>

// #define TWI_BUFFER_SIZE 128
#define PEC_ENABLE  0          //Smbus PEC(Packet Error Code) support. 1 = Enabled, 0 = Disabled.
#define PEC_DISABLE  0
#define UI_BUFFER_SIZE 100
#define I2C_NOSTOP 0
#define PS_I2C_ADDRESS 0x58         // PMbus Power Supply address 0x58/B0;
#define PS_PARTNER_ADDRESS 0x5B
#define MSG_BUFFER_SIZE  (64)
#define LOG_LEVEL LOG_LEVEL_VERBOSE

static struct PowerPmbus
{
  float inputV;
  float inputA;
  float inputP;
  float inputE;
  float outputV;
  float outputVsb;
  float outputA;
  float outputAsb;
  float outputP;
  float outputE;
  float fanSpeed;
  float temp1;
  float temp2;
  float temp3;
  uint16_t statusWord;
  uint8_t i2cAddr;  
}pd;

uint8_t smbus_data[32];
static uint8_t eepbuffer[256];
static uint8_t key = 0;
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static int eeprom_address;
static uint16_t pmInterval = 1000;  //PMbus refresh rate (miliseconds) 

static bool Protocol = true;   // If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
static bool buttonflag = true;
static bool scani2c = true;    //initialze i2c address, 
static bool dataflag = true;
static bool ledstatus = true;
static bool statusflag = true;
static bool smbuscomun = false;
static bool pecflag = true;
static bool pmbusflag = true;
static bool standbyflag = false;
static bool eepromsize = true;   // true (eeprom data size > 0x100). 
       bool expandengery = false;
       bool expandsensor = false;

const char hex_table[]{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

char msg[64];
char ui_buffer[UI_BUFFER_SIZE];
unsigned long previousMillis = 0;
long count = 0;
uint16_t value = 0;
        
//const uint8_t kBuzzerPin = 19;
const uint8_t kButtonPin = 30;
const uint8_t kLedPin = 21;

int debounce = 20; // ms debounce period to prevent flickering when pressing or releasing the button
int DCgap = 200; // max ms between clicks for a double click event
int holdTime = 2000; // ms hold period: how long to wait for press+hold event
int longHoldTime = 5000; // ms long hold period: how long to wait for press+hold event
             // Other button variables
boolean buttonVal = HIGH; // value read from button
boolean buttonLast = HIGH; // buffered value of the button's previous state
boolean DCwaiting = false; // whether we're waiting for a double click (down)
boolean DConUp = false; // whether to register a double click on next release, or whether to wait and click
boolean singleOK = true; // whether it's OK to do a single click
long downTime = -1; // time the button was pressed down
long upTime = -1; // time the button was released
boolean ignoreUp = false; // whether to ignore the button release because the click+hold was triggered
boolean waitForUp = false; // when held, whether to wait for the up event
boolean holdEventPast = false; // whether or not the hold event happened already
boolean longHoldEventPast = false;// whether or not the long hold event happened already

void setup()
{
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, LOW);  
//  Serial.setRx(PA3); 
//  Serial.setTx(PA2);
  Serial.begin(38400);    //! Initialize the serial port to the PC 38400
  Serial.println("wo yao money");
  delay(200);
  Log.begin(LOG_LEVEL, &Serial, false);  //
//  Wire.setSDA(PB7); 
//  Wire.setSCL(PB6); // using pin number PYn
  Wire.begin();
//  Wire.setClock(50000);    // Set the I2C clock, default(100kHz); 
    defaultint(); 
    pmbus_devices_init();  
    printhelp(); 
 }

void loop()
{  
   checkButton();
   checkSerial();
   checkSensors();
}
