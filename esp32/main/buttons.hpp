#pragma once

#include "reading.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

void StartButtons(Reading *reading, SemaphoreHandle_t mutex);