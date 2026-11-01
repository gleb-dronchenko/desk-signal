#include "buttons.hpp"
#include "http_state.hpp"
#include "reading.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern "C" void app_main(void) {
    static Reading reading;
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    
    StartButtons(&reading, mutex);
    StartNetwork(&reading, mutex);
}