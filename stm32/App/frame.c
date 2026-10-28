#include "frame.h"

static uint8_t crc8_byte(uint8_t crc, uint8_t byte)
{
    crc ^= byte; // XOR the crc with the byte

    // crc calculation algorithm
    for (int bit = 0; bit < 8; bit++) {
        if (crc & MOST_SIGNIFICANT_BIT_MASK) // check if the most significant bit is set
            crc = (uint8_t)((crc << 1) ^ FRAME_CRC_POLY); // XOR the crc with the polynomial
        else
            crc = (uint8_t)(crc << 1); // shift the crc left by 1
    }

    return crc;
}

static uint8_t crc8(uint8_t payload)
{
    // initialize crc value
    uint8_t crc = 0;
    
    crc = crc8_byte(crc, FRAME_SYNC);
    crc = crc8_byte(crc, payload);

    return crc;
}

int frame_pack(uint8_t outFrame[3], uint8_t mask)
{
    if (mask > FRAME_MASK_MAX) {
        return -1;
    }

    outFrame[0] = FRAME_SYNC;
    outFrame[1] = mask;
    outFrame[2] = crc8(mask);

    return 0;
}
