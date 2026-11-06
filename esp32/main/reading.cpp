#include "reading.hpp"

void Reading::Accept(uint8_t mask, int64_t now_micros)
{
    mask_ = mask;
    fresh_ = true;
    accepted_micros_ = now_micros;
}

void Reading::Expire(int64_t now_micros)
{
    if (!fresh_)
        return;

    if (now_micros - accepted_micros_ >= 500000)
        fresh_ = false;
}

void Reading::Copy(uint8_t *mask, bool *fresh) const
{
    *mask = mask_;
    *fresh = fresh_;
}
