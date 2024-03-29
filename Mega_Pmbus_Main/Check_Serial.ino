
void checkSerial(){
  char readval;
  if (Serial.available())                //! Serial input read
  {
      readval = read_char();                     //! Reads the user command
    if ((char)readval == '0')      key = 0;
    else if ((char)readval == '1') key = 1;
    else if ((char)readval == '2') key = 2;
    else if ((char)readval == '3') key = 3;
    else if ((char)readval == '4') key = 4;
    else if ((char)readval == '5') i2cdetectsstatus();    
    else if ((char)readval == '6') key = 6;
    else if ((char)readval == '7') key = 7;
    else if ((char)readval == '8') mfr_menu_commands();
    else if ((char)readval == '9') pecstatus();
    else if ((char)readval == 'a'  || (char)readval == 'A') setModel(); 
    else if ((char)readval == 'c'  || (char)readval == 'C') smbus_commands();
    else if ((char)readval == 'e'  || (char)readval == 'E') setIntervaltime();   
    else if ((char)readval == 'r'  || (char)readval == 'R') reset_address();
    else if ((char)readval == 'h'  || (char)readval == 'H') {
      printhelp();
      delay(1500);
      while (!digitalRead(kButtonPin)) {};  // wait for button unpress
    }
    else {
      Serial.println(F("unknown command"));
      Serial.println(F("type 'h' for help"));
      key = 0;       
    }
    Serial.printf("\nKey= %#01d:\n", key);
 }
}

void i2cdetectsstatus(){
  scani2c = !scani2c;
  if(scani2c) {
    Serial.print(F("I2C Detect Device Enable\n"));
    pmbusflag = false;
  }
  else {
    Serial.print(F("I2C Detect Device Disable\n"));
    pmbusflag = true;
  }
  delay(500);
}

void mfr_menu_commands(){
        uint8_t user_command;       
  do{  
    Serial.print(F("  1-Read MFR Info \n"));
    Serial.print(F("  2-Clear The Faults \n"));
    Serial.print(F("  3-TBD \n"));
    Serial.print(F("  4-TBD \n"));
    Serial.print(F("  5-Margin High \n"));
    Serial.print(F("  6-Margin Low \n"));
    Serial.print(F("  7-Mass Erase the PFlash \n"));
    Serial.print(F("  8-Reads UCD3138 Pflash At 2k_32K_64K Checksum \n"));
    Serial.print(F("  9-Erase&Write UCD3138 Pflash At 2k Checksum \n"));
    Serial.print(F("  m-Main Menu \n"));
    Serial.print(F("\nEnter a command: "));  
    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')   Serial.print(F("m\n"));     // Print m if it is entered
    else  Serial.println(user_command);                    // Print user command     
    switch (user_command)
    {
      case 1:
//      getId();
//      getModel();
//      getRevision();
      Serial.println(F(" "));
        break;
      case 2:
      Serial.println(F("TBD "));
        break;
      case 3:
      Serial.println(F("TBD "));
        break;
      case 4:
      Serial.println(F("TBD "));     
        break;
      case 5:
      Serial.println(F("TBD "));
      break;      
      case 6:
      Serial.println(F("No Test "));
        break;        
      case 7:
        Serial.println(F(" "));
        Serial.println(F("Mass Erase all the PFlash 0x040000 to 0x04FFFF For 64K"));
        Serial.print(F("Press button to continue"));
        while(digitalRead(kButtonPin) != 0);
        Serial.println(F(" "));
         ucd3138MassEraseFlash(0x01);     //Pflash Block0 32k
         delay(30);
         ucd3138MassEraseFlash(0x02);     //Pflash Block1 32k
         delay(30); 
         buzzing();     
        break;        
      case 8:
         Serial.println(F(" "));
         Serial.println(F("Display the PFlash checksum_2k_32k_64k"));
         Serial.println(F(" "));
          ucd3138FlashDisplay(0x0700);
          Serial.println(F(" "));
          buzzing();
          ucd3138FlashDisplay(0x7F00);
          Serial.println(F(" "));
          buzzing();
          ucd3138FlashDisplay(0xFF00);
          Serial.println(F(" "));
          buzzing();
        break;        
      case 9:
         Serial.println(F(" "));
         Serial.println(F("Erase&Write the PFlash Block0 Page 1(0x040400) For 2K checksum "));
         Serial.print(F("Press button to continue"));
          while(digitalRead(kButtonPin) != 0);
          Serial.println(F(" "));
          ucd3138Blocl0PageErase(0x01);  //Erase Pflash Block0 Page 1;
          delay(25);
          ucd3138Write2kChecksum();
          buzzing();
        break;        
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}

void pecstatus(){
    pecflag = !pecflag;
    if(pecflag) Serial.print(F("PEC Enable\n"));
    else Serial.print(F("PEC Disable\n"));
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
      delay(1000);
}

void smbus_commands(){
      uint8_t user_command;
      uint16_t offset;
      int count;
      struct smbusCommand
        {      
            uint8_t commands;
            uint8_t databyte;
            uint16_t dataword;
            uint8_t datablock[32];
            uint16_t blocksize;
            uint8_t datablock_b[32];
            uint16_t blocksize_b;              
        }sm;
  Serial.println(F("Input Smbus Commands: (Hex Data Format)"));
  Serial.println(F("Example Syntax: [03 58 00 01] MSB:03(Smbus Function:) Wirte Byte"));
  Serial.println(F("Smbus Function (0 to 7) Info:"));
  Serial.print(F("  0-Read Byte (RB)\n"));
  Serial.print(F("  1-Read Word (RW)\n"));
  Serial.print(F("  2-Read Block (RBL)\n"));
  Serial.print(F("  3-Write Byte (WB)\n"));
  Serial.print(F("  4-Write Word (WW)\n"));
  Serial.print(F("  5-Write Block (WBL)\n"));
  Serial.print(F("  6-Write Read Block (WRB)\n"));
  Serial.print(F("  7-Sent Byte (SBY)\n"));
  Serial.print(F("  8-EEPROM Read Byte (ER)\n"));
  Serial.print(F("  9-EEPROM Read Bytes (ERS)\n"));
  Serial.println(F("[03 58 00 01]: 58 I2C_Address; 00 Smbus Command; 01 Data Byte;"));
  Serial.println(F("Send (03)writeByte: smbus_writeByte(0x58, 0x00, 0x01)."));
//  Serial.println(F(" "));
  delay(100);             
  do{  
      Serial.print(F("\n0-RB; 1-RW; 2-RBL; 3-WB; 4-WW; 5-WBL; 6-WRB\n"));
      Serial.print(F("7-SByte; 8-ER; 9-ERS; 'h'-Help; 'm'-Main Menu.\n"));
      Serial.print(F("Enter Smbus Function: [xx xx xx xx xx]\n"));  
      user_command = smbus_sent();                              //! Reads the user command
      if (user_command == 'm')   Serial.print(F("m\n"));        // Print m if it is entered
//      else  Serial.println(user_command);                       // Print user command         
    switch (user_command)
    {
      case 0:
        Serial.println(F("Smbus Read Byte:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.databyte = smbus_readByte(ps_i2c_address, sm.commands);
        Serial.printf("%02X: [%02X]\n", sm.commands, sm.databyte);
        delay(1000);       
      break;
      
      case 1:
        Serial.println(F("Smbus Read Word:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.dataword = smbus_readWord(ps_i2c_address, sm.commands);
        Serial.printf("%02X: [%02X %02X]\n", sm.commands, sm.dataword >> 8, (uint8_t)sm.dataword);
        delay(1000);
      break;
        
      case 2:
        Serial.println(F("Smbus Read Block:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.blocksize = smbus_data[3]; //Max size 32 block
        smbus_readBlock(ps_i2c_address, sm.commands, sm.datablock, sm.blocksize);
        Serial.printf(" %02X:", sm.commands);
        for (int n = 0; n < sm.blocksize; n++){
          Serial.printf(" %02X", sm.datablock[n]);
          delay(100);
        }
        Serial.println(F(" "));
        delay(1000);
      break;
            
      case 3:
        Serial.println(F("Smbus Write Byte:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.databyte = smbus_data[3];
        smbus_writeByte(ps_i2c_address, sm.commands, sm.databyte);
        Serial.println(F("Done."));
        delay(1000); 
        break;
                
      case 4:
        Serial.println(F("Smbus Write Word:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.dataword = smbus_data[3] << 8 | smbus_data[4];
        smbus_writeWord(ps_i2c_address, sm.commands, sm.dataword);
        Serial.println(F("Done."));
        delay(1000);
        break;
                
      case 5:
        Serial.println(F("Smbus Write Block:"));
        delay(100);
          ps_i2c_address = smbus_data[1];
          sm.commands = smbus_data[2];
          sm.blocksize = smbus_data[3];   // size Max 256 
          for(int i = 0; i < sm.blocksize; i++) {
            sm.datablock[i] = smbus_data[4+i];;
            Serial.printf("Block n=%02X Data:%02X\n", i, sm.datablock[i]);
            delay(100);     
          }
          smbus_writeBlock(ps_i2c_address, sm.commands, sm.datablock, sm.blocksize);
          Serial.println(F("Done."));
          delay(1000);     
        break;
      
       case 6:
        Serial.println(F("Smbus Write Read Blocks:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        sm.blocksize = smbus_data[3];
        for(int i = 0; i < sm.blocksize; i++) {
          sm.datablock[i] = smbus_data[i+4];
//          Serial.printf("Block n=%02X Data:%02X\n", i, sm.datablock[i]);
//          delay(100);     
        }
        sm.blocksize_b = smbus_data[sm.blocksize+4];    
        smbus_writeReadBlock (ps_i2c_address, sm.commands, sm.datablock, sm.blocksize, sm.datablock_b, sm.blocksize_b);      
        Serial.printf("%02X:", sm.blocksize_b);              
        for (int n = 0; n < sm.blocksize_b; n++){
          Serial.printf(" %02X", sm.datablock_b[n]);
          delay(100);
        }
        Serial.println(F(" "));
        delay(1000);     
        break;

       case 7:
        Serial.println(F("Smbus Sent Byte:"));
        delay(100);
        ps_i2c_address = smbus_data[1];
        sm.commands = smbus_data[2];
        smbus_sendByte(ps_i2c_address, sm.commands);
        Serial.println(F("Done."));
        delay(1000);       
          break;
         
      case 8:
        Serial.println(F("EEPROM Read Byte"));
        eeprom_address = smbus_data[1];
        offset = smbus_data[2] << 8 | smbus_data[3];
        if(smbus_data[4] == 1) eepromsize = false;
        else eepromsize = true;
        eepbuffer[0] = eepromreadbyte(eeprom_address, offset);
        Serial.printf("offset0x%04X: %02X\n", offset, eepbuffer[0]);
        Serial.printf("EEPROMSIZE 0x:%02X\n", smbus_data[4]);
        delay(1000);
        break;
      
      case 9:
        Serial.println(F("EEPROM Read Bytes"));
        eeprom_address = smbus_data[1];
        offset = smbus_data[2] << 8 | smbus_data[3];
        count = smbus_data[4];
        if(smbus_data[5] == 1) eepromsize = false;
        else eepromsize = true;
        eepromreadbytes(eeprom_address, offset, count, eepbuffer);
        Serial.printf("offset0x%04X:\n", offset);
        printFru(0, count-1, eepbuffer); 
        delay(1000);
        break;
      case 'h':
         SyntaxHelp();
         delay(1000);         
      default:
        if (user_command != 'm' && user_command != 'h')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}

uint8_t tohex(uint8_t val){
  uint8_t hex;
  if ( val - '0' >= 0 && val - '0' <=9){
     hex = val - '0';
  }
  else if( val == 'a' || val == 'A') hex = 10;
  else if( val == 'b' || val == 'B') hex = 11;
  else if( val == 'c' || val == 'C') hex = 12;
  else if( val == 'd' || val == 'D') hex = 13;
  else if( val == 'e' || val == 'E') hex = 14;
  else if( val == 'f' || val == 'F') hex = 15;
  else {
    Serial.println(F("Invalid Hex Data" ));
    delay(100);
    return 0;
  }
  return hex;
}

uint8_t smbus_sent(){
  uint8_t count;
  bool martflag = false;
  Serial.println(F(" " ));
  read_data();
  if (ui_buffer[0] == 'm') return('m');
  if (ui_buffer[0] == 'h') return('h');
  if (ui_buffer[0] != '['){
    Serial.println(F("Smbus Invaild format" ));
    delay(100);
    return 0xff;
  }
  smbus_data[0] = tohex(ui_buffer[1])*16 + tohex(ui_buffer[2]);
//  Serial.printf("Data[0] is: %02x \n", data[0]);
//  delay(100);
  for(int i = 0; i < 32; i++){
    if (ui_buffer[3*i+3] == ']') break;
    smbus_data[i+1] = tohex(ui_buffer[3*i+4])*16 + tohex(ui_buffer[3*i+5]);
    count = i + 2;
    if (i == 31) {
      Serial.println(F("Smbus Invalid format" ));
      martflag = true;
      delay(100);
      break; 
    }
  }
  if (martflag) return 0xff;
  Serial.print(F("[" ));
  Serial.printf("%02x", smbus_data[0]);
  for(int i = 1; i < count; i++){
    Serial.printf(" %02x", smbus_data[i]);
  }
  Serial.println(F("]" ));
  if (smbus_data[0] >= 0 && smbus_data[0] <=9) return smbus_data[0];
  return 0;
}

void SyntaxHelp(){
      Serial.println(F("Command Syntax: "));
      Serial.println(F("0-Read Byte(RB)   [Smbus_Fn=00 I2C_Addr Smbus_CMD]"));
      Serial.println(F("1-Read Word(RW)   [Smbus_Fn=01 I2C_Addr Smbus_CMD]"));
      Serial.println(F("2-Read Block(RBL) [Smbus_Fn=02 I2C_Addr Smbus_CMD BlockSize]"));
      Serial.println(F("3-Write Byte(WB)  [Smbus_Fn=03 I2C_Addr Smbus_CMD DataByte]"));
      Serial.println(F("4-Write Word(WW)  [Smbus_Fn=04 I2C_Addr Smbus_CMD DataMSB DataLSB]"));
      Serial.println(F("5-Write Block(WBL)      [Smbus_Fn=05 I2C_Addr Smbus_CMD BlockSize DataMSB .. DataLSB]"));
      Serial.println(F("6-Write Read Block(WRB) [Smbus_Fn=06 I2C_Addr Smbus_CMD BlockSize DataMSB .. DataLSB ReadBlockSize]"));
      Serial.println(F("7-Sent Byte(SBY)        [Smbus_Fn=07 I2C_Addr Smbus_CMD]"));
      Serial.println(F("8-EEPROM Read Byte(ER)   [Fn=08 I2C_Addr OffsetMSB OffsetLSB]"));
      Serial.println(F("9-EEPROM Read Bytes(ERS) [Fn=09 I2C_Addr OffsetMSB OffsetLSB Count]"));
      Serial.println(F("if eeprom data size < 0x100,Add the 01 Parameters at the end if not Ignore.\n"));
      Serial.println(F("'h' Display This Help."));
      Serial.println(F("'m' Main Menu."));
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

void printhelp(){
      Serial.print(F("\n Here are commands can be used.\r\n "));
      Serial.print(F(" 1 > Moitor the PSU Status Word \r\n "));
      Serial.print(F(" 2 > Set Standby/(Other Page) \r\n "));
      Serial.print(F(" 3 > TBD \r\n "));
      Serial.print(F(" 4 > TBD \r\n "));
      Serial.print(F(" 5 > Detect PSU Address in PMbus. \r\n "));
      Serial.print(F(" 6 > TBD \r\n "));
      Serial.print(F(" 7 > TBD \r\n "));
      Serial.print(F(" 8 > Enter MFR Commands \r\n "));
      Serial.print(F(" 9 > Enable/Disable PEC \r\n "));
      Serial.print(F(" 0 > Set All to Default \r\n "));
      Serial.print(F(" a > Set the PUS Model \r\n "));
      Serial.print(F(" e > Set the PMbus Interval Timing \r\n "));
      Serial.print(F(" c > Enter The SMbus Command Function & Read EEPROM \r\n "));
      Serial.print(F(" r > Set the PMbus/SMbus Address \r\n "));
      Serial.print(F(" h > Display This Help \r\n "));
      Serial.println("");
      Serial.print("Enter a command: ");      
      delay(500);  
}
