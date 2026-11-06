#include "reading.hpp"
#include <stdio.h>

int main()
{
    Reading reading;

    uint8_t mask;
    bool fresh;

    // constructor leaves mask 0 and freshness false
    reading.Copy(&mask, &fresh);
    if (mask != 0 || fresh)
    {
        printf("Test failed\n");
        return 1;
    }

    // expire before the first accept leaves the same pair
    reading.Expire(500000);
    reading.Copy(&mask, &fresh);
    if (mask != 0 || fresh)
    {
        printf("Test failed\n");
        return 1;
    }

    // accept stores mask 5 and turns freshness on
    reading.Accept(5, 1000);
    reading.Copy(&mask, &fresh);
    if (mask != 5 || !fresh)
    {
        printf("Test failed\n");
        return 1;
    }

    // 499999 us under the threshold keeps mask 5 and freshness
    reading.Expire(1000 + 499999);
    reading.Copy(&mask, &fresh);
    if (mask != 5 || !fresh)
    {
        printf("Test failed\n");
        return 1;
    }

    // exactly 500000 us clears freshness and keeps mask 5
    reading.Expire(1000 + 500000);
    reading.Copy(&mask, &fresh);
    if (mask != 5 || fresh)
    {
        printf("Test failed\n");
        return 1;
    }

    // another expire past the threshold leaves mask 5 and freshness false
    reading.Expire(1000 + 500000);
    reading.Copy(&mask, &fresh);
    if (mask != 5 || fresh)
    {
        printf("Test failed\n");
        return 1;
    }

    // accept after expiry stores mask 1 and turns freshness on again
    reading.Accept(1, 2000000);
    reading.Copy(&mask, &fresh);
    if (mask != 1 || !fresh)
    {
        printf("Test failed\n");
        return 1;
    }

    printf("Test passed\n");

    return 0;
}
