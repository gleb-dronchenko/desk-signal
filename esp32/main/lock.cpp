#include "lock.hpp"


Lock::Lock(SemaphoreHandle_t mutex)
    : mutex_(mutex)
{
    xSemaphoreTake(mutex_, portMAX_DELAY);
}

Lock::~Lock()
{
    xSemaphoreGive(mutex_);
}