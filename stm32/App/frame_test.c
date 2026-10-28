#include "frame.h"
#include <stdio.h>

static int expect_frame(uint8_t mask, uint8_t b0, uint8_t b1, uint8_t b2)
{
    uint8_t frame[FRAME_LEN] = { 0, 0, 0 };
    int result = frame_pack(frame, mask);

    if (result != 0 || frame[0] != b0 || frame[1] != b1 || frame[2] != b2) {
        printf("mask %d: got %d and %02X %02X %02X\n",
            (int)mask, result,
            (unsigned)frame[0], (unsigned)frame[1], (unsigned)frame[2]);

        return 1;
    }

    return 0;
}

static int expect_untouched(void)
{
    uint8_t frame[FRAME_LEN] = { 0x11, 0x22, 0x33 };
    int result = frame_pack(frame, 16);

    if (result != -1 || frame[0] != 0x11 || frame[1] != 0x22 || frame[2] != 0x33) {
        printf("mask 16: got %d and %02X %02X %02X\n",
               result,
               (unsigned)frame[0], (unsigned)frame[1], (unsigned)frame[2]);
        return 1;
    }
    return 0;
}

int main(void)
{
    if (expect_frame(0, 0xA5, 0x00, 0x59) != 0)
        return 1;
    if (expect_frame(1, 0xA5, 0x01, 0x5E) != 0)
        return 1;
    if (expect_frame(5, 0xA5, 0x05, 0x42) != 0)
        return 1;
    if (expect_untouched() != 0)
        return 1;
    
    printf("All tests passed\n");
    return 0;
}