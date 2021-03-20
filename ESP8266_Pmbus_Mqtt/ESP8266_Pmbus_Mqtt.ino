#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define SDA_PIN 4
#define SCL_PIN 5
#define PECENABLE  1      //PEC ENABLE
#define PS_ADDRESS 0x58
#define PS_PARTNER_ADDRESS 0x5E
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
const char* mqtt_server = "192.168.12.1";
const char* clientID = "device1";
const char* mqtt_user = "dfiot";
const char* mqtt_password = "123abc";
const uint16_t mqtt_port =  1883;
const int ledPin = 16; 
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
uint16_t value = 0;
long count = 0;

boolean Protocol = true;   // If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
boolean scani2c = true;
bool ledstatus = true;

WiFiClient eClient;
PubSubClient client(mqtt_server, mqtt_port, eClient);
//const char *mqtt_broker = "broker.emqx.io";
//const int mqtt_port = 1883;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(38400);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  ps_i2c_address = PS_ADDRESS;

  setupWifi();
  client.setCallback(callback);
  client.connect(clientID, mqtt_user, mqtt_password);
  Serial.println("Debug message..2..: ");
  digitalWrite(ledPin, HIGH);
  
  for (int i = 0; i <3; i++){
    i2cdetects(0x03, 0x7F);
    delay(100);
  }
  while(scani2c){
    digitalWrite(ledPin, HIGH);
    pmbusdetects();
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(210);
    if(n > 0) break;
    }
 Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    if(readpmbusdata()){
      publishPmbusData(pd);
    }
        count++;
    if(count%15 == 0){
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
     delay(200);
}

void setupWifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
     Serial.println("LED_light...");                  // but actually the LED is on; this is because
                      // it is acive low on the ESP-01)
  }
  else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("LED_OFF_light...");
  }
}

void reconnect() {
  // Loop until we're reconnected
  //client.connect(clientID, mqtt_user, mqtt_password);
  while (!client.connected()) {
    //trc(F("MQTT connection...")); //F function enable to decrease sram usage
     Serial.print("Attempting MQTT connection...");
    // Attempt to connect
     String clientId = "ESP8266Client-";
     clientId += String(random(0xffff), HEX);
    if (client.connect((clientId.c_str()), mqtt_user, mqtt_password)) {
     // if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      //trc(F("Connected to broker"));
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool readpmbusdata()
{   
    bool ret = 0;
    if(smbus_waitForAck(ps_i2c_address, 0x00) == 1)
       ret = 1;
     pd.inputV = pmbus_readVin(ps_i2c_address);
     pd.inputA = pmbus_readIin(ps_i2c_address);
     pd.outputV = pmbus_readVout(ps_i2c_address);
     pd.outputA = pmbus_readIout(ps_i2c_address);
     pd.inputP = pmbus_readPin(ps_i2c_address);
     pd.outputP = pmbus_readPout(ps_i2c_address);
     pd.temp1 = pmbus_readOtemp(ps_i2c_address);           //temp sensor 0x8D  
     pd.temp2 = pmbus_readItemp(ps_i2c_address);        //temp sensor 0x8E  
     pd.temp3 = pmbus_readMtemp(ps_i2c_address);        //temp sensor 0x8F  
     pd.fanSpeed = pmbus_readFanSpeed1(ps_i2c_address);
     pd.statusWord = pmbus_readStatusWord(ps_i2c_address); 
     pmbus_setPage(ps_i2c_address,1);                    //set Page to 1, read 12Vsb 
     pd.outputVsb = pmbus_readVout(ps_i2c_address);
     pd.outputAsb = pmbus_readIout(ps_i2c_address);
     pmbus_setPage(ps_i2c_address,0);
     pd.i2cAddr = ps_i2c_address;
     return ret;
   //  delay(20);     
}
