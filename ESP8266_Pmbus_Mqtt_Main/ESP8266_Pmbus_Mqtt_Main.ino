/*ESP8266 model: ESP-01S Board SDA = 0; SCL = 2; ESP-12F Board SDA = 4; SCl = 5;
 *               HEKR 1.1 Board  SDA = 0; SCL = 13; LED = 4 Button = 14;
 *   Using the ESP8266 HEKR 1.1 Board (Purple);               
 *   https://play.google.com/store/apps/details?id=com.app.vetru.mqttdashboard  Mqtt Dashboard For  Android Iot APP
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PECENABLE  1      //Smbus PEC(Packet Error Code) support
#define PS_ADDRESS 0x58
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

static uint8_t key = 0;
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static int m24c32_address;
static uint8_t ver[6];
static uint8_t eepbuffer[256];
static bool Protocol = true;   // If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
static bool wifistatus = true;
static bool mqttflag = false;
static bool buttonflag = true;
static bool pmbuswrite = false;
static bool stbyflag = false;
static bool scani2c = true;
static bool ledstatus = true;
static bool bluestatus = false;

const char* ssid = "FAIOT";       // Enter your WiFi name
const char* password = "20212021";    // Enter WiFi password
const char* mqtt_server = "192.168.12.1";  //Local Raspberry-Pi MQTT server(&WiFi hotspot).
const uint16_t mqtt_port =  1883;
//const char *mqtt_broker = "broker.emqx.io";  // Free Public MQTT broker 
//const int mqtt_port = 1883; //There is no privacy protection for public access broker.
                              //Any device can publish and subscribe to topics on it.
const char* clientID = "zhsnpi1fdevice001";
const char* mqtt_user = "dfiot";
const char* mqtt_password = "123abc";
const int SDA_PIN = 0;        //For HEKR 1.1 Board
const int SCL_PIN = 13;
const uint8_t kButtonPin = 14;  
const uint8_t kLedPin = 4;

char ui_buffer[UI_BUFFER_SIZE];
char msg[MSG_BUFFER_SIZE];
unsigned long lastMsg = 0;
long count = 0;
uint16_t value = 0;
uint16_t k = 0;
uint8_t n = 0 ;

WiFiClient eClient;
PubSubClient client(mqtt_server, mqtt_port, eClient);

void setup() {
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(38400);
  digitalWrite(kLedPin, LOW);
  ps_i2c_address = PS_ADDRESS;
  setWifiMqtt(); 
  delay(100);
  deviceAddrset();
  getversion();
  Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);
  m24c32_address = (ps_i2c_address - 0x58) + 0x50;
  Serial.printf("EEPROMADDRESS %#02x:\n", m24c32_address); 
}

void loop() {
  if(wifistatus) mqttLoop();
  serialread();
  unsigned long now = millis(); 
  if (now - lastMsg >= 666) {
      lastMsg = now;
    if(readpmbusdata()){
      if(0 != pd.statusWord) pmbusStatus();      
      if(0 == count%3) printpmbusData(pd);
      if(wifistatus && mqttflag) publishPmbusData(pd);     
      if(key == 1){
         blueLedtest();
         key = 0;
      }
      else if(key == 2){
        blueLedOff();
        key = 0;
      }
      else if(key == 3){
        readCalibration();
        key = 0;
      }
      else if(key == 4){
        m24c32Checksum();
        key = 0;
        delay(1000);
      }
      else if(key == 5){
        getversion();
        key = 0;
        delay(1000);
      }
    } 
    count++;
    buttonflag = true;  
  }
   buttoncheck();
}
