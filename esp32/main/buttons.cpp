#include "buttons.hpp"
#include "lock.hpp"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/task.h"

struct ButtonPin
{
    gpio_num_t pin;
    uint8_t bit;
};

static constexpr ButtonPin kButtons[] = {
    {GPIO_NUM_16, Reading::kForwardBit},
    {GPIO_NUM_17, Reading::kBackBit},
    {GPIO_NUM_18, Reading::kLeftBit},
    {GPIO_NUM_19, Reading::kRightBit},
};

static constexpr gpio_num_t kLedPin = GPIO_NUM_2;

static uint64_t ButtonPinMask()
{
    uint64_t pins = 0;

    for (const ButtonPin &button : kButtons)
        pins |= (1ULL << button.pin);

    return pins;
}

static uint8_t ReadMask()
{
    uint8_t mask = 0;

    for (const ButtonPin &button : kButtons)
    {
        if (gpio_get_level(button.pin) == 0)
            mask |= button.bit;
    }

    return mask;
}

static void ConfigurePins()
{
    gpio_config_t input = {};
    input.pin_bit_mask = ButtonPinMask();
    input.mode = GPIO_MODE_INPUT;
    input.pull_up_en = GPIO_PULLUP_ENABLE;
    input.pull_down_en = GPIO_PULLDOWN_DISABLE;
    input.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&input);

    gpio_config_t led = {};
    led.pin_bit_mask = (1ULL << kLedPin);
    led.mode = GPIO_MODE_OUTPUT;
    led.pull_up_en = GPIO_PULLUP_DISABLE;
    led.pull_down_en = GPIO_PULLDOWN_DISABLE;
    led.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&led);
    gpio_set_level(kLedPin, 1);
}

extern "C" void ButtonsTask(void *arg)
{
    SharedReading *shared = static_cast<SharedReading *>(arg);

    ConfigurePins();

    for (;;)
    {
        uint8_t mask = ReadMask();
        gpio_set_level(kLedPin, mask == 0 ? 1 : 0);

        {
            Lock lock(shared->mutex);
            shared->reading->Accept(mask, esp_timer_get_time());
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void StartButtons(SharedReading *shared)
{
    xTaskCreate(ButtonsTask, "buttons", 4096, shared, 5, nullptr);
}
