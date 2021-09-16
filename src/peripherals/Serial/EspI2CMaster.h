// // TODO: https://github.com/espressif/esp-idf/tree/master/examples/peripherals/i2c/i2c_self_test

#ifndef MYRL_ESP_I2C_MASTER_H
#define MYRL_ESP_I2C_MASTER_H

#include <stdio.h>

#include "driver/i2c.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"

// i2c_set_period()
// enum CLOCK_SPEED {
//     KHz100 = 2500, /*!< 2500 = 10us = 100 kHz */
//     KHz400 = 626   /*!< 622 = 2.504us = 399.3610 kHz */
// };

// This class is not implementet as a singleton. It is possible to have multiple i2c masters/slave
// Todo make

class EspI2CMaster {
   private:
    uint8_t slaveAddress;

    int port = 0;
    void showBuf(uint8_t *buf, int len);

   public:
    EspI2CMaster();

    int writeRegister(uint8_t *buf, uint8_t slaveAddr, uint8_t regAddr, uint8_t size);

    int readRegister(uint8_t *buf, uint8_t slaveAddr, char regAddr, uint8_t size);

    int getSlaveAddress() const { return slaveAddress; }

    void setSlaveAddress(uint8_t addr) { slaveAddress = addr; }

    int getPort();

    ~EspI2CMaster();
};

#endif  // ESP_I2C_MASTER_H