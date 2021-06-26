void ReadMfrRev(){
      smbus_writeWord(ps_i2c_address, 0xD7, 0xFFFF);
      smbus_writeWord(ps_i2c_address, 0xD7, 0xFFFF);
      delay(20);
      smbus_writeByte(ps_i2c_address, 0xD2, 0x06);
      smbus_writeWord(ps_i2c_address, 0xD3, 0x0016);
      smbus_readBlock(ps_i2c_address, 0xB4, ver, 6);
      smbus_writeWord(ps_i2c_address, 0xD7, 0x0055);
      printchar(ver, 6);
}

void SetVout2_46V(){
      smbus_writeWord(ps_i2c_address, 0x21, 0x5C00);
      Serial.printf("\n Set Vout to 46V \n");
      delay(1000);
}

void SetVout2_50V(){
      smbus_writeWord(ps_i2c_address, 0x21, 0x6400);
      Serial.printf("\n Set Vout to 50V \n");
      delay(1000);
}

void SetVout2_54V(){
      smbus_writeWord(ps_i2c_address, 0x21, 0x6C00);
      Serial.printf("\n Set Vout to 54V \n");
      delay(1000);
}
