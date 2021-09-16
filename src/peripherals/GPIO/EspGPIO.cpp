
#include "EspGPIO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

EspGPIO::EspGPIO(gpio_num_t pin, gpio_mode_t mode) {
    pinNumber = pin;

    gpio_pulldown_dis(pin);
    setPinMode(mode);
}

EspGPIO::EspGPIO(gpio_num_t pin, gpio_mode_t mode, bool isInterruptPin) {
    EspGPIO(pin, mode);
    if (isInterruptPin) {
        gpio_install_isr_service(GPIO_MODE_INPUT);
    }
}

int EspGPIO::read() {
    return gpio_get_level(getPinNumber());
}

void EspGPIO::setPinMode(gpio_mode_t mode) {
    ESP_ERROR_CHECK(gpio_set_direction(getPinNumber(), (gpio_mode_t)mode));
}

void EspGPIO::set() {
    ESP_ERROR_CHECK(gpio_set_level(getPinNumber(), 1));
}

void EspGPIO::clear() {
    ESP_ERROR_CHECK(gpio_set_level(getPinNumber(), 0));
}

void EspGPIO::pullUpEnabled() {
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_pulldown_en(getPinNumber()));
}

void EspGPIO::pullUpDisabled() {
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_pulldown_dis(getPinNumber()));
}

void EspGPIO::attachInterrupt(gpio_isr_t function, void* args) {
    // TODO:implement interrup service routine https://github.com/espressif/esp-idf/blob/master/examples/peripherals/gpio/generic_gpio/main/gpio_example_main.c
    // ISR can be used for motor encoder implementation
    gpio_isr_handler_add(getPinNumber(), function, args);
}

gpio_num_t EspGPIO::getPinNumber() {
    return pinNumber;
}
