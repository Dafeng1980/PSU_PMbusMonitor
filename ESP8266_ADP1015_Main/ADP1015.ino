void ReadMfrRev(){
      smbus_writeWord(ps_i2c_address, 0xD7, 0xFFFF);
      smbus_writeWord(ps_i2c_address, 0xD7, 0xFFFF);
      delay(20);
      smbus_writeByte(ps_i2c_address, 0xD2, 0x06);
      smbus_writeWord(ps_i2c_address, 0xD3, 0x0016);
      smbus_readBlock(ps_i2c_address, 0xB4, ver, 6);
      smbus_writeWord(ps_i2c_address, 0xD7, 0x0055);
      Serial.printf("\nMRF_Rev:  %01x%01x%01x%01x%01x%01x \n",ver[0],ver[1],ver[2],ver[3],ver[4],ver[5]);
}

void SetVout2_46V(){
      smbus_writeWord(ps_i2c_address, 0x21, 0xB800);
      Serial.printf("\n Set Vout to 46V \n");
      delay(1000);
}

void SetVout2_50V(){
      smbus_writeWord(ps_i2c_address, 0x21, 0xC800);
      Serial.printf("\n Set Vout to 50V \n");
      delay(1000);
}

void SetVout2_54V(){
      smbus_writeWord(ps_i2c_address, 0x21, 0xD800);
      Serial.printf("\n Set Vout to 54V \n");
      delay(1000);
}
