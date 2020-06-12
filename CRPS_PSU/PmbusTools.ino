void print_title()
{
  Serial.print(F("\n*****************************************\n"));
  Serial.print(F("* PSU     PMbus control demo        *\n"));
  Serial.print(F("* This program demonstrates how to sent *\n"));
  Serial.print(F("* and receive data form PSU      *\n"));
  Serial.print(F("*****************************************\n"));
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.setCursor(0,10);
      u8g2.print(F("PMbus control Setting "));
      u8g2.setCursor(0,30);
      u8g2.print(F("Baud Rate is 115200"));
      u8g2.sendBuffer();
        delay(1000); 
}

void print_memu()
    {
       u8g2.clearBuffer();
       u8g2.setFontMode(1);
       u8g2.setFont(u8g2_font_6x10_tr);    
          u8g2.setCursor(0,10);
          u8g2.print(F("1-Read Volt&Curr"));
          u8g2.setCursor(0, 20);
          u8g2.print(F("2-Read AllSensor"));
          u8g2.setCursor(0, 30);
          u8g2.print(F("3-Read Status"));
          u8g2.setCursor(0, 60);
          u8g2.print(F("0-Main Menu:"));
          u8g2.sendBuffer();
              Serial.print(F("\n  1-Read Voltages and Currents\n"));
              Serial.print(F("  2-Read All Sensors\n"));
              Serial.print(F("  3-Read Status\n"));
              Serial.print(F("  0-Mai Menu\n"));
              Serial.print(F("\nEnter SONY-IR command:"));
                delay(500);
    }

void print_all_volt_curr()
{
  uint16_t w_val;
  float   current,voltage, temp;
        u8g2.clearBuffer();
        u8g2.setFontMode(1);
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.setCursor(0,10);
    voltage = pmbus->readVin(ps_i2c_address, false);
    current = pmbus->readIin(ps_i2c_address, false);
        u8g2.print(F("IN: "));
        u8g2.setCursor(24,10);
        u8g2.print(F("V="));
        u8g2.print(voltage, 1);
        u8g2.setCursor(78,10);
        u8g2.print(F("C="));
        u8g2.print(current, 2);
    voltage = pmbus->readVout(ps_i2c_address, false);
    current = pmbus->readIout(ps_i2c_address, false);
        u8g2.setCursor(0,20);
        u8g2.print(F("OT: "));
        u8g2.setCursor(24,20);
        u8g2.print(F("V="));
        u8g2.print(voltage, 3);
        u8g2.setCursor(78,20);
        u8g2.print(F("C="));
        u8g2.print(current, 2);
//          temp = pmbus->readOtemp(ps_i2c_address);           //temp sensor 0x8D  
//          voltage = pmbus->readItemp(ps_i2c_address);        //temp sensor 0x8E  
//          current = pmbus->readtemp3(ps_i2c_address);        //temp sensor 0x8F  
//          u8g2.setCursor(0,30);
//          u8g2.print(F("8D "));
//          u8g2.print(temp, 0);
//          u8g2.setCursor(42,30);
//          u8g2.print(F("8E "));
//          u8g2.print(voltage, 0);
//          u8g2.setCursor(84,30);
//          u8g2.print(F("8F "));
//          u8g2.print(current, 0);
//          temp = pmbus->readFanSpeed1(ps_i2c_address);
//           u8g2.setCursor(0,40);
//           u8g2.print(F("Fan1 Speed  "));
//           u8g2.print(temp, 1);
          w_val = pmbus->readStatusWord(ps_i2c_address);
          u8g2.setCursor(0,50);
          u8g2.print(F("STATUS WORD 0x"));
          u8g2.print(w_val, HEX);          
     voltage = pmbus->readPin(ps_i2c_address, false);
     current = pmbus->readPout(ps_i2c_address, false);
        u8g2.setCursor(0,60);
        u8g2.print(F("Pin="));
        u8g2.print(voltage, 1);
        u8g2.setCursor(66,60);
        u8g2.print(F("Pout="));
        u8g2.print(current, 1);
        u8g2.sendBuffer();
}

void print_all_status()
{
    uint16_t w_val;
    uint8_t io,in,tm,fa,vo;
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x10_tr);  
        w_val = pmbus->readStatusWord(ps_i2c_address);
          u8g2.setCursor(0,10);
          u8g2.print(F("STATUS WORD 0x"));
          u8g2.print(w_val, HEX);
          u8g2.setCursor(0,20);
          u8g2.print(F("LOW  "));      
          u8g2.print((w_val & 0xFF), BIN);
          u8g2.setCursor(0,30);
          u8g2.print(F("HIGH  "));
          u8g2.print((w_val >> 8), BIN);
//
      io = pmbus -> readStatusIout(ps_i2c_address);
          u8g2.setCursor(0,60);
          u8g2.print(F("STATUS_IOUT "));
          u8g2.print(io, HEX);
//      in = pmbus -> readStatusInput(ps_i2c_address);
//      tm = pmbus -> readStatusTemp(ps_i2c_address);
//      fa = pmbus -> readStatusFan(ps_i2c_address);
//          u8g2.setCursor(0,40);
//          u8g2.print(F("STATUS FAN 0x"));
//          u8g2.print(fa, HEX);
//      vo = pmbus -> readStatusVout(ps_i2c_address);
      u8g2.sendBuffer();
}

void print_all_sensors()
  {
      float temp,fspeed;
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x10_tr);    
      u8g2.setCursor(0,10);
        temp = pmbus->readOtemp(ps_i2c_address);           //temp sensor 0x8D  Inlet Air
          u8g2.print(F("Inlet Air  Temp "));
          u8g2.print(temp, 1);
          Serial.print(F("Inlet Air Temp "));
          Serial.println(temp, 1);
           temp = pmbus->readItemp(ps_i2c_address);            //0x8E Hotspot1 Sensor located on secondary side of DC/DC stage
            u8g2.setCursor(0,20);
            u8g2.print(F("DC/DC stag Temp "));
            u8g2.print(temp, 1);
            Serial.print(F("DC/DC stage Temp "));
            Serial.println(temp, 1);
              temp = pmbus->readtemp3(ps_i2c_address);          // 0x8F Hotspot2 Sensor located on primary heat sink
                 u8g2.setCursor(0,30);
                 u8g2.print(F("P heatsink Temp "));
                 u8g2.print(temp, 1);
                 Serial.print(F("P heatsink Temp "));
                 Serial.println(temp, 1);
                  temp = pmbus->readtemp1(ps_i2c_address);
                    u8g2.setCursor(0,40);
                    u8g2.print(F("MFR D3 Temp "));
                    u8g2.print(temp, 1);
                    Serial.print(F("MFR D3 Temp "));
                    Serial.println(temp, 1);
                      temp = pmbus->readtemp2(ps_i2c_address);
                        u8g2.setCursor(0,50);
                        u8g2.print(F("MFR D4 Temp "));
                        u8g2.print(temp, 1);
                        Serial.print(F("MFR D4 Temp "));
                        Serial.println(temp, 1);
                          fspeed = pmbus->readFanSpeed1(ps_i2c_address);
                            u8g2.setCursor(0,60);
                            u8g2.print(F("Fan1 Speed  "));
                            u8g2.print(fspeed, 1);
                            u8g2.sendBuffer();
                            Serial.print(F("Fan1 Speed  "));
                            Serial.println(fspeed, 1);
          }

void sound(){
         tone(kBuzzerPin, 2100);
          delay(100);
           noTone(kBuzzerPin);
    }

void alarm(){
      for (int i = 0; i < 50; i++){
              tone(kBuzzerPin, 2100);
              delay(100);
              noTone(kBuzzerPin);
              delay(200);            
        }
}

void displayPecOn(){
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_8x13_tr);    
      u8g2.setCursor(30, 30);
      u8g2.print(F(" PEC On"));  
      u8g2.sendBuffer();
      Serial.println(F("PEC On"));
}
void displayPecOff(){
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_8x13_tr);    
      u8g2.setCursor(0,10);
      u8g2.setCursor(30,30);
      u8g2.print(F(" PEC Off"));
      u8g2.setCursor(0, 40);
      u8g2.sendBuffer();
      Serial.println(F("PEC Off"));
}

int readir(){
  uint8_t dataValue[4];   // String for 3 digits + null char
  char ch;
  int index = 0;
  int accumulVal = 0;
   u8g2.clearBuffer();
   u8g2.setFontMode(1);
   u8g2.setFont(u8g2_font_8x13_tr);    
   u8g2.setCursor(0, 13);
   u8g2.print(F("Input: "));
   u8g2.sendBuffer();
  do {
    irrecv.resume();
    while(irrecv.decode(&results) == 0);
         sound();
    Serial.println(results.value, HEX);   
    delay(100);   
    // Serial.print("index: ");
    // Serial.println(index, DEC);
    switch (results.value) {         
         case 0x010:
            dataValue[index++] = 1;
            break;           
         case 0x810:
            dataValue[index++] = 2;       
            break;
         case 0x410:
            dataValue[index++] = 3;        
            break;
         case 0xC10:
            dataValue[index++] = 4;        
            break;
         case 0x210:
            dataValue[index++] = 5;         
            break;
         case 0xA10:
            dataValue[index++] = 6;        
            break;
         case 0x610:
            dataValue[index++] = 7;         
            break;
         case 0xE10:
            dataValue[index++] = 8;        
            break;
         case 0x110:
            dataValue[index++] = 9;         
            break;                        
         case 0x910:
            dataValue[index++] = 0;
            break;
         case 0xA90:
            ch = 'a';          
            break;
         case 0xA50:
            ch = 'c';      
            break;
         }
//    Serial.print("dataValue[index]: ");
//    Serial.println(dataValue[index], DEC);
    if ( index == 1){
        accumulVal = dataValue[0];
        u8g2.setCursor(56, 13);    
         u8g2.print(accumulVal);
         u8g2.sendBuffer();
      }
    if (index == 2){
    accumulVal = dataValue[0]*10 + dataValue[1];
         u8g2.setCursor(56, 13);    
         u8g2.print(accumulVal);
         u8g2.sendBuffer();
   }
    if (index == 3){
    accumulVal = dataValue[0]*100 + dataValue[1]*10 + dataValue[2];
       u8g2.setCursor(56, 13);    
       u8g2.print(accumulVal);
       u8g2.sendBuffer();
        ch = 'a';
   }
//    Serial.print("Accumulated value: ");
//    Serial.println(accumulVal, DEC);
         } while( ch != 'a'); 
         irrecv.resume();
         return  accumulVal;
}

float readeout(){
  uint8_t data[6];
 // unsigned int m,b,r;
  unsigned long maxValue,lastEnergyCount,lastSampleCount,currentEnergyCount,currentSampleCount;
  float eoutResult;
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_6x10_tr);
//  smbus -> readBlock(ps_i2c_address, 0x30, data, 5);
//        u8g2.clearBuffer();
//        u8g2.setCursor(0,10);
//        u8g2.print(F("Data "));
//        u8g2.print(data[0],HEX);
//        u8g2.print(data[1],HEX);
//        u8g2.print(data[2],HEX);
//        u8g2.print(data[3],HEX);
//        u8g2.print(data[4],HEX);
//        u8g2.sendBuffer();
//        delay(2000);
//      m = data[1] * 256 + data[0];
//      b = data[3] * 256 + data[2];
//      r = data[4];
//      maxValue = (m * 32767 + b)* pow(10,r);
      maxValue = 32767;
//      u8g2.clearBuffer();
//      u8g2.setCursor(00,10);
//      u8g2.print(F("MaxValue= "));
//      u8g2.print(maxValue, DEC);
//      u8g2.sendBuffer();
    smbus -> readBlock(ps_i2c_address, 0x87, data, 6);             //EOUT 0x87
    lastEnergyCount = data[0] + data[1]*0x100 + data[2]*maxValue;
    lastSampleCount = data[3] + data[4]*0x100 + data[5]*0x10000;
      delay(1000);
      smbus -> readBlock(ps_i2c_address, 0x87, data, 6);
      currentEnergyCount = data[0] + data[1]*0x100 + data[2]*maxValue;
      currentSampleCount = data[3] + data[4]*0x100 + data[5]*0x10000;
        u8g2.clearBuffer();
        u8g2.setCursor(0,20);
        u8g2.print(F("SampleCount"));
        u8g2.setCursor(0,30);
        u8g2.print(currentSampleCount - lastSampleCount);
        u8g2.sendBuffer();
    eoutResult = (float)(currentEnergyCount - lastEnergyCount)/(float)(currentSampleCount - lastSampleCount);
    return eoutResult; 
}
