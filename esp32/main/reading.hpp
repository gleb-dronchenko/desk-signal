#pragma once

#include <cstdint>

class Reading
{
    public:
        static constexpr uint8_t kForwardBit = 1;
        static constexpr uint8_t kBackBit = 2;
        static constexpr uint8_t kLeftBit = 4;
        static constexpr uint8_t kRightBit = 8;

        Reading();

        void Accept(uint8_t mask, int64_t timestamp);
        void Expire(int64_t timestamp);
        void Copy(uint8_t *mask, bool *fresh) const;

    private:
        uint8_t mask_;
        bool fresh_;
        int64_t accepted_timestamp_;
};