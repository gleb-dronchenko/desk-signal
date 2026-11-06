#pragma once

#include "reading.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// a Reading never takes the mutex, so the tests can compile it without FreeRTOS.
// the caller holds mutex across one Reading method.
struct SharedReading
{
    Reading *reading;
    SemaphoreHandle_t mutex;
};
