#pragma once

#include <cstdint>

class Reading
{
    public:
        Reading();

        void Accept(uint8_t mask, int64_t timestamp);
        void Expire(int64_t timestamp);
        void Copy(uint8_t *mask, bool *fresh) const;

    private:
        uint8_t mask_;
        int64_t accepted_timestamp_;
        bool fresh_;
};