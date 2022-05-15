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

         else if(smbus_data[1] == 5) i2cdetectsstatus();            //[AA 05] Scan Pmbus device.
         else if(smbus_data[1] == 6) standbystatus();               //[AA 06] Standby monitoring Enable/Disble.
         else if(smbus_data[1] == 9) pecstatus();                   //[AA 09] PEC Enable/Disable.
       }
       smbusflag = false;
     }
  if(key == 1) monitorstatus();           
    else if(key == 2){
               pmbusexpand = true;
               key = 0;
            }
    else if(key == 3){
               pmbusexpand1 = true;
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
    ledflash();
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
      if(mqttflag){
        Serial.println("MQTT Broker Connected. \n ");
        client.subscribe("rrh/pmbus/set");
        delay(100);
      }       
    } 
  else{
    Serial.println("");
    Serial.println("WiFi connect Failed / Wifi Staus FALSE");
    }
  k = 0; 
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
            client.subscribe("rrh/pmbus/set");            
    }
  if (mqttflag){    
      client.loop();      
//      delay(200);
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

void pmbus_devices_detect(){
  i2cdetects(0x03, 0x7F);
  int i = 8;  
  while(scani2c){
    ledflash();
    pmbusdetects();
    delay(80);
    i--;
    if(n > 0) scani2c = false;
    if(i == 0) scani2c = false;
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
  int q = 0;
  char addr[35];
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
        addr[3*q] = hex_table[address >> 4];
        addr[3*q + 1] = hex_table[address & 0x0f];
        addr[3*q + 2] = ' ';
        q++;
      } else if (error == 4) {    // other error      
        Serial.print(" XX");
      } else {                   // error = 2: received NACK on transmit of address              
        Serial.print(" --");    // error = 3: received NACK on transmit of data
      }
    } else {                 // address not scanned      
      Serial.print("   ");
    }
    if(q > 10) break;
  }
  addr[3*q] = '\0';
  Serial.println("\n");
  snprintf (msg, MSG_BUFFER_SIZE, "Scan addr at:0x%s", addr);
  if(mqttflag) client.publish("rrh/pmbus/set/info", msg);
//  delay(100); 
}

void pecstatus(){
    pecflag = !pecflag;
    if(pecflag) {
      Serial.print(F("PEC Enable\n"));
      if(mqttflag) client.publish("rrh/pmbus/status/pec", "1");
    }
    else {
      Serial.print(F("PEC Disable\n"));
      if(mqttflag) client.publish("rrh/pmbus/status/pec", "0");
    }
    delay(500);
}

void i2cdetectsstatus(){
  scani2c = !scani2c;
  if(scani2c) {
    Serial.print(F("I2C Detect Device Enable\n"));
    if(mqttflag) client.publish("rrh/pmbus/status/scan", "1");
    pmbusflag = false;
  }
  else {
    Serial.print(F("I2C Detect Device Disable\n"));
    if(mqttflag) client.publish("rrh/pmbus/status/scan", "0");
    pmbusflag = true;
  }
  delay(500);
}

void setIntervaltime() {
      Serial.printf("Current Interval Time(ms):%d\n", pmInterval);
      Serial.println(F("Input New Interval Time(ms) < 60S :"));
      pmInterval = read_int();     
      Serial.printf("New Interval Time(ms):%d\n", pmInterval);
      delay(1000);
}

void reset_address(){
      Serial.printf("Current PSU Address:%#02X; Patner PSU Address:%#02X.\n", ps_i2c_address, ps_patner_address);
      Serial.println(F("Input PSU address: (Can recognize Hex, Decimal, Octal, or Binary)"));
      Serial.println(F("Example: Hex: 0x11 (0x prefix) Octal: O21 (letter O prefix) Binary: B10001" ));
      ps_i2c_address = read_int();     
      Serial.printf("New PSU address: %#02X\n", ps_i2c_address);
      Serial.println(F("Input Patner PSU address:"));
      ps_patner_address = read_int();
      Serial.printf("New Patner PSU address: %#02X\n", ps_patner_address);
      delay(1000);
}

void setModel() {
      Serial.printf("Current PSU Model is %#02X \n", unitname);
      Serial.println(F("PSU Model Numbers: ()"));
      Serial.println(F("0 - Standard " ));
      Serial.println(F("1 - ADP1051 " ));
      Serial.println(F("2 - Redundancy " ));
      Serial.println(F("3 - Others " ));
      Serial.println(F("Input New PSU Model:"));
      unitname = read_int();     
      Serial.printf("New PSU Model is: %02x \n", unitname);
      delay(500);
}

void ledflash(){
  ledstatus = !ledstatus;
  if(ledstatus) digitalWrite(kLedPin, HIGH);
  else digitalWrite(kLedPin, LOW);
}

void monitorstatus(){
  statusflag = !statusflag;
  if(statusflag) Serial.print(F("Status Monitor Enable\n"));
  else Serial.print(F("Status Monitor Disable\n"));
  key = 0;
  delay(500);
  }

void standbystatus(){
  standbyflag = !standbyflag;
  if(standbyflag) Serial.print(F("Standby Enable\n"));
  else Serial.print(F("Standby Disable\n"));
  delay(500);
  }

void print_memu() {                   
           Serial.print(F("PSU PMbus Debug Tools (Smbus Protocol)\n"));
           Serial.print(F("Monitoring PSU Status etc. and Detect i2c Devices\n"));
           Serial.print(F("Send the Smbus Protocol Command.\n"));
           Serial.println(F("\nEnter command:"));
           printhelp();
           Serial.println(F(" "));
        }  
