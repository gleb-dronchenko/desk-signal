#include "reading.hpp"

Reading::Reading()
    // constructor initializes the mask, fresh, and accepted timestamp props
    : mask_(0), fresh_(false), accepted_timestamp_(0)
{}


void Reading::Accept(uint8_t mask, int64_t timestamp)
{
    mask_ = mask;
    fresh_ = true;
    accepted_timestamp_ = timestamp;
}

void Reading::Expire(int64_t timestamp)
{
    if (!fresh_)
        return;

    if (timestamp - accepted_timestamp_ >= 500000)
        fresh_ = false;
}

void Reading::Copy(uint8_t *mask, bool *fresh) const
{
    *mask = mask_;
    *fresh = fresh_;
}