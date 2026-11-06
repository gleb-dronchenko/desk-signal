#include "state_body.hpp"

#include <stdio.h>
#include <string.h>

static int Fail(const char *name)
{
    printf("%s\n", name);

    return 1;
}

static int Expect(const char *name, uint8_t mask, bool fresh, const char *expected)
{
    char body[80];
    int length = WriteStateBody(body, sizeof(body), mask, fresh);

    if (length != (int)strlen(expected) || strcmp(body, expected) != 0)
    {
        return Fail(name);
    }

    return 0;
}

int main()
{
    int result = Expect(
        "mask 5",
        5,
        true,
        "{\"forward\":true,\"back\":false,\"left\":true,\"right\":false,\"fresh\":true}");

    if (result != 0)
    {
        return 1;
    }

    result = Expect(
        "mask 0",
        0,
        false,
        "{\"forward\":false,\"back\":false,\"left\":false,\"right\":false,\"fresh\":false}");

    if (result != 0)
    {
        return 1;
    }

    result = Expect(
        "mask 10",
        10,
        true,
        "{\"forward\":false,\"back\":true,\"left\":false,\"right\":true,\"fresh\":true}");

    if (result != 0)
    {
        return 1;
    }

    printf("Test passed\n");

    return 0;
}
