#include "buttons.hpp"
#include "lock.hpp"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/task.h"

static Reading *reading_ = nullptr;
static SemaphoreHandle_t mutex_ = nullptr;

extern "C" void ButtonsTask(void *)
{
    // сonfigure buttons as inputs
    gpio_config_t input = {};

    input.pin_bit_mask =
        (1ULL << GPIO_NUM_16) |
        (1ULL << GPIO_NUM_17) |
        (1ULL << GPIO_NUM_18) |
        (1ULL << GPIO_NUM_19);
    
    input.mode = GPIO_MODE_INPUT;
    input.pull_up_en = GPIO_PULLUP_ENABLE;
    input.pull_down_en = GPIO_PULLDOWN_DISABLE;
    input.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&input);

    // configure LED as output
    gpio_config_t led = {};

    led.pin_bit_mask = (1ULL << GPIO_NUM_2);
    led.mode = GPIO_MODE_OUTPUT;
    led.pull_up_en = GPIO_PULLUP_DISABLE;
    led.pull_down_en = GPIO_PULLDOWN_DISABLE;
    led.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&led);
    gpio_set_level(GPIO_NUM_2, 1);

    // main loop
    for (;;)
    {
        uint8_t mask = 0;

        // read buttons
        if (gpio_get_level(GPIO_NUM_16) == 0)
            mask |= 1;
        if (gpio_get_level(GPIO_NUM_17) == 0)
            mask |= 2;
        if (gpio_get_level(GPIO_NUM_18) == 0)
            mask |= 4;
        if (gpio_get_level(GPIO_NUM_19) == 0)
            mask |= 8;

        // update LED
        gpio_set_level(GPIO_NUM_2, mask == 0 ? 1 : 0);

        {
            // update reading
            Lock lock(mutex_);
            // accept buttons state
            reading_->Accept(mask, esp_timer_get_time());
        }

        // delay
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void StartButtons(Reading *reading, SemaphoreHandle_t mutex)
{
    reading_ = reading;
    mutex_ = mutex;
    
    // create buttons task
    xTaskCreate(ButtonsTask, "buttons", 4096, nullptr, 5, nullptr);
}