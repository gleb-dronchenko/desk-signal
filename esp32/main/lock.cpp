#include "lock.hpp"


// task context only. portMAX_DELAY waits out the other task; an interrupt must not take this.
Lock::Lock(SemaphoreHandle_t mutex)
    : mutex_(mutex)
{
    xSemaphoreTake(mutex_, portMAX_DELAY);
}

Lock::~Lock()
{
    xSemaphoreGive(mutex_);
}