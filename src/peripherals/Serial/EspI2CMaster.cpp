
#include "EspI2CMaster.h"

/*!< I2CMaster nack value */

//TODO: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html
// https://github.com/espressif/esp-idf/blob/master/examples/peripherals/i2c/i2c_self_test/main/i2c_example_main.c
#define I2C_MASTER_SCL GPIO_NUM_22
#define I2C_MASTER_SDA GPIO_NUM_21
#define I2C_MASTER_PORT_NUM 1
#define I2C_MASTER_FREQUENCY 100000
#define I2C_MASTER_TX_BUF_DISABLE 0             /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0             /*!< I2C master doesn't need buffer */
#define ESP_SLAVE_ADDR CONFIG_I2C_SLAVE_ADDRESS /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN true                       /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS false                     /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */

EspI2CMaster::EspI2CMaster() {
    // Default config
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA;  // select GPIO
    conf.scl_io_num = I2C_MASTER_SCL;  // select GPIO
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQUENCY;  // select frequency
    // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */

    ESP_ERROR_CHECK(i2c_param_config(getPort(), &conf));
    ESP_ERROR_CHECK(i2c_driver_install(getPort(), conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
}

/**
 *  test code to read slave
 *  We need to fill the buffer of esp slave device, then master can read them out.
 * ---------------------------------------------------------------------------------------
 * | start | slave_addr + rd_bit +ack | read n-1 bytes + ack | read 1 byte + nack | stop |
 * ---------------------------------------------------------------------------------------
 */
int EspI2CMaster::readRegister(uint8_t* buf, uint8_t slaveAddr, char regAddr, uint8_t size) {
    int result = 0;
    if (size > 0) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        // first, send device address (indicating write) & register to be read
        i2c_master_write_byte(cmd, (slaveAddr << 1), ACK_CHECK_EN);
        // send register we want
        i2c_master_write_byte(cmd, regAddr, ACK_CHECK_EN);
        // Send repeated start
        i2c_master_start(cmd);
        // now send device address (indicating read) & read data
        i2c_master_write_byte(cmd, (slaveAddr << 1) | READ_BIT, ACK_CHECK_EN);
        if (size > 1) {
            i2c_master_read(cmd, buf, size - 1, I2C_MASTER_ACK);
        }
        i2c_master_read_byte(cmd, buf + size - 1, I2C_MASTER_NACK);
        result = i2c_master_stop(cmd);
        result = i2c_master_cmd_begin(getPort(), cmd, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);
    }

    return result;
}

int EspI2CMaster::writeRegister(uint8_t* buf, uint8_t slaveAddr, uint8_t regAddr, uint8_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be written
    i2c_master_write_byte(cmd, (slaveAddr << 1) | WRITE_BIT, ACK_CHECK_EN);
    // send register we want
    i2c_master_write_byte(cmd, regAddr, ACK_CHECK_EN);
    // write the data
    i2c_master_write(cmd, buf, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(getPort(), cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

int EspI2CMaster::getPort() {
    return port;
}

/**
 * @brief test function to show buffer
 */
void EspI2CMaster::showBuf(uint8_t* buf, int len) {
    int i;

    for (i = 0; i < len; i++) {
        ESP_LOGI("I2CMaster", "value: %x", buf[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
}

EspI2CMaster::~EspI2CMaster() {
    i2c_driver_delete(getPort());
}
