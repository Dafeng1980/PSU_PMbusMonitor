 
 #include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PEC_ENABLE  1      //Smbus PEC(Packet Error Code) support
#define PS_ADDRESS 0x58
#define ATS_ADDRESS 0x60
#define PS_PARTNER_ADDRESS 0x5E
#define MSG_BUFFER_SIZE  (50)
#define UI_BUFFER_SIZE 64
#define I2C_NOSTOP 0

static struct PowerPmbus
{
  float inputV;
  float inputVs1;
  float inputVs2;
  float inputA;
  float inputP;
  float outputV;
  float outputVcap;
  float outputA;
  float outputP;
  float fanSpeed;
  float fanSpeed2;
  float temp1;
  float temp2;
  float temp3;
  float temp4;
  float temp5;
  float temp6;
  uint16_t statusWord;
  uint16_t atsStatusWord;
  uint16_t chgStatusWord;
  uint16_t dchStatusWord;
  uint16_t ioutrawData;
  uint16_t vsenrawData;
  uint16_t vorirawData;
  uint8_t i2cAddr;  
}pd;

uint8_t n = 0;
char ui_buffer[UI_BUFFER_SIZE];
static uint8_t key = 0;
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
static uint8_t eepbuffer[256];
static int m24c32_address;
static uint8_t ats_i2c_address;

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
static boolean scani2c = true;
static bool wifistatus = true;
static bool wiset = true;
static bool psustatus = false;
static bool buttonflag = true;

WiFiClient eClient;
PubSubClient client(mqtt_server, mqtt_port, eClient);

void setup() {
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(38400);
  digitalWrite(kLedPin, LOW);
  ps_i2c_address = PS_ADDRESS;
  ats_i2c_address = ATS_ADDRESS;
  delay(10);
  setupWifi();
  if(wifistatus){
    client.setCallback(callback);
    client.connect(clientID, mqtt_user, mqtt_password);
    Serial.println("MQTT Broker Connected. \n ");
  }  
  delay(100);
  deviceAddrset();
 //getversion();
 Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);
// m24c32_address = (ps_i2c_address - 0x58) + 0x50;
// Serial.printf("EEPROMADDRESS %#02x:\n", m24c32_address); 
}

void loop() {
  char readval;
    
  if (wifistatus){
  if (!client.connected()) {
    reconnect();
  }    
    client.loop();
    client.subscribe("pmbus/set");
 } 
  
  if (Serial.available())  {              //! Serial input read
      readval = read_char();              //! Reads the user command
    //      Serial.println((char)readval);
    if ((char)readval == '0')      key = 0;
    else if ((char)readval == '1') key = 1;
    else if ((char)readval == '2') key = 2;
    else if ((char)readval == '3') key = 3;
    else if ((char)readval == '4') key = 4;
    else if ((char)readval == '5') key = 5;
    else if ((char)readval == '6') key = 6;
    else if ((char)readval == '7') key = 7;
    else if ((char)readval == '8') key = 8;
    else if ((char)readval == '9') key = 9;
    else if ((char)readval == 'O' || (char)readval == 'o')
    {
        if(!psustatus){
                  psOn();
                  digitalWrite(kLedPin, HIGH);
                  psustatus = true;
                  Serial.println("PSU ON. \n ");
                  delay(100);
                }
        else{
                  Serial.println("PSU ON STATUS. \n ");
                }   
    }
    else if ((char)readval == 'R' || (char)readval == 'r')
    {
        if(psustatus){
                  psustatus = false;
                  digitalWrite(kLedPin, LOW);                  
                  Serial.println("PSU RESTAR ON. \n ");
                  delay(100);
                }
        else{
                  Serial.println("PSU OFF STATUS. \n ");
                }
            count = 0;       
    }
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
            ATSreading();
            ATSvincheck();
            if(count >= 1000) key = 0;           
      }
      else if(key == 2){
            ATSvin_rawdata();
            vsenor_rawdata();
            if(count >= 2000) key = 0;
      }
      else if(key == 3){
            pmbus_writeProtect(ats_i2c_address, 0x00);
            pmbus_writeProtect(ps_i2c_address, 0x00);
            delay(50);
            for (uint8_t i = 0; i < 10; i++)
            {
              AtsBlackbox(i);
              delay(50);
              PsuBlackbox(i);
              delay(50);
            }
          AtsBlackbox(0xff);
          delay(50);
          PsuBlackbox(0xff);
          delay(6000);
          key = 0;
      }
      else if(key == 4){
        Serial.println("PSU = 4. \n ");
        key = 0;
      }
    }
    buttonflag = true; 
    count++;  
  }
  if (digitalRead(kButtonPin) == 0 && buttonflag){
      delay(10);
        if(digitalRead(kButtonPin) == 0){
            if(!psustatus){
                psOn();
                digitalWrite(kLedPin, HIGH);
                psustatus = true;
                Serial.println("PSU ON. \n ");
                delay(200);
              }
            else{
              Serial.println("PSU ON STATUS. \n ");
              key++;
              if (key >= 10) key = 0;
              Serial.printf("\n Key= %#01d:\n", key);
            }
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
  Serial.println();
//  String topicStr(topic);
//  if (topicStr.compareTo("pmbus/set") == 0)
    if ((char)payload[0] == '0') key = 0;
    else if ((char)payload[0] == '1') key = 1;
    else if ((char)payload[0] == '2') key = 2;
    else if ((char)payload[0] == '3') key = 3;
    else if ((char)payload[0] == '4') key = 4;
    else if ((char)payload[0] == '5') key = 5;
    else if ((char)payload[0] == '6') key = 6;
    else if ((char)payload[0] == '7') key = 7;
    else if ((char)payload[0] == '8') key = 8;
    else if ((char)payload[0] == '9') key = 9;
    else if ((char)payload[0] == 'o' || (char)payload[0] == 'O')
    {
            if(!psustatus){
                  psOn();
                  digitalWrite(kLedPin, HIGH);
                   psustatus = true;
                  Serial.println("PSU ON. \n ");
                  delay(100);
            }
        }
    else if ((char)payload[0] == 'R' || (char)payload[0] == 'r')
    {
           if(psustatus){
                  psustatus = false;
                  digitalWrite(kLedPin, LOW);                  
                  Serial.println("PSU RESTAR ON. \n ");
                  delay(100);
                }
                count = 0;
    }
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
