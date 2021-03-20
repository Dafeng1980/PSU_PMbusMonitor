
uint8_t    runningpec;     //!< Temporary pec calc value

void pecClear(void)
{
  runningpec = 0;
}

void pecAdd(uint8_t byte_value)
{
  uint8_t i;
  runningpec = runningpec ^ byte_value;

  for (i=0; i<8; i++)
  {
    if ((runningpec & 0x80) > 0x00)
    {
      runningpec = (runningpec << 1) ^ 0x07; //0x07 poly used in the calc
    }
    else
    {
      runningpec = runningpec << 1;
    }

  }
}

uint8_t pecGet(void)
{
  return runningpec;
}


uint8_t smbus_waitForAck(uint8_t address, uint8_t command) //! Read with the address and command in loop until ack, 
{                                                          //  then issue stop 
  uint8_t data;
  // A real application should timeout at 4.1 seconds.
  uint16_t timeout = 8192;
  while (timeout-- > 0)
  {
    if (0 == i2c_readByteData(address, command, &data))
      return 1;    //SUCCESS
  }
  return 0;    //FAILURE
}

uint8_t smbus_readByte(uint8_t address, uint8_t command)
{
  if (PECENABLE)
  {
    uint8_t input[2];
    input[0] = 0x00;
    input[1] = 0x00;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd((address << 1) | 0x01);
    if (i2c_readBlockData(address, command, 2, input))
      Serial.print(F("Read Byte With Pec: fail.\n"));

    pecAdd(input[0]);
    if (pecGet() != input[1])
      Serial.print(F("Read Byte With Pec: fail pec\n"));

    return input[0];
  }
  else
  {
    uint8_t result;

    if (i2c_readByteData(address, command, &result))
      Serial.print(F("Read Byte: fail.\n"));
    return result;
  }
}

uint16_t smbus_readWord(uint8_t address, uint8_t command){
  if (PECENABLE)
  {
    uint8_t input[3];
    input[0] = 0x00;
    input[1] = 0x00;
    input[2] = 0x00;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd((address << 1) | 0x01);

    if (i2c_readBlockData(address, command, 3, input))
      Serial.print(F("Read Word With Pec: fail.\n"));

    pecAdd(input[0]);
    pecAdd(input[1]);
    if (pecGet() != input[2])
      Serial.print(F("Read Word With Pec: fail pec\n"));

    return input[1] << 8 | input[0];
  }
  
  else
  {
    uint16_t rdata;
    if (i2c_readWordData(address, command, &rdata))
      Serial.print(F("Read Word: fail.\n"));
    return (rdata << 8) | (rdata >> 8);
  }
}

void smbus_writeByte(uint8_t address, uint8_t command, uint8_t data)
{
  if (PECENABLE)
  {
    uint8_t buffer[2];
    buffer[0] = data;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd(data);
    buffer[1] = pecGet();
    if (i2c_writeBlockData(address, command, 2, buffer))
      Serial.print(F("Write Byte With Pec: fail.\n"));
  }
  else
  {
    if (i2c_writeByteData(address, command, data))
      Serial.print(F("Write Byte: fail.\n"));
  }
}

void smbus_writeBytes(uint8_t *addresses, uint8_t *commands, uint8_t *data, uint8_t no_addresses)
{
  if (PECENABLE)
  {
    uint8_t buffer[2];
    uint16_t index = 0;

    while (index < no_addresses)
    {
      buffer[0] = data[index];
      pecClear();
      pecAdd(addresses[index] << 1);
      pecAdd(commands[index]);
      pecAdd(data[index]);
      buffer[1] = pecGet();

      if (i2c_writeBlockData(addresses[index], commands[index], 2, buffer))
        Serial.print(F("Write Bytes With Pec: fail.\n"));
      index++;
    }
  }
  else
  {
    uint16_t index = 0;

    while (index < no_addresses)
    {
      if (i2c_writeBlockData(addresses[index], commands[index], 1, &data[index]))
        Serial.print(F("Write Bytes: fail.\n"));
      index++;
    }
  }
}

void smbus_writeWord(uint8_t address, uint8_t command, uint16_t data)
{
  if (PECENABLE)
  {
    uint8_t buffer[3];
    buffer[0] = (uint8_t) (data & 0xff);
    buffer[1] = (uint8_t) (data >> 8);

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd(data & 0xff);
    pecAdd(data >> 8);
    buffer[2] = pecGet();
    if (i2c_writeBlockData(address, command, 3, buffer))
      Serial.print(F("Write Word With Pec: fail.\n"));
  }
  else
  {
    uint16_t rdata;
    rdata = (data << 8) | (data >> 8);
    if (i2c_writeWordData(address, command, rdata))
      Serial.print(F("Write Word: fail.\n"));
  }
}

void smbus_writeBlock(uint8_t address, uint8_t command, uint8_t *block, uint16_t block_size)
{
  if (PECENABLE)
  {
    uint16_t pos = 0;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd(block_size);

    while (pos < block_size)
      pecAdd(block[pos++]);
    uint8_t pec = pecGet();

    uint8_t *data_with_pec = (uint8_t *) malloc(block_size + 2);
    data_with_pec[0] = block_size;
    memcpy(data_with_pec + 1, block, block_size);
    data_with_pec[block_size + 1] = pec;

    if (i2c_writeBlockData(address, command, block_size + 2, data_with_pec))
      Serial.print(F("Write Block With Pec: fail.\n"));
    free(data_with_pec);
  }
  else
  {
    uint8_t *buffer = (uint8_t *)malloc(block_size + 1);
    buffer[0] = block_size;
    memcpy(buffer + 1, block, block_size);
    if (i2c_writeBlockData(address, command, block_size + 1, buffer))
      Serial.print(F("Write Block: fail.\n"));
    free(buffer);
  }
}

uint8_t smbus_readBlock(uint8_t address, uint8_t command, uint8_t *block, uint16_t block_size)
{
  if (PECENABLE)
  {
    uint16_t pos;
    uint8_t *buffer = (uint8_t *)malloc(block_size + 2);
    uint8_t actual_block_size;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd((address << 1) | 0x01);

    if (i2c_readBlockData(address, command, block_size + 2, buffer))

      if (buffer[0] > block_size)
        Serial.print(F("Read Block with PEC: fail size too big.\n"));

    memcpy(block, buffer + 1, block_size);

    for (pos = 0; pos<buffer[0] + 1u; pos++)
      pecAdd(buffer[pos]);
    if (pecGet() != buffer[buffer[0]+1])
      Serial.print(F("Read Block With Pec: fail pec\n"));

    actual_block_size = buffer[0];
    free(buffer);
    return actual_block_size;
  }
  else
  {
    uint8_t *buffer = (uint8_t *)malloc(block_size + 1);
    uint8_t actual_block_size;

    if (i2c_readBlockData(address, command, block_size + 1, buffer))
      Serial.print(F("Read Block: fail.\n"));
    if (buffer[0] > block_size)
    {
      Serial.print(F("Read Block: fail size too big.\n"));
    }
    memcpy(block, buffer + 1, block_size);

    actual_block_size = buffer[0];
    free(buffer);
    return actual_block_size;
  }
}


uint8_t smbus_writeReadBlock(uint8_t address, uint8_t command, uint8_t *block_out, uint16_t block_out_size, uint8_t *block_in, uint16_t block_in_size)
{
  if (PECENABLE)
  {
    uint16_t pos = 0;
    uint8_t actual_block_size;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd(block_out_size);
    while (pos < block_out_size)
      pecAdd(block_out[pos++]);


    uint8_t *buffer = (uint8_t *)malloc(block_out_size + 1);
    buffer[0] = block_out_size;
    memcpy(buffer + 1, block_out, block_out_size);
    Protocol = false;  //sends a restart message after transmission.
    if (i2c_writeBlockData(address, command, block_out_size + 1, buffer))
      Serial.print(F("Write/Read Block w/PEC: write fail\n"));
    free(buffer);



    pecAdd((address << 1) | 0x01);
    Protocol = true;  // sends a stop message after transmission
    buffer = (uint8_t *)malloc(block_in_size + 2);
    if (i2c_readBlockData(address, block_in_size + 2, buffer))
      Serial.print(F("Write/Read Block w/PEC: read fail.\n"));
    if (buffer[0] > block_in_size)
    {
      Serial.print(F("Write/Read Block w/PEC: fail read size too big.\n"));
    }
    memcpy(block_in, buffer + 1, block_in_size);

    for (pos = 0; pos<buffer[0] + 1u; pos++)
      pecAdd(buffer[pos]);
    if (pecGet() != buffer[buffer[0]+1])
      Serial.print(F("Write/Read Block w/Pec: fail pec\n"));

    actual_block_size = buffer[0];
    free(buffer);
    return actual_block_size;
  }
  else
  {
    uint8_t *buffer = (uint8_t *)malloc(block_out_size + 1);
    uint8_t actual_block_size;

    buffer[0] = block_out_size;
    memcpy(buffer + 1, block_out, block_out_size);

    Protocol = false;  //sends a restart message after transmission.
    if (i2c_writeBlockData(address, command, block_out_size + 1, buffer))
      Serial.print(F("Write/Read Block write fail\n"));
    free(buffer);
    Protocol = true;  // sends a stop message after transmission
    buffer = (uint8_t *)malloc(block_in_size + 1);
    if (i2c_readBlockData(address, block_in_size + 1, buffer))
      Serial.print(F("Write/Read Block: read fail.\n"));
    if (buffer[0] > block_in_size)
    {
      Serial.print(F("Write/Read Block: fail size too big.\n"));
    }
    memcpy(block_in, buffer + 1, block_in_size);

    actual_block_size = buffer[0];
    free(buffer);
    return actual_block_size;
  }
}


void smbus_sendByte(uint8_t address, uint8_t command)
{
  if (PECENABLE)
  {
    uint8_t pec;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pec = pecGet();

    if (i2c_writeBlockData(address, command, 1, &pec))
      Serial.print(F("Send Byte With Pec: fail.\n"));
  }
  else
  {
    if (i2c_writeByte(address, command))
      Serial.print(F("Send Byte: fail.\n"));
  }
}
