#include <Arduino.h>

uint8_t throttle = 0;
uint16_t channel1 = 8040;

uint8_t data[38] = {
    0x3E,
    0x01,
    0x28,
    0x37,
    0x31, // identifier (channel values)
    0x20, // length (32 bytes)

    // channel 1
    0x82,
    0x1F,

    // channel 2
    0x82,
    0x1F,

    // channel 3
    0x82,
    0x1F,

    0x82,
    0x1F,

    0x82,
    0x1F,

    0x82,
    0x1F,

    0x82,
    0x1F,

    0x82,
    0x1F,
    0x82,
    0x1F,
    0x82,
    0x1F,
    0x82,
    0x1F,
    0x82,
    0x1F,
    0x82,
    0x1F,
    0x82,
    0x1F,
    0x82,
    0x1F,
    0x82,
    0x1F,
    // crc1
    // crc2
};

uint16_t crc_ccitt_update(uint16_t crc, uint8_t data);

void setup()
{
  Serial.begin(115200);
  Serial2.begin(125000, SERIAL_8N1, 16, 17);
}

void loop()
{
  for (int i = 0; i < 10; i++)
  {
    // 6,7 - channel 1
    data[6] = channel1 & 0xFF;
    data[7] = channel1 >> 8;

    uint16_t crcCalc;
    for (auto i : data)
    {
      Serial2.write(i);
      crcCalc = crc_ccitt_update(crcCalc, i);
    }
    Serial2.write((crcCalc & 0xFF));
    Serial2.write((crcCalc >> 8));
    delay(10);

    if(Serial.available())
    {
      char c = Serial.read();
      if(c == 'w')
      {
        throttle+=1;
      }
      else if(c == 's')
      {
        throttle-=1;
      }
      channel1 = throttle * (16080-8040) / (100-0) + 8040;

      Serial.print(channel1);
      Serial.print(" ");
      Serial.println(throttle);
    }
  }
}

// CRC calculation
//////////////////
uint16_t crc_ccitt_update(uint16_t crc, uint8_t data)
{
  uint16_t ret_val;
  data ^= (uint8_t)(crc) & (uint8_t)(0xFF);
  data ^= data << 4;
  ret_val = ((((uint16_t)data << 8) | ((crc & 0xFF00) >> 8)) ^ (uint8_t)(data >> 4) ^ ((uint16_t)data << 3));
  return ret_val;
}