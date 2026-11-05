#include "state_body.hpp"
#include "reading.hpp"

#include <cstdio>

static const char *JsonBool(bool value)
{
    return value ? "true" : "false";
}

int WriteStateBody(char *dst, size_t size, uint8_t mask, bool fresh)
{
    return snprintf(
        dst,
        size,
        "{\"forward\":%s,\"back\":%s,\"left\":%s,\"right\":%s,\"fresh\":%s}",
        JsonBool((mask & Reading::kForwardBit) != 0),
        JsonBool((mask & Reading::kBackBit) != 0),
        JsonBool((mask & Reading::kLeftBit) != 0),
        JsonBool((mask & Reading::kRightBit) != 0),
        JsonBool(fresh));
}
