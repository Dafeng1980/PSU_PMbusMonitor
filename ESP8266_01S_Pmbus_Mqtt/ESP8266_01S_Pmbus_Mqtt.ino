#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PECENABLE  1      //Smbus PEC(Packet Error Code) support
#define PS_ADDRESS 0x58
#define PS_PARTNER_ADDRESS 0x5E
#define MSG_BUFFER_SIZE  (50)
#define UI_BUFFER_SIZE 64

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

uint8_t n = 0 ;
char ui_buffer[UI_BUFFER_SIZE];
static uint8_t key = 0;
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
const char* ssid = "FAIOT";       // Enter your WiFi name
const char* password = "20212021";    // Enter WiFi password
const char* mqtt_server = "192.168.12.1";
const uint16_t mqtt_port =  1883;
//const char *mqtt_broker = "broker.emqx.io";  // Free Public MQTT broker 
//const int mqtt_port = 1883; //There is no privacy protection for public access broker.
                              //Any device can publish and subscribe to topics on it.
const char* clientID = "zhsnpi1fdevice001";
const char* mqtt_user = "dfiot";
const char* mqtt_password = "123abc";

const int SDA_PIN = 0;        //ESP-01S SDA = 0; SCL = 2;
const int SCL_PIN = 2;       // ESP-12F SDA = 4; SCl = 5;

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
static bool pmbuswrite = false;
static bool stbyflag = false;

WiFiClient eClient;
PubSubClient client(mqtt_server, mqtt_port, eClient);


void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(38400);
  ps_i2c_address = PS_ADDRESS;
  delay(10);
  setupWifi();
  if(wifistatus){
  client.setCallback(callback);
  client.connect(clientID, mqtt_user, mqtt_password);
  Serial.println("MQTT Broker Connected. \n ");
  }  
  delay(100);
 // scani2c = false;
  i2cdetects(0x03, 0x7F);
  
  while(scani2c){
    pmbusdetects();
    delay(50);
    if(n > 0) break;
    }    
 Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);
 
  if(smbus_waitForAck(ps_i2c_address, 0x00))
  {
     readFw_version(ps_i2c_address, ver);
     readBootloader_version(ps_i2c_address, ver+3);
     Serial.printf("\n FW_REV: %02x%02x%02x \n", ver[0],ver[1],ver[2]);
     Serial.printf("\n Bootloader_REV: %02x%02x%02x \n", ver[3],ver[4],ver[5]);
    if(wifistatus){ 
     snprintf (msg, MSG_BUFFER_SIZE, "FW_REV: %02x%02x%02x",ver[0],ver[1],ver[2]);
     client.publish("pmbus/fru/HWversion", msg);
     snprintf (msg, MSG_BUFFER_SIZE, "BL_REV: %02x%02x%02x",ver[3],ver[4],ver[5]);
     client.publish("pmbus/fru/BLversion", msg);
    }
  } 
}

void loop() {
  char readval;
  if (wifistatus){
  if (!client.connected()) {
    reconnect();
  }
    client.subscribe("pmbus/set");
    client.loop();
 } 

    if (Serial.available())                //! Serial input read
  {
      readval = read_char();                     //! Reads the user command
      Serial.println((char)readval);
    if ((char)readval == '0'){
      key = 0;
      Serial.println(": key = 0 :");
    }
    if ((char)readval == '1'){
      key = 1;
      Serial.println(": key = 1 :");
    }
    if ((char)readval == '2'){
      key = 2;
      Serial.println(": key = 2 :");     
    }
    if ((char)readval == '3'){
      key = 3;
      Serial.println(": key = 3 :");     
    }
    if ((char)readval == 'w'){
      pmbuswrite = true;
      Serial.println(": Write Enable :");     
    }
  } 
  
  unsigned long now = millis(); 
  if (now - lastMsg >= 333) {
      lastMsg = now;
    if(readpmbusdata()){
      if(0 != pd.statusWord) pmbusStatus();      
      if(0 == count%5) printpmbusData(pd);
      if(wifistatus) publishPmbusData(pd);
      if(key == 1){
        read_calibrationOutputvolt();
        delay(100);
        read_calibrationCount();
        delay(100);
        key = 0;
        if(wifistatus){
          client.publish("pmbus/set", "0");      
        }
      }
      if(key == 2){
        pmbus_blueLed(ps_i2c_address, 0x01);
        Serial.println(": Blue LED On :");
        key = 0;
      }
      if(key == 3){
        pmbus_blueLed(ps_i2c_address, 0x00);
        Serial.println(": Blue LED OFF :");
        key = 0;
      }
    } 
    count++;  
  }
}

void setupWifi(){
  delay(10);
  wiset = true;
  wifistatus = true;
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    k++;
    Serial.print(".");
    if( k >= 30){
    wiset = false;
    wifistatus = false;
    Serial.printf("\n Wi-Fi Not Connect.. \n");
    break;
    }
  }
  if(wiset){
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  }
  else{
    Serial.println("");
    Serial.println("WiFi connect Failed");
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
  String topicStr(topic);
  if (topicStr.compareTo("pmbus/set") == 0)
  {
    if ((char)payload[0] == '0'){
        key = 0;
        Serial.println(": key = 0 :");
    }
    if ((char)payload[0] == '1'){
        key = 1;
        Serial.println(": key = 1 :");
    }
    if ((char)payload[0] == '2'){
        key = 2;
        Serial.println(": key = 2 :");
    }
    if ((char)payload[0] == '3'){
        key = 3;
        Serial.println(": key = 3 :");
    }
    if ((char)payload[0] == 'w'){
        pmbuswrite = true;
        Serial.println(": Write Enable :");
    }
    delay(10);
  }  
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
