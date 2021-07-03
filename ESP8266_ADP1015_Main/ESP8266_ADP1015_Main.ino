/*
 * 

*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PEC_ENABLE  0      //Smbus PEC(Packet Error Code) support. 1 = Enabled, 0 = Disabled.
#define PS_ADDRESS 0x70
#define PS_PARTNER_ADDRESS 0x5E
#define MSG_BUFFER_SIZE  (50)
#define UI_BUFFER_SIZE 64
#define I2C_NOSTOP 0

static struct PowerPmbus
{
  float inputV;
  float inputA;
  float inputP;
  float outputV;
  float outputA;
  float outputP;
  float temp1;
  uint16_t statusWord;
  uint8_t i2cAddr;  
}pd;

static uint8_t ver[6];
static uint8_t eepbuffer[256];
static uint8_t key = 0;
static uint8_t ps_i2c_address;
static int m24c32_address;
static bool Protocol = true;   // If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
static bool wifistatus = true;
static bool mqttflag = false;
static bool buttonflag = true;

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
const int SDA_PIN = 4;        //ESP-01S SDA = 0; SCL = 2;
const int SCL_PIN = 5;       // ESP-12F SDA = 4; SCl = 5;
const uint8_t kButtonPin = 13;
const uint8_t kLedPin = 12;

char msg[MSG_BUFFER_SIZE];
char ui_buffer[UI_BUFFER_SIZE];
unsigned long lastMsg = 0;
long count = 0;
uint16_t value = 0;
uint16_t k = 0;

WiFiClient eClient;
PubSubClient client(mqtt_server, mqtt_port, eClient);

void setup()
{ 
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(38400);
  digitalWrite(kLedPin, LOW);
  ps_i2c_address = PS_ADDRESS;
  setWifiMqtt();  
  delay(100);
  i2cdetects(0x03, 0x7F);
  Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);      
  }

void loop()
{     
  if(wifistatus) mqttLoop();
  serialread(); 
  unsigned long now = millis(); 
  if(now - lastMsg >= 666) {
      lastMsg = now;
    if(readpmbusdata()){
      if(0 != pd.statusWord) pmbusStatus();      
      if(0 == count%3) printpmbusData(pd);
      if(wifistatus && mqttflag) publishPmbusData(pd);       
      if(key == 1){
          SetVout2_46V();
          key = 0;
      }
      else if(key == 2){
          SetVout2_54V();
          key = 0;
      }
      else if(key == 3){
          SetVout2_50V();
          key = 0;
      }
      else if(key == 4){
          ReadMfrRev();
          delay(1000);
          key = 0;
        }
    }
    buttonflag = true; 
    count++;  
  }
  buttoncheck();
}
