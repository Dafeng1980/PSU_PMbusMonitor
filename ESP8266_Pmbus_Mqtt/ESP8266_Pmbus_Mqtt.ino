#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define PECENABLE  1      //Smbus PEC(Packet Error Code) support
#define PSU_ADDRESS 0x58
#define PSU_PARTNER_ADDRESS 0x5E
#define MSG_BUFFER_SIZE  (50)

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
static uint8_t ps_i2c_address;
static uint8_t ps_patner_address;
const char* ssid = "FAIOT";       // Enter your WiFi name
const char* password = "20212021";    // Enter WiFi password
const char* mqtt_server = "192.168.12.1";  //Local Raspberry-Pi MQTT server(WiFi hotspot).
const uint16_t mqtt_port =  1883;
//const char *mqtt_broker = "broker.emqx.io";  // Free Public MQTT broker  
//const int mqtt_port = 1883;  //There is no privacy protection for public access broker.
                              //Any device can publish and subscribe to topics on it.
const char* clientID = "device1";
const char* mqtt_user = "dfiot";
const char* mqtt_password = "123abc";

const int ledPin = 16; 
const int SDA_PIN = 4;        //ESP-01S SDA = 0; SDC = 2;
const int SCL_PIN = 5;       // ESP-12F SDA = 4; SCl = 5;

unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
uint16_t value = 0;
long count = 0;
uint16_t k = 0;

static boolean Protocol = true;   // If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
static boolean scani2c = true;
static bool ledstatus = true;
static bool wifistatus = true;
static bool wiset = true;

WiFiClient eClient;
PubSubClient client(mqtt_server, mqtt_port, eClient);

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(38400);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  ps_i2c_address = PSU_ADDRESS;

  delay(10);
  setupWifi();
  if(wifistatus){
  client.setCallback(callback);
  client.connect(clientID, mqtt_user, mqtt_password);
  Serial.println("MQTT Broker Connected.  \n");
  digitalWrite(ledPin, HIGH);
  } 
  delay(100);
  i2cdetects(0x03, 0x7F);
  scani2c = false;
  while(scani2c){
    digitalWrite(ledPin, HIGH);
    pmbusdetects();
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(210);
    if(n > 0) break;
    }
 Serial.printf("\n PSU_ADDRESS: %#02x:\n", ps_i2c_address);
}

void loop() {
  if (wifistatus){
    if (!client.connected()) {
        reconnect();
       }
    client.subscribe("pmbus/set");
    client.loop();
 } 
  
 unsigned long now = millis(); 
  if (now - lastMsg >= 330) {
      lastMsg = now;
    if(readpmbusdata()){
      if(wifistatus) publishPmbusData(pd);
      if(count%5 == 0){
        printpmbusData(pd);
        if(ledstatus){
            digitalWrite(ledPin, LOW);
            ledstatus = false;
          } 
        else{
            digitalWrite(ledPin, HIGH);
            ledstatus = true;
          }
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
    if( k >= 20){
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
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(ledPin, LOW);   // Turn the LED on (Note that LOW is the voltage level
     Serial.println("LED_light...");                  // but actually the LED is on; this is because
                      // it is acive low on the ESP-01)
  }
  else {
    digitalWrite(ledPin, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("LED_OFF_light...");
  }
}

void reconnect() {
  // Loop until we're reconnected
  //client.connect(clientID, mqtt_user, mqtt_password);
  while (!client.connected()) {
     Serial.print("Attempting MQTT connection...");// Attempt to connect   
     String clientId = "ESP8266Client-";
     clientId += String(random(0xffff), HEX);
    if (client.connect((clientId.c_str()), mqtt_user, mqtt_password)) {
     // if (client.connect(clientId.c_str())) {
      Serial.println("connected to broker");
      client.publish("outTopic", "hello world");       // Once connected, publish an announcement...
      client.subscribe("inTopic");       // ... and resubscribe
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");  
      delay(5000);   // Wait 5 seconds before retrying
    }
  }
}
