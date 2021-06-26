/*
 * 

*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PEC_ENABLE  1      //Smbus PEC(Packet Error Code) support
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

char ui_buffer[UI_BUFFER_SIZE];
static uint8_t key = 0;
static uint8_t ps_i2c_address;
static uint8_t eepbuffer[256];
static int m24c32_address;

const char* ssid = "FAIOT";           // Enter your WiFi name
const char* password = "20212021";    // Enter WiFi password
const char* mqtt_server = "192.168.12.1";
const uint16_t mqtt_port =  1883;
//const char *mqtt_broker = "broker.emqx.io";  // Free Public MQTT broker 
//const int mqtt_port = 1883;  //There is no privacy protection for public access broker.
                              //Any device can publish and subscribe to topics on it.
const char* clientID = "zhsnpi1fdevice001";
const char* mqtt_user = "dfiot";
const char* mqtt_password = "123abc";

const int SDA_PIN = 4;        //ESP-01S SDA = 0; SCL = 2;
const int SCL_PIN = 5;       // ESP-12F SDA = 4; SCl = 5;
const uint8_t kButtonPin = 13;
const uint8_t kLedPin = 12;

unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
uint16_t value = 0;
long count = 0;
uint16_t k = 0;

static uint8_t ver[6];
static boolean Protocol = true;   // If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
static bool wifistatus = true;
static bool wiset = true;
static bool buttonflag = true;

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
  delay(10);
  setupWifi();
  if(wifistatus){
    client.setCallback(callback);
    client.connect(clientID, mqtt_user, mqtt_password);
    Serial.println("MQTT Broker Connected. \n ");
  }  
    delay(100);
    i2cdetects(0x03, 0x7F);
    Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);          
}

void loop()
{
  char readval;  
  if (wifistatus){
      if (!client.connected()) {
        reconnect();
    }    
      client.loop();
      client.subscribe("pmbus/set");
 } 
  if (Serial.available())                //! Serial input read
  {
      readval = read_char();                     //! Reads the user command
      if ((char)readval == '0')      key = 0;
      else if ((char)readval == '1') key = 1;
      else if ((char)readval == '2') key = 2;
      else if ((char)readval == '3') key = 3;
      else if ((char)readval == '4') key = 4;
      else key = 0;       
      Serial.printf("\n Key= %#01d:\n", key);
 }
  
  unsigned long now = millis(); 
  if (now - lastMsg >= 333) {
      lastMsg = now;
    if(readpmbusdata()){
        if(0 != pd.statusWord) pmbusStatus();      
        if(0 == count%5) printpmbusData(pd);
        if(wifistatus) publishPmbusData(pd);
            
        if(key == 1){
            SetVout2_46V;
            key = 0;
          }
        else if(key == 2){
            SetVout2_54V;
            key = 0;
          }
        else if(key == 3){
            SetVout2_50V;
            key = 0;
          }
        else if(key == 4){
            ReadMfrRev();
            key = 0;
          }
      }
    buttonflag = true; 
    count++;  
  }

  if (digitalRead(kButtonPin) == 0 && buttonflag){
        delay(10);
        if(digitalRead(kButtonPin) == 0){
              key++;
              if (key >= 5) key = 0;
              Serial.printf("\n Key= %#01d:\n", key);
              buttonflag = false;
       }
    }
 }


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
  }
    if ((char)payload[0] == '0') key = 0;
    else if ((char)payload[0] == '1') key = 1;
    else if ((char)payload[0] == '2') key = 2;
    else if ((char)payload[0] == '3') key = 3;
    else if ((char)payload[0] == '4') key = 4;
    else key = 0;
    Serial.printf("\n Key= %#01d:\n", key);
}

void reconnect() {
  // Loop until we're reconnected
  //client.connect(clientID, mqtt_user, mqtt_password);
  while (!client.connected()) {
     Serial.print("Attempting MQTT connection..."); // Attempt to connect   
     String clientId = "ESP8266Client-";
     clientId += String(random(0xffff), HEX);
    if (client.connect((clientId.c_str()), mqtt_user, mqtt_password)) {
     // if (client.connect(clientId.c_str())) {
      Serial.println("connected to broker");
      client.publish("outTopic", "hello world");  // Once connected, publish an announcement...   
      client.subscribe("inTopic"); // ... and resubscribe
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");      
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}
