#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define TRACE 1
#define MSG_BUFFER_SIZE  (50)
#define MAXBUFFER (50)
static uint8_t receivedata[MAXBUFFER];

union floatData{
  float value;
  uint8_t n[4];
}fd;
const char* ssid = "FAIOT";       // Enter your WiFi name
const char* password = "20212021";    // Enter WiFi password
const char* mqtt_server = "192.168.0.13";
const char* clientID = "device1";
const char* mqtt_user = "dfiot";
const char* mqtt_password = "123abc";
const uint16_t mqtt_port =  1883;
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
uint16_t value = 0;
long n = 0;

WiFiClient eClient;
PubSubClient client(mqtt_server, mqtt_port, eClient);
//const char *mqtt_broker = "broker.emqx.io";
//const int mqtt_port = 1883;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(38400);
  Serial1.begin(38400);
  setupWifi();
  client.setCallback(callback);
  client.connect(clientID, mqtt_user, mqtt_password);
  Serial1.println("Debug message..2..: ");
}

void loop() {
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if(crps_receives()){
  unsigned long now = millis();
    if (now - lastMsg > 1500) {
      lastMsg = now;
      ++value;
      snprintf (msg, MSG_BUFFER_SIZE, "CRPS Refresh#%ld", value);
      Serial1.print("Publish message: ");
      Serial1.println(msg);
      client.publish("crps/Topic", msg);
    }
  float val = tofloat(0);          ////inputV;
  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/inputVolt", msg);

   val = tofloat(4);            //inputA
  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/inputCurr", msg);
  
   val = tofloat(8);   //inputP
  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/inputPower", msg);

  val = tofloat(12);   //outputV
  snprintf (msg, MSG_BUFFER_SIZE, "%5.4f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/outputVolt", msg);

  val = tofloat(16);   //outputVsb
  snprintf (msg, MSG_BUFFER_SIZE, "%5.4f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/outputVsb", msg);

    val = tofloat(20);   //outputA
  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/outputCurr", msg);

    val = tofloat(24);   //outputAsb
  snprintf (msg, MSG_BUFFER_SIZE, "%4.3f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/outputCsb", msg);

    val = tofloat(28);    //outputP
  snprintf (msg, MSG_BUFFER_SIZE, "%3.2f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/outputPower", msg);

    val = tofloat(32);     //fanSpeed
  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/fanSpeed", msg);

    val = tofloat(36);    //temp1
  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/temp1", msg);

    val = tofloat(40);   //temp2
  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/temp2", msg);

      val = tofloat(44);   //temp3
  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", val);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/temp3", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "0x%02x%02x", receivedata[49], receivedata[48]);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/status", msg);
  //client.subscribe("inTopic");
  }
   n++;
  if(n%20 == 0){
    Serial1.print("debug-port");
    Serial1.println(n);
  } 
  delay(100);
}

void setupWifi(){
  delay(10);
  Serial1.println();
  Serial1.print("Connecting to ");
  Serial1.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }
  randomSeed(micros());
  Serial1.println("");
  Serial1.println("WiFi connected");
  Serial1.println("IP address: ");
  Serial1.println(WiFi.localIP());
}

void trc(String msg) {
  if (TRACE) {
    Serial1.println(msg);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial1.print("Message arrived [");
  Serial1.print(topic);
  Serial1.print("] ");
  for (int i = 0; i < length; i++) {
    Serial1.print((char)payload[i]);
  }
  Serial1.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
     Serial1.println("LED_light...");                  // but actually the LED is on; this is because
                      // it is acive low on the ESP-01)
  }
  else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial1.println("LED_OFF_light...");
  }
  

}



void reconnect() {
  // Loop until we're reconnected
  //client.connect(clientID, mqtt_user, mqtt_password);
  while (!client.connected()) {
    //trc(F("MQTT connection...")); //F function enable to decrease sram usage
     Serial1.print("Attempting MQTT connection...");
    // Attempt to connect
     String clientId = "ESP8266Client-";
     clientId += String(random(0xffff), HEX);
    if (client.connect((clientId.c_str()), mqtt_user, mqtt_password)) {
     // if (client.connect(clientId.c_str())) {
      Serial1.println("connected");
      //trc(F("Connected to broker"));
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    }
    else {
      Serial1.print("failed, rc=");
      Serial1.print(client.state());
      Serial1.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


uint8_t crps_receives(){
  uint8_t val = 0;
  uint8_t msb,lsb;
  uint16_t incom;
  if (Serial.available()>0){
    
    while(Serial.available() < 2){     
    }   
    msb = Serial.read();
    lsb = Serial.read();
    incom = (msb << 8) | lsb;
    
  if (incom == 0xAACC)
  {
    Serial.write(0xBB);
    Serial.write(0xDD);
    while(Serial.available( )< 52){
    }
    Serial.read();
    Serial.read();
   for (int i = 0; i < 50; i++) {
        receivedata[i] = Serial.read();
        }
      val = 1;
  }
  
  }
  Serial.flush();
  return val;
}

float tofloat(uint16_t num){
  for (int i = 0; i<4; i++){
    fd.n[i] = receivedata[i+num];
  }
  return fd.value;
}
