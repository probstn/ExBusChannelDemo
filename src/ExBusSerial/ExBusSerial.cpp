#include "ExBusSerial.h"

JetiExBusSerial::JetiExBusSerial(int comPort)
{
    switch(comPort) {
        case 0:
            exserial = &Serial;
            break;

        case 1:
            exserial = &Serial1;
            break;

        case 2:
            exserial = &Serial2;
            break;

        default:
            exserial = &Serial2;
            break;
    }
}

void JetiExBusSerial::begin()
{
    exserial->begin(125000);
    reset();
}

void JetiExBusSerial::sendTelRequest(uint8_t id) {
    std::vector<uint8_t> request = {0x3D, 0x01, 0x08, 0x06, 0x3A, 0x00, 0x98, 0x81};
    Serial.println("Sending request");

    for(auto byte : request) {
        exserial->write(byte);
    }
}

void JetiExBusSerial::receiveMessage()
{
    while (exserial->available() && !processing)
    {
        buffByte(exserial->read());
    }

    //std::vector<uint8_t> bytes = {0x3B, 0x01, 0x1C, 0x62, 0x3A, 0x14, 0xAF, 0x12, 0x49, 0x95, 0x3C, 0x55, 0xA1, 0xF2, 0xD4, 0x98, 0xA2, 0xB1, 0xE0, 0xB9, 0xF2, 0x96, 0xF7, 0xB4, 0xDC, 0x5C, 0x14, 0x8E}; //U Battery V
    //std::vector<uint8_t> bytes = {0x3B, 0x01, 0x21, 0x5E, 0x3A, 0x19, 0xAF, 0x57, 0x49, 0x95, 0x3C, 0x55, 0xA3, 0xE0, 0xD0, 0xD0, 0x9E, 0xCE, 0x9C, 0xB4, 0xBB, 0xCE, 0xB8, 0x81, 0xB7, 0xCE, 0x30, 0xF0, 0xB3, 0xCE, 0x20, 0x9E, 0x98}; //11,2V
    //std::vector<uint8_t> bytes = {0x3B, 0x01, 0x1E, 0x5E, 0x3A, 0x16, 0xAF, 0x54, 0x49, 0x95, 0x3C, 0x55, 0x65, 0x3C, 0x59, 0x09, 0x46, 0x66, 0x5C, 0x09, 0x27, 0x36, 0x41, 0x09, 0xEF, 0x37, 0x4D, 0x04, 0xFF, 0x5C};
    /*
    for (uint8_t byte : bytes)
    {
        buffByte(byte);
    }
    */
}

void JetiExBusSerial::buffByte(uint8_t byte)
{
    // receive ExBus message
    switch (state)
    {
    // HEAD
    case WAIT_HEAD:
        if (byte == 0x3B)
        {
            exbuff.push_back(byte);
            state = WAIT_HEAD_REQ;
        }
        else
            reset();
        break;

    // HEAD_REQ
    case WAIT_HEAD_REQ:
        if (byte == 0x01)
        {
            exbuff.push_back(byte);
            state = WAIT_LEN;
        }
        else
            state = WAIT_HEAD;
        break;

    // LEN
    case WAIT_LEN:
        exbuff.push_back(byte);
        i_byte = 3;
        state = WAIT_CRC;
        break;

    // END
    case WAIT_CRC:
        exbuff.push_back(byte);
        if (i_byte++ >= exbuff[2] - 1)
        {
            // check CRC
            uint16_t calc_crc = crc16(&exbuff[0], exbuff.size() - 2);
            uint16_t rec_crc = (exbuff[exbuff.size() - 1] << 8) | exbuff[exbuff.size() - 2];

            if(calc_crc == rec_crc) {
                processing = true;
                Serial.println("CRC16 OK");
                protocol.decodeMessage(&exbuff);

            } else {
                Serial.println("CRC16 error");
                reset();
            }
        }
        break;
    }
}

void JetiExBusSerial::reset()
{
    exbuff.clear();
    i_byte = 0;
    state = WAIT_HEAD;
}

uint16_t crc16_update(uint16_t crc, uint8_t data)
{
    uint16_t ret_val;
    data ^= (uint8_t)(crc) & (uint8_t)(0xFF);
    data ^= data << 4;
    ret_val = ((((uint16_t)data << 8) | ((crc & 0xFF00) >> 8)) ^ (uint8_t)(data >> 4) ^ ((uint16_t)data << 3));
    return ret_val;
}
uint16_t crc16(uint8_t *p, uint16_t len)
{
    uint16_t crc16_data = 0;

    while (len--)
    {
        crc16_data = crc16_update(crc16_data, p[0]);
        p++;
    }
    return (crc16_data);
}