
/*
 * #include <Wire.h>
 * Modify "Wire.h", Wire.cpp", added the Wire.requestFromS to Support SMbus Wire.requestFrom.
 * in folder %USERPROFILE%\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\libraries\
 * 
 */
uint8_t i2c_requestFrom(uint8_t address, uint8_t *acceptBuffer, uint16_t quantity)
{
  return Wire.requestFrom((uint8_t)address, (uint8_t *) acceptBuffer, (uint16_t)quantity, (uint8_t)true);
}


int8_t i2c_readByte(uint8_t address, uint8_t *value)
{
  uint8_t ret = 0;
  Wire.beginTransmission(address);
  i2c_requestFrom(address, value, (uint16_t)1);
  return ret;
}

// Write "value" byte to device at "address"
int8_t i2c_writeByte(uint8_t address, uint8_t value)
{
  int8_t ret = 1;
  Wire.beginTransmission(address);
  Wire.write(value);
  ret = Wire.endTransmission(Protocol);
  return ret;
}

int8_t i2c_readByteData(uint8_t address, uint8_t command, uint8_t *value)
{
  int8_t ret = 1;
  Wire.beginTransmission(address);
  Wire.write(command);
  ret = Wire.endTransmission(false);
  Wire.beginTransmission(address);
  i2c_requestFrom(address, value, (uint16_t)1);
  return ret;                             // Return result
}

int8_t i2c_writeByteData(uint8_t address, uint8_t command, uint8_t value)
{
  int8_t ret = 1;
  
  Wire.beginTransmission(address);
  Wire.write(command);
  Wire.write(value);
  ret = Wire.endTransmission(Protocol);
  return ret;
}

// Read a 16-bit word of data from register specified by "command"
int8_t i2c_readWordData(uint8_t address, uint8_t command, uint16_t *value)
{
  int8_t ret = 1;

  Wire.beginTransmission(address);
  Wire.write(command);
  ret = Wire.endTransmission(false);
  Wire.beginTransmission(address);
  uint8_t tempHolder[2];
  i2c_requestFrom(address, tempHolder, (uint16_t)2);
  *value = tempHolder[0] << 8;
  *value |= tempHolder[1];

  return ret;
}

// Write a 16-bit word of data to register specified by "command"
int8_t i2c_writeWordData(uint8_t address, uint8_t command, uint16_t value)
{
  int8_t ret = 1;

  Wire.beginTransmission(address);
  Wire.write(command);
  Wire.write(value >> 8);
  Wire.write(value & 0xFF);
  ret = Wire.endTransmission(Protocol);

  return ret;
}

int8_t i2c_readBlockData(uint8_t address, uint8_t command, uint16_t length, uint8_t *values)
{
  int8_t ret = 0;
  Wire.beginTransmission(address);
  Wire.write(command);
  ret = Wire.endTransmission(false);
  Wire.beginTransmission(address);
  i2c_requestFrom(address, values, length);

  return ret;
}


// Read a block of data, no command byte, reads length number of bytes and stores it in values.
int8_t i2c_readBlockData(uint8_t address, uint16_t length, uint8_t *values)
{
  int8_t ret = 0;

  Wire.beginTransmission(address);
  i2c_requestFrom(address, values, length);

  return ret;
}

// Write a block of data, starting at register specified by "command" and ending at (command + length - 1)
int8_t i2c_writeBlockData(uint8_t address, uint8_t command, uint16_t length, uint8_t *values)
{
  uint8_t i = length;
  int8_t ret = 1;

  Wire.beginTransmission(address);
  Wire.write(command);
  do
  {
    i--;
  }
  while (Wire.write(values[length - 1 - i]) == 1 && i > 0);

  ret = Wire.endTransmission(Protocol);

  return ret;
}

// Write two command bytes, then receive a block of data
int8_t i2c_twoByteCommandReadBlock(uint8_t address, uint16_t command, uint16_t length, uint8_t *values)
{
  int8_t ret = 0;

  Wire.beginTransmission(address);
  Wire.write(command >> 8);
  Wire.write(command & 0xFF);
  ret = Wire.endTransmission(false);
  Wire.beginTransmission(address);
  i2c_requestFrom(address, values, length);
  return ret;
}

int8_t i2c_blockCommandReadBlock(uint8_t address, uint8_t clength, uint8_t *commands, uint16_t length, uint8_t *values)
{
  uint8_t i = clength;
  int8_t ret = 1;
  Wire.beginTransmission(address);
  Wire.write(commands[0]);
  i = i - 1;
  do
  {
    i--;
  }
  while (Wire.write(commands[clength - 1 - i]) == 1 && i > 0);
  ret = Wire.endTransmission(false);
  Wire.beginTransmission(address);
  i2c_requestFrom(address, values, length);
  return ret;
}


uint8_t eepromreadbyte(int address, uint16_t offset)
{
  uint8_t data;
  Wire.beginTransmission(address);
  if(eepromsize)
  Wire.write((int)(offset >> 8));
  Wire.write((int)(offset & 0xFF));
  Wire.endTransmission(I2C_NOSTOP);
  Wire.requestFrom(address, 1);
  data = Wire.read();
  return data;
}

void eepromreadbytes(int address, uint16_t offset, int count, uint8_t * dest)
{
  Wire.beginTransmission(address);
  if(eepromsize)
  Wire.write((int)(offset >> 8));
  Wire.write((int)(offset & 0xFF));
  Wire.endTransmission(I2C_NOSTOP);
  uint8_t i = 0;
  Wire.requestFrom(address, count);
  while (Wire.available()) {
    dest[i++] = Wire.read();
  }
}



uint8_t read_data()
{
  uint8_t index = 0; //index to hold current location in ui_buffer
  int c; // single character used to store incoming keystrokes
  while (index < UI_BUFFER_SIZE-1)
  {
    ESP.wdtFeed();      // wdtFeed or delay can solve the ESP8266 SW wdt reset while is waiting for serial data
//    delay(1);
    c = Serial.read(); //read one character
    if (((char) c == '\r') || ((char) c == '\n')) break; // if carriage return or linefeed, stop and return data
    if ( ((char) c == '\x7F') || ((char) c == '\x08') )   // remove previous character (decrement index) if Backspace/Delete key pressed      index--;
    {
      if (index > 0) index--;
    }
    else if (c >= 0)
    {
      ui_buffer[index++]=(char) c; // put character into ui_buffer
    }
  }
  ui_buffer[index]='\0';  // terminate string with NULL

  if ((char) c == '\r')    // if the last character was a carriage return, also clear linefeed if it is next character
  {
    delay(10);  // allow 10ms for linefeed to appear on serial pins
    if (Serial.peek() == '\n') Serial.read(); // if linefeed appears, read it and throw it away
  }

  return index; // return number of characters, not including null terminator
}

int32_t read_int()
{
  int32_t data;
  read_data();
  if (ui_buffer[0] == 'm')
    return('m');
  if ((ui_buffer[0] == 'B') || (ui_buffer[0] == 'b'))
  {
    data = strtol(ui_buffer+1, NULL, 2);
  }
  else
    data = strtol(ui_buffer, NULL, 0);
  return(data);
}

int8_t read_char()
{
  read_data();
//  delay(1);
  return(ui_buffer[0]);
}

// Read a string from the serial interface.  Returns a pointer to the ui_buffer.
char *read_string()
{
  read_data();
  return(ui_buffer);
}
