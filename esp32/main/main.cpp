#include "buttons.hpp"
#include "http_state.hpp"
#include "shared_reading.hpp"

extern "C" void app_main(void)
{
    // the Reading object is not heap-allocated. The mutex is the only object taken from the heap.
    static Reading reading;
    static SharedReading shared;

    shared.reading = &reading;
    shared.mutex = xSemaphoreCreateMutex();

    StartButtons(&shared);
    StartNetwork(&shared);
}
