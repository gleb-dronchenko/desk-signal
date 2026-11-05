#include "reading.hpp"

void Reading::Accept(uint8_t mask, int64_t now_us)
{
    mask_ = mask;
    fresh_ = true;
    accepted_us_ = now_us;
}

void Reading::Expire(int64_t now_us)
{
    if (!fresh_)
        return;

    if (now_us - accepted_us_ >= 500000)
        fresh_ = false;
}

void Reading::Copy(uint8_t *mask, bool *fresh) const
{
    *mask = mask_;
    *fresh = fresh_;
}
