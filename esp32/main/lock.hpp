#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// a lock is a RAII wrapper for a semaphore.
class Lock
{
    public:
        explicit Lock(SemaphoreHandle_t mutex);
        
        ~Lock();

        Lock(const Lock &) = delete;
        Lock &operator=(const Lock &) = delete;
        Lock(Lock &&) = delete;
        Lock &operator=(Lock &&) = delete;

    private:
        SemaphoreHandle_t mutex_;
};