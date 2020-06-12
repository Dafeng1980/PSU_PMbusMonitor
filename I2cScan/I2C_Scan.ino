/*
I2C Address Scan


*/

#include<Wire.h>
#include <U8g2lib.h>

 U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 20, 21, /* reset=*/ U8X8_PIN_NONE);

 const uint8_t kButtonPin = 24;
 const uint8_t kBuzzerPin = 15;


void setup() {
  pinMode(kButtonPin, INPUT_PULLUP);
    Wire.begin();
    u8g2.begin();
    Serial.begin(115200); 
     print_title();
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(0,10);
     u8g2.print(F("Start the I2C address"));
     u8g2.setCursor(0,20);
     u8g2.print(F("scan.  terminator."));
     u8g2.setCursor(0,30);
     u8g2.print(F("Baud Rate is 115200"));
     u8g2.sendBuffer();
     Serial.println("\nSend any character to start the I2C address scan. (Press enter in Arduino Serial Monitor.)");
      delay(1000);  
}

void loop() {
  uint8_t n;
  uint8_t ack;
  uint8_t address;
  uint8_t ack_count;
    ack_count = 0;
    n = 0;
     u8g2.clearBuffer();
     u8g2.setFontMode(1);
     u8g2.setFont(u8g2_font_6x12_tr);  
     u8g2.setCursor(0,36);
     u8g2.print(F("Pre Button2Scan Dev"));
     u8g2.sendBuffer();     
      while(digitalRead(kButtonPin) != 0);
      sound();
      
       u8g2.clearBuffer();
       u8g2.setFontMode(1);
       u8g2.setFont(u8g2_font_6x10_tr);
       u8g2.setCursor(0,10);
       u8g2.print(F("Ack rec from addr:"));     
       u8g2.setCursor(0,24);
         for (address = 0; address < 127; address++)
          {
            Wire.beginTransmission(address);
            ack = Wire.endTransmission();
           if (ack == 0)
           // if ((address+1)%8 == 0)
            {
                ack_count++;
                  if(address < 16){
                    u8g2.print(F("0x0"));
                    Serial.print("Acknowledge received from address : 0x0");
                        }          
                  else{
                    u8g2.print(F("0x"));
                    Serial.print("Acknowledge received from address : 0x");       
                      }                  
                    u8g2.print(address, HEX);
                    u8g2.print(F(" "));
                    Serial.println(address, HEX);
                    if (ack_count%4 == 0){
                      n++;
                      u8g2.setCursor(0,26+n*10);
                    }
               }
                  delay(10);
            }     
              u8g2.sendBuffer();
              delay(100);
      
            if (ack_count == 0){
                  Serial.println("No addresses acknowledged.");
                  u8g2.clearBuffer();
                  u8g2.setFontMode(1);
                  u8g2.setFont(u8g2_font_6x12_tr);    
                  u8g2.setCursor(30,30);
                  u8g2.println("No Addr ACK.");
                  u8g2.sendBuffer();
                  sound();
                  delay(100);
                  sound();
                  delay(100);
            }
             sound();
             while(digitalRead(kButtonPin) != 0);
  }

void print_title()
{
  Serial.println("");
  Serial.println("*****************************************************************");
  Serial.println("* I2C Address Scan                                              *");
  Serial.println("*                                                               *");
  Serial.println("* This program will scan all 128 I2C addresses and report those *");
  Serial.println("* that acknowledge a address+!w command.                        *");
  Serial.println("*                                                               *");
  Serial.println("* Set the baud rate to 38400 select the newline terminator.    *");
  Serial.println("*                                                               *");
  Serial.println("*****************************************************************");
}

void sound(){
         tone(kBuzzerPin, 2100);
          delay(100);
           noTone(kBuzzerPin);
    }
