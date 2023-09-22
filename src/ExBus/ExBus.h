#include <Arduino.h>
#include <map>
#include <vector>
#include "../ExTel/ExTel.h"

#define TELEMETRY_ID 0x3A
#define JETIBOX_ID   0x3B

#define CRC8_POLY 0x07

enum ExBus
{
    HEAD,
    HEAD_REQ,
    LEN,
    PACKET_ID,
    DATA_ID,
    SUB_LEN,
    DATA,
    CRC1,
    CRC2,
};

class JetiExBusProtocol
{
public:
    JetiExBusProtocol();
    int decodeMessage(std::vector<uint8_t> *buff); // returns id of telemetry data received
private:
    void reset(); // reset state machine
    JetiExTelemetry telemetry;
};