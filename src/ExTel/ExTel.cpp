#include "ExTel.h"

bool JetiExTelemetry::decodeData(std::vector<uint8_t> &buff)
{
    // decode telemetry data starting with EX ID

    // decrypt data
    crypt_data(&buff[1], buff.size() - 1);

    for (auto byte : buff)
    {
        switch (state)
        {

        case EX_ID:
            if (byte && 0x0F == 0x0F)
            {
                state = TYPExLEN;
            }
            else
            {
                return false;
                // reset
            }
            break;

        case TYPExLEN:
            int type = byte >> 6;  // 2 bits type
            int len = byte & 0x3F; // 6 bits byte

            Serial.print("type: ");
            Serial.println(type);

            Serial.print("len: ");
            Serial.println(len);

            // optional: check if received length is valid (should be as crc is ok)

            // packet with data starts at byte index 7 and ends at end-1 (1 before crc)
            switch (type)
            {
            case 0:
                // text protocol
                p_text(buff);
                break;

            case 1:
                // data protocol
                p_data(buff);
                break;

            case 2:
                // message protocol
                // decodeMessage();
                break;
            }
            state = SERIAL_UPPER;
            break;
        }
    }
    return true;
}

void JetiExTelemetry::p_text(std::vector<uint8_t> &packet)
{
    uint8_t id = packet.at(T_ID);
    uint8_t desc_len = packet.at(T_LENGTHS) >> 3;
    uint8_t unit_len = packet.at(T_LENGTHS) & 0x07;

    String desc;
    String unit;

    // get description
    for (int i = T_TEXT; i <= (T_TEXT + desc_len - 1); i++)
    {
        desc += (char)packet.at(i);
    }
    Serial.println(desc);

    // get unit
    for (int i = T_TEXT + desc_len; i <= (T_TEXT + desc_len + unit_len - 1); i++)
    {
        unit += (char)packet.at(i);
    }
    Serial.println(unit);

    // save label
    Serial.print("id: ");
    Serial.println(id);
    Serial.print("desc: ");
    Serial.println(desc);
    Serial.print("unit: ");
    Serial.println(unit);

    sensors[id].id = id;
    sensors[id].desc = desc;
    sensors[id].unit = unit;
}

void JetiExTelemetry::p_data(std::vector<uint8_t> &packet)
{
    for (int i = D_IDxTYPE; i < packet.size() - 1;)
    {
        // get id and data type
        uint8_t id = packet.at(i) >> 4;
        uint8_t dtype = packet.at(i) & 0x0F;
        i += 1; // jump to data index
        Serial.print(" id: ");
        Serial.print(id);
        Serial.print(" dtype: ");
        Serial.print(dtype);

        if (dtype == INT6_T)
        {
            // int6_t -> length = 1 Byte
            const int length = 8; // 8 bit

            uint8_t data = packet.at(i);
            bool sign = data & 0x80;
            int p_dec = data >> length - 3 & 0x03;
            double value = data & 0x1F;

            sensors[id].value = sign ? value / pow(10, p_dec) * -1 : value / pow(10, p_dec);

            Serial.print(" id: ");
            Serial.print(id);
            Serial.print(" value: ");
            Serial.println(sensors[id].value);

            i += 1; // jump to next id index
        }
        else if (dtype == INT14_T)
        {
            // int14_t -> length = 2 Bytes
            const int length = 16; // 16 bit

            uint16_t data = packet.at(i + 1) << 8 | packet.at(i);

            bool sign = data & 0x8000;
            int p_dec = data >> length - 3 & 0x03;
            double value = data & 0x1FFF;

            sensors[id].value = sign ? value / pow(10, p_dec) * -1 : value / pow(10, p_dec);

            Serial.print(" id: ");
            Serial.print(id);
            Serial.print(" value: ");
            Serial.println(sensors[id].value);

            i += 2; // jump to next id index
        }
        else if (dtype == INT22_T)
        {
            // int22_t -> length = 3 Bytes
            const int length = 24; // 24 bit

            uint32_t data = packet.at(i + 2) << 16 | packet.at(i + 1) << 8 | packet.at(i);

            bool sign = data & 0x800000;
            int p_dec = data >> length - 3 & 0x03;
            double value = data & 0x1FFFFF;

            sensors[id].value = sign ? value / pow(10, p_dec) * -1 : value / pow(10, p_dec);

            Serial.print(" id: ");
            Serial.print(id);
            Serial.print(" value: ");
            Serial.println(sensors[id].value);

            i += 3; // jump to next id index
        }
        else if (dtype == INT30_T)
        {
            // int30_t -> length = 4 Bytes
            const int length = 32; // 32 bit

            uint32_t data = packet.at(i + 3) << 24 | packet.at(i + 2) << 16 | packet.at(i + 1) << 8 | packet.at(i);

            bool sign = data & 0x80000000;
            int p_dec = data >> length - 3 & 0x03;
            double value = data & 0x1FFFFFFF;

            sensors[id].value = sign ? value / pow(10, p_dec) * -1 : value / pow(10, p_dec);

            Serial.print(" id: ");
            Serial.print(id);
            Serial.print(" value: ");
            Serial.println(sensors[id].value);

            i += 4; // jump to next id index
        }
    }
}

uint8_t pattern; // cipher pattern for encryption
void update_crypt(uint8_t *data, uint8_t position)
{
    uint8_t temp;

    temp = *data;
    temp ^= pattern;
    if (pattern & 0x02)
        pattern ^= 0x68 + position;
    else
        pattern ^= 0x57 - position;

    /* The header of the message let without cipher */
    if (position < 6)
    {
        pattern ^= temp;
    }
    else
    {
        *data = temp;
    }
}
void crypt_data(uint8_t *crypt, uint8_t crypt_len)
{
    uint8_t c;

    pattern = 0;

    for (c = 0; c < crypt_len; c++)
    {
        update_crypt(&crypt[c], c);
    }
}