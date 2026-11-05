#pragma once

#include "reading.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

struct SharedReading
{
    Reading *reading;
    SemaphoreHandle_t mutex;
};
