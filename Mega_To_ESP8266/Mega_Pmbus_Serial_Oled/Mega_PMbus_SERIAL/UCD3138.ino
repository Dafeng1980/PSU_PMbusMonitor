
void ucd3138Read16B(uint8_t *data)
    {
    smbus->readBlock(0x0B, 0xF9, data, 16);  
    }

void ucd3138ReadNex16B(uint8_t *data)
    {
    smbus->readBlock(0x0B, 0xF8, data, 16);  
    }

void ucd3138Reads()
      {
        uint8_t data[16];
        uint8_t var[256];
        
        for ( int j = 0; j < 16; j++)
            {
              ucd3138ReadNex16B(data);
              for( int i = 0; i < 16; i++)
                 {
                    var[i+j*16] = data[i];
                  }
              }
          printFru(0,255,var);
      }

void ucd3138ConfReaAddr(unsigned int address)
      {
        uint8_t data[4];
//          data[0] = (address >> 24) & 0xff;
//          data[1] = (address >> 16) & 0xffff;
          data[0] = 0x00;
          data[1] = 0x04;
          data[2] = (address >> 8) & 0xffff;
          data[3] = address & 0xff;
          smbus->writeBlock(0x0B, 0xFD, data, 4); 
      }

void ucd3138MassEraseFlash()
     {
        smbus -> writeByte(0x0B, 0xF2, 0x02); //Erase flashP block1 32k(0x8000 - 0xFFFF)
        delay(30);
      }
      
void ucd3138PageEraseFlash()
        {
          uint8_t data[4] = {0x01,0x01,0x00,0x00};
          smbus->writeBlock(0x0B, 0xF1, data, 4);  //Erase flashP block0 page1 1k
          delay(30);
        }

void ucd3138PageEraseFlashCus(uint8_t val)
        {
          uint8_t data[4] = {0x01,0x00,0x00,0x00};
          data[1] = val;
          smbus->writeBlock(0x0B, 0xF1, data, 4);  //Erase flashP block0 page1 1k
          delay(50);
        }

void ucd3138Write16Byte(unsigned long address)
        {   
            uint8_t buf[20];
            buf[0] = (address >> 24) & 0xff;
       //   Serial.println(buf[0]); 
            buf[1] = (address >> 16) & 0xffff;
        //  Serial.println(buf[1]);
            buf[2] = (address >> 8) & 0xffffff;
        //  Serial.println(buf[2]);
            buf[3] = address & 0xff;
        //  Serial.println(buf[3]);
            for(int k = 0; k < 16; k++)
              {
                buf[4+k] = pgm_read_byte_near(boot_1k + k);
               }
             smbus->writeBlock(0x0B, 0xF4, buf, 20);
          }
        
void ucd3138WriteNext16(uint8_t *data)
        {
          smbus->writeBlock(0x0B, 0xF3, data, 16);
        }
        
void ucd3138Write1k()
      {
        uint8_t buff[16];
        ucd3138Write16Byte(0x00040400);
        delay(5);
        for ( int i = 1; i < 64; i++)
            { 
              for (int j = 0; j < 16; j++)
                   {
                     buff[j] = pgm_read_byte_near(boot_1k + (16*i+j));
                   }
               ucd3138WriteNext16(buff);
               delay(5);
               Serial.print(".");
              }
          Serial.println(".");
          Serial.println("Program Flash Success!");       
      }
        
void ucd3138Write4Byte(unsigned long address, unsigned long val)
        {
          uint8_t data[8];
          data[0] = (address >> 24) & 0xff;
          data[1] = (address >> 16) & 0xffff;
          data[2] = (address >> 8) & 0xffffff;
          data[3] = address & 0xff;
          data[4] = (val >> 24) & 0xff;
          data[5] = (val >> 16) & 0xffff;
          data[6] = (val >> 8) & 0xffffff;
          data[7] = val & 0xff;
          smbus->writeBlock(0x0B, 0xF5, data, 8);
         }
         
void ucd3138FlashDisplay (unsigned int address)
      {
        ucd3138ConfReaAddr(address);
        Serial.println(F(" "));
        Serial.print(F("Configure Read Address at : 0x"));
        Serial.println(address, HEX);
        Serial.println(F(" "));
        delay(10);
        ucd3138Reads();
        Serial.println(F(" ")); 
      }
     
