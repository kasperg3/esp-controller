

#pragma once

#include "hal/gpio_types.h"

class EspGPIO {
   public:
    EspGPIO(gpio_num_t pin, gpio_mode_t mode);

    EspGPIO(gpio_num_t pin) : EspGPIO(pinNumber, GPIO_MODE_INPUT_OUTPUT){};

    EspGPIO(gpio_num_t pin, gpio_mode_t mode, bool isInterruptPin);

    int read();

    void set();

    void clear();

    gpio_num_t getPinNumber();

    void setPinMode(gpio_mode_t mode);

    void attachInterrupt(gpio_isr_t function, void* args);

    void pullUpEnabled();

    void pullUpDisabled();

   private:
    gpio_num_t pinNumber;
};
