#pragma once

#include "reading.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

void StartNetwork(Reading *reading, SemaphoreHandle_t mutex);
