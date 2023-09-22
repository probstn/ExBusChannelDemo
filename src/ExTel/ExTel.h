#include <Arduino.h>
#include <map>
#include <vector>

#define MAX_SENSORS 16 // max number of sensors for dx16


/**
 * @brief Data types
 * @see https://www.jetimodel.com/support/telemetry-protocol/jeti-telemetry-communication-protocol.html
*/
enum dTypes {
    INT6_T = 0,
    INT14_T = 1,
    INT22_T = 4,
    INT30_T = 8,
};

/**
 * @brief Basic EX protocol structure
*/
enum ExTel {
    EX_ID = 0,
    TYPExLEN = 1,
    SERIAL_UPPER = 2,
    SERIAL_LOWER = 4,
    RESERVED = 6,
};

/**
 * @brief Data protocol structure
*/
enum Data {
    D_IDxTYPE = 7,
    D_DATA = 8,
};


/***
 * @brief Text protocol structure
*/
enum Text {
    T_ID = 7,
    T_LENGTHS = 8,
    T_TEXT = 9,
};

/**
 * @brief Message protocol structure
*/
enum Message {
    M_TYPE = 7,
    M_CLASSxLENGTH = 8,
    M_MESSAGE = 11,
};



struct Telemetry
{
    uint8_t id;
    String desc;
    String unit;
    double value; //always double (max 8 bytes)
};

class JetiExTelemetry
{
    public:
    bool decodeData(std::vector<uint8_t> &buff);

    private:
    Telemetry sensors[MAX_SENSORS]; // map of telemetry data
    uint16_t serial_upper; // manufacturer id
    uint16_t serial_lower; // device id
    ExTel state;

    void p_text(std::vector<uint8_t> &packet);
    void p_data(std::vector<uint8_t> & packet);
};

unsigned char update_crc (unsigned char crc, unsigned char crc_seed);
unsigned char crc8 (unsigned char *crc, unsigned char crc_lenght);


/* Copyright (c) 2010, JETI model s.r.o All rights reserved.                 */
/*****************************************************************************/

/*! \brief Elementary cipher function for particular byte of the EX packet
 *
 *  Function computes mask of cipher from header of the message.
 *  It is simple XOR symmetrical cypher.
 *
 *  \param *data     Pointer to  whole EX message
 *  \param position  Counter
 */
void update_crypt(uint8_t *data, uint8_t position);

/*! \brief Function for crypt whole message
 *
 *  The message is crypted member by member.
 *  The function changes the source data (*crypt).
 *
 *  \param *crypt    Poiter to whole EX message
 *  \param crypt_len Length of the EX message
 */
void crypt_data(uint8_t *crypt, uint8_t crypt_len);