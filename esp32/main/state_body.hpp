#pragma once

#include <cstddef>
#include <cstdint>

int WriteStateBody(char *dst, size_t size, uint8_t mask, bool fresh);
