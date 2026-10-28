#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

int board_read_move(uint8_t *mask);
int board_send_link(const uint8_t frame[3]);

void board_set_led(int on);
void board_log_move(uint8_t mask);

#endif