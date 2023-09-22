#include <Arduino.h>
#include <vector>
#include "./ExBus/ExBus.h"

enum EXFormat
{
    WAIT_HEAD,
    WAIT_HEAD_REQ,
    WAIT_LEN,
    WAIT_CRC,
};

class JetiExBusSerial : public JetiExBusProtocol
{
public:
    /**
     * @brief Construct a new JetiExBusSerial object
     * @param comPort Serial port number
     * @note Serial port number is 0 for Serial, 1 for Serial1, 2 for Serial2
     */
    JetiExBusSerial(int comPort);
    /**
     * @brief Initialize serial port
     */
    void begin();

    /**
     * @brief Receive message from serial port byte after byte
     * @note This function should be called repeadetly in loop()
     */
    void receiveMessage();
    void sendTelRequest(uint8_t id);

private:
    HardwareSerial *exserial;    // serial port
    std::vector<uint8_t> exbuff; // buffer for ExBusState message
    EXFormat state;              // state of state machine
    int i_byte;                  // index of the byte we are processing
    bool processing;             // flag to indicate that we are processing a message
    void reset();                // reset state machine
    void buffByte(uint8_t byte); // write bytes to buffer and check crc

    JetiExBusProtocol protocol;

};

uint16_t crc16_update(uint16_t crc, uint8_t data);
uint16_t crc16(uint8_t *p, uint16_t len);