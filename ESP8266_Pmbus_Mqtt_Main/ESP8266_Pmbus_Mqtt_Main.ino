/*ESP8266 model: ESP-01S Board SDA = 0; SCL = 2; ESP-12F Board SDA = 4; SCl = 5;
 *               HEKR 1.1 Board  SDA = 0; SCL = 13; LED = 4 Button = 14;
 *               Using the ESP8266 HEKR 1.1 Board (Purple);               
 *   https://play.google.com/store/apps/details?id=com.app.vetru.mqttdashboard  Mqtt Dashboard For  Android Iot APP
 *   Author Dafeng 2022
*/

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define TWI_BUFFER_SIZE 128
#define PEC_ENABLE  1      //Smbus PEC(Packet Error Code) support. 1 = Enabled, 0 = Disabled.
#define PEC_DISABLE  0
#define PS_ADDRESS 0x58
#define PS_I2C_ADDRESS 0x58 
#define PS_PARTNER_ADDRESS 0x5E
#define MSG_BUFFER_SIZE  (64)
#define UI_BUFFER_SIZE 100
#define I2C_NOSTOP 0

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
uint8_t smbus_data[64];
static uint8_t eepbuffer[256];
static uint8_t key = 0;
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static int eeprom_address;
static uint8_t unitname;
static uint16_t pmInterval = 500;  //PMbus refresh rate (miliseconds) 

static bool Protocol = true;   // If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
static bool wifistatus = true;
static bool mqttflag = false;
static bool buttonflag = true;
static bool scani2c = true;    //initialze i2c address, 
static bool pmbusflag = true;
static bool statusflag = true;
static bool ledstatus = true;
static bool pecflag = true;
static bool eepromsize = true;   // true (eeprom data size > 0x100). 
static bool standbyflag = false;

const char* ssid = "FAIOT";           // Enter your WiFi name
const char* password = "20212021";    // Enter WiFi password
const char* mqtt_server = "192.168.12.1";
const uint16_t mqtt_port =  1883;
//const char *mqtt_broker = "broker.emqx.io";  // Free Public MQTT broker 
//const int mqtt_port = 1883;  //There is no privacy protection for public access broker.
                              //Any device can publish and subscribe to topics on it.
const char* clientID = "zhsnpi1fdevice003";
const char* mqtt_user = "dfiot";
const char* mqtt_password = "123abc";
const int SDA_PIN = 0;         //ESP-01S Board SDA = 0; SCL = 2;   ESP8266 HEKR 1.1 Board  SDA = 0; SCL = 13
const int SCL_PIN = 13;       
//const int SDA_PIN = 4;         
//const int SCL_PIN = 5;       // ESP-12F Board SDA = 4; SCl = 5;
const uint8_t kButtonPin = 14;
const uint8_t kLedPin = 4;

char msg[MSG_BUFFER_SIZE];
char ui_buffer[UI_BUFFER_SIZE];
unsigned long previousMillis = 0;
unsigned long lastMsg = 0;
long count = 0;
uint8_t n = 0;
uint16_t value = 0;
uint16_t k = 0;

WiFiClient eClient;
PubSubClient client(mqtt_server, mqtt_port, eClient);

void setup()
{ 
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
//  Wire.setClock(50000);    // Set the I2C clock, default(100kHz);    
  digitalWrite(kLedPin, LOW);
  setWifiMqtt();  
  delay(100);
  ps_i2c_address = PS_I2C_ADDRESS;
  ps_patner_address = PS_PARTNER_ADDRESS;
  pecflag = PEC_DISABLE;
  print_memu();   
  pmbus_devices_detect();
  scani2c = false;     
  }

void loop()
{     
  if(wifistatus) mqttLoop();
  checkSerial(); 
  checkSensors();
  checkButton();
}
