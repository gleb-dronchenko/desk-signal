#pragma once

#include <cstdint>

class Reading
{
    public:
        static constexpr uint8_t kForwardBit = 1;
        static constexpr uint8_t kBackBit = 2;
        static constexpr uint8_t kLeftBit = 4;
        static constexpr uint8_t kRightBit = 8;

        Reading() = default;

        void Accept(uint8_t mask, int64_t now_micros);
        void Expire(int64_t now_micros);
        void Copy(uint8_t *mask, bool *fresh) const;

    private:
        uint8_t mask_ = 0;
        bool fresh_ = false;
        int64_t accepted_micros_ = 0;
};
