#ifndef FRAME_H

#define FRAME_H

#include <stdint.h>

#define FRAME_LEN 3
// start byte
#define FRAME_SYNC 0xA5

#define FRAME_MASK_MAX 15

#define MOST_SIGNIFICANT_BIT_MASK 0x80

/* CRC-8/SMBus poly (x^8 + x^2 + x + 1). Divisible by (x + 1), so any odd
 * number of bit errors is caught — enough for a one-byte payload. */
#define FRAME_CRC_POLY 0x07

int frame_pack(uint8_t outFrame[3], uint8_t mask);

#endif