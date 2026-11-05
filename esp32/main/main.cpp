#include "buttons.hpp"
#include "http_state.hpp"
#include "shared_reading.hpp"

extern "C" void app_main(void)
{
    static Reading reading;
    static SharedReading shared;

    shared.reading = &reading;
    shared.mutex = xSemaphoreCreateMutex();

    StartButtons(&shared);
    StartNetwork(&shared);
}
