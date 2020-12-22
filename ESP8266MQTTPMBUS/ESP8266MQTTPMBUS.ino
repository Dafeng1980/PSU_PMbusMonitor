#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define TRACE 1
#define MSG_BUFFER_SIZE  (50)
const char* ssid = "CMCC-kS9s";       // Enter your WiFi name
const char* password = "DA431431";    // Enter WiFi password
const char* mqtt_server = "192.168.1.19";
const char* clientID = "device1";
const char* mqtt_user = "dfiot";
const char* mqtt_password = "123abc";
const uint16_t mqtt_port =  1883;
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;

struct Mystruct
{
  float volt = 34.53;
  float curr = 11.56;
  float outp = 123.5;
  float ina = 5.6;
  float temp = 23;
  float fspeed = 1230;
  uint16_t stus = 0x38A2;
}pd;

WiFiClient eClient;
PubSubClient client(mqtt_server, mqtt_port, eClient);
//const char *mqtt_broker = "broker.emqx.io";
//const int mqtt_port = 1883;

int n;



void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial1.begin(38400);
  setupWifi();
  client.setCallback(callback);
  client.connect(clientID, mqtt_user, mqtt_password);
}

void loop() {

   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial1.print("Publish message: ");
    Serial1.println(msg);
    client.publish("outTopic", msg);
  }
  snprintf (msg, MSG_BUFFER_SIZE, "Current: %4.3fA", pd.curr);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/inputCurr", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%3.2fW", pd.outp);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/outputPower", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "Voltage: %5.4fV", pd.volt);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/inputVoltage", msg);

  snprintf (msg, MSG_BUFFER_SIZE, "%2.1f", pd.temp);
  Serial1.print("Publish message: ");
  Serial1.println(msg);
  client.publish("crps/temp1", msg);

  client.subscribe("inTopic");
  
  Serial1.print("debug-port");
  n++;
  Serial1.println(n);
  delay(1000);
  // put your main code here, to run repeatedly:

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
