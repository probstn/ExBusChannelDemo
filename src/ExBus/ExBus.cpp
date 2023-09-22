#include "ExBus.h"

JetiExBusProtocol::JetiExBusProtocol()
{
    reset();
}

void JetiExBusProtocol::reset()
{
}

int JetiExBusProtocol::decodeMessage(std::vector<uint8_t> *buff)
{
    if (buff->at(DATA_ID) == TELEMETRY_ID)
    {
        // telemetry data
        std::vector<uint8_t> telbuff(buff->begin() + DATA, buff->end() - 2);

        // check crc of telemetry data (normally done when reading from serial)
        uint8_t crc = crc8(&telbuff[1], telbuff.size() - 2);

        if (crc == telbuff.at(telbuff.size() - 1))
        {
            // crc ok
            Serial.println("CRC8 OK");

            telemetry.decodeData(telbuff);

            /*
            //debug:
            //0x9F 0x4C 0xA1 0xA8 0x5D 0x55 0x00 0x11 0xE8 0x23 0x21 0x1B 0x00 0xF4
            std::vector<uint8_t> db = {0x9F, 0x4C, 0xA1, 0xA8, 0x5D, 0x55, 0x00, 0x11, 0xE8, 0x23, 0x21, 0x1B, 0x00, 0xF4};
            telemetry.decodeData(db);
            */
        }
        else
        {
            // crc error
            Serial.println("CRC8 ERROR");
            // TODO: reset
        }
    }
    else
    {
        // some other data
        reset();
    }
    return 0;
}

/* 8-bit CRC polynomial X^8 + X^2 + X + 1 */
unsigned char update_crc(unsigned char crc, unsigned char crc_seed)
{
    unsigned char crc_u;
    unsigned char i;

    crc_u = crc;
    crc_u ^= crc_seed;

    for (i = 0; i < 8; i++)
    {
        crc_u = (crc_u & 0x80) ? CRC8_POLY ^ (crc_u << 1) : (crc_u << 1);
    }
    return crc_u;
}

unsigned char crc8(unsigned char *crc, unsigned char crc_lenght)
{
    unsigned char crc_up = 0;
    unsigned char c;

    for (c = 0; c < crc_lenght; c++)
    {
        crc_up = update_crc(crc[c], crc_up);
    }

    return crc_up;
}