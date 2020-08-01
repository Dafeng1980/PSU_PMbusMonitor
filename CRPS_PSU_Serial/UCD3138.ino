
void ucd3138Read16B(uint8_t *data)
    {
    smbus->readBlock(0x0B, 0xF9, data, 16);  
    }

void ucd3138ReadNex16B(uint8_t *data)
    {
    smbus->readBlock(0x0B, 0xF8, data, 16);  
    }

void ucd3138Reads(){
  uint8_t data[16];
  uint8_t data1[16];
  uint8_t data2[16];
  uint8_t data3[16];
  uint8_t data4[16];
  uint8_t data5[16];
  uint8_t var[96];
  ucd3138ReadNex16B(data);
  ucd3138ReadNex16B(data1);
  ucd3138ReadNex16B(data2);
  ucd3138ReadNex16B(data3);
  ucd3138ReadNex16B(data4);
  ucd3138ReadNex16B(data5);
//  ucd3138Read16B(data1);
//  ucd3138Read16B(data2);
  for ( int i = 0; i<16; i++)
      {
        var[i] = data[i];
      }
    for ( int i = 0; i<16; i++)
      {
        var[i+16] = data1[i];
      }
          for ( int i = 0; i<16; i++)
      {
        var[i+32] = data2[i];
      }
          for ( int i = 0; i<16; i++)
      {
        var[i+48] = data3[i];
      }
          for ( int i = 0; i<16; i++)
      {
        var[i+64] = data4[i];
      }
          for ( int i = 0; i<16; i++)
      {
        var[i+80] = data5[i];
      }
      printFru(0,95,var);
}

void ucd3138ConfReaAddr()
    {
  uint8_t data[4];
  data[0] = 0x00;
  data[1] = 0x04;
  data[2] = 0x10;
  data[3] = 0xA0;
  smbus->writeBlock(0x0B, 0xFD, data, 4); 
    }

void ucd3138MassEraseFlash()
     {
        smbus -> writeByte(0x0B, 0xF2, 0x01);
        delay(30);
      }
