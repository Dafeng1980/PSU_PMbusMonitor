void checkButton(){
    if(digitalRead(kButtonPin) == 0 && buttonflag){
          
          delay(10);
        if(digitalRead(kButtonPin) == 0){
              delay(1);
              key++;
              if (key >= 5) key = 0;
              if(scani2c) {
                scani2c = false;
                Serial.print(F("I2C Detect Device Disable\n"));
                pmbusflag = true;
                key = 0;
                delay(500);
              }
              Serial.printf("\n Key= %#01d:\n", key);
              buttonflag = false;
              delay(100);           
        }
    }

    if(smbusflag){
      if (smbus_data[0] >= 0 && smbus_data[0] <=9) smbus_command_sent(smbus_data[0]);
      else if(smbus_data[0] == 0xAA){                                  
        if(smbus_data[1] == 0) ps_i2c_address = smbus_data[2];           //[AA 00 XX] set Pmbus device address
        else if(smbus_data[1] == 1) pmInterval = (smbus_data[2]<<8)  + smbus_data[3];  //[AA 01 XX XX] Set pmbus poll time;
        else if(smbus_data[1] == 2) unitname = smbus_data[2];            //[AA 02 XX] set unit 
      }
      smbusflag = false;
    }
    if(key == 1) monitorstatus();
    else if(key == 2) standbystatus();

            
    else if(key == 3){
               Serial.println(F("TBD "));
               delay(100);
               key = 0;
            }
    else if(key == 4){
               Serial.println(F("TBD "));
               delay(100);
               key = 0;                
              }       
}

void setWifiMqtt(){
  delay(10);
  wifistatus = true;
  mqttflag = false;
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
    wifistatus = false;
    break;
    }
  }
 if(wifistatus){
      randomSeed(micros());
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      client.setCallback(callback);
      if(client.connect(clientID, mqtt_user, mqtt_password)) mqttflag = true;
      Serial.println("MQTT Broker Connected. \n "); 
    } 
  else{
    Serial.println("");
    Serial.println("WiFi connect Failed / Wifi Staus FALSE");
    }
  k = 0; 
}

void reconnect() {
  // Loop until we're reconnected
  //client.connect(clientID, mqtt_user, mqtt_password);
  while (!client.connected()) {
     Serial.print("Attempting MQTT connection..."); // Attempt to connect   
     String clientId = "ESP8266Client-";
     clientId += String(random(0xffff), HEX);
    if (client.connect((clientId.c_str()), mqtt_user, mqtt_password)) {
      Serial.println("connected to broker");
      client.publish("outTopic", "hello world reconnected");  // Once connected, publish an announcement...
      mqttflag = true;   
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      k++;
      delay(2000);   // Wait 2 seconds before retrying
        if( k >= 5){
            mqttflag = false;
            if( k >= 15){
                wifistatus = false;
                Serial.println(F("WiFi connect Failed!!"));
                delay(1000);
                k = 0;
                break;
              }
            break;
         }                   
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
    if ((char)payload[0] == '[')  {
    smbus_data[0] = tohex(payload[1])*16 + tohex(payload[2]);
//  Serial.printf("Data[0] is: %02x \n", data[0]);
//  delay(100);
  for(int i = 0; i < 32; i++){
    smbusflag = true;
    if (payload[3*i+3] == ']') break;
    smbus_data[i+1] = tohex(payload[3*i+4])*16 + tohex(payload[3*i+5]);
    count = i + 2;
    if (i == 31) {
      Serial.println(F("Smbus Invalid format" ));
      delay(100);
      smbusflag = false;
      break; 
      }      
    }
  }
    else if ((char)payload[0] == '0') key = 0;
    else if ((char)payload[0] == '1') key = 1;
    else if ((char)payload[0] == '2') key = 2;
    else if ((char)payload[0] == '3') key = 3;
    else if ((char)payload[0] == '4') key = 4;
    else key = 0;
    Serial.printf("\n Key= %#01d:\n", key); 
}

void mqttLoop(){
  if (!client.connected()) {
            reconnect();            
    }
    if (mqttflag){    
      client.loop();
      client.subscribe("rrh/pmbus/set");
    }
}

void pmbus_devices_detect(){
  i2cdetects(0x03, 0x7F);  
  while(scani2c){
    digitalWrite(kLedPin, HIGH);
    pmbusdetects();
    delay(50);
    digitalWrite(kLedPin, LOW);
    delay(210);
    if(n > 0) scani2c = false;
    if (digitalRead(kButtonPin) == 0){
          delay(10);
          if(digitalRead(kButtonPin) == 0){
              buttonflag = false;                   
              scani2c = false;
              Serial.print(F("PMbus Detect Device Disable\n"));
              delay(100);
          }
       }
    }
    Serial.printf("\nPMBUSADDRESS %#02x:\n", ps_i2c_address);
    n = 0;
    delay(100);
 }


void pmbusdetects() {
  uint8_t i, address, error;
  Serial.print("   ");                   // table header
  for (i = 8; i < 16; i++) {
    Serial.printf("%3x", i);
  }
  Serial.printf("\n%#02x:", 0x50);
  for (address = 88; address <= 95; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
    //  if (address == 92) error = 0;
      if (error == 0) {                  // device found        
        n++;
        if(n == 1) ps_i2c_address = address;
        else ps_i2c_address = PS_I2C_ADDRESS;
        if(n == 2) ps_patner_address = address;
        else ps_patner_address = PS_PARTNER_ADDRESS;
        Serial.printf(" %02x", address);       
      } 
      else if (error == 4) {              // other error        
        Serial.print(" XX");
      } 
      else {                             // error = 2: received NACK on transmit of address        
        Serial.print(" --");             // error = 3: received NACK on transmit of data        
      }
      delay(20);
  }
  Serial.println("\n");
  Serial.println(n);
}

void i2cdetects(uint8_t first, uint8_t last) {
  uint8_t i, address, error;
  //char buff[10];
  Serial.print("   ");            // table header
  for (i = 0; i < 16; i++) {
    Serial.printf("%3x", i);
//    sprintf(buff, "%3x", i);
//    Serial.print(buff);
  }
  for (address = 0; address <= 127; address++) {             // addresses 0x00 through 0x77
    if (address % 16 == 0) {                            // table body
      Serial.printf("\n0x%02x:", address & 0xF0);
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      delay(10);
      if (error == 0) {                           // device found
        Serial.printf(" %02x", address);
      } else if (error == 4) {    // other error      
        Serial.print(" XX");
      } else {                   // error = 2: received NACK on transmit of address              
        Serial.print(" --");    // error = 3: received NACK on transmit of data
      }
    } else {                 // address not scanned      
      Serial.print("   ");
    }
  }
  Serial.println("\n");
}

void print_memu()
        {                   
           Serial.print(F("PSU PMbus Debug Tools (Smbus Protocol)\n"));
           Serial.print(F("Monitoring PSU Status etc. and Detect i2c Devices\n"));
           Serial.print(F("Send the Smbus Protocol Command.\n"));
           Serial.println(F("\nEnter command:"));
           printhelp();
           Serial.println(F(" "));
        }  
