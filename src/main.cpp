#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "peripherals/GPIO/EspGPIO.h"
#include "peripherals/Sensor/Bmx055Driver.h"
#include "peripherals/Serial/EspI2CMaster.h"

static const float PI = 3.14159265359;
static const char* TAG = "MAIN";

extern "C" {
void app_main(void);
}

void gpioTest(void* parameter) {
    gpio_num_t gpioPin = GPIO_NUM_33;
    ESP_LOGI(TAG, "STATIN GPIO TEST: SETTING GPIO %i ", gpioPin);
    EspGPIO io = EspGPIO(gpioPin, GPIO_MODE_OUTPUT);
    // gpio_set_direction(gpioPin, GPIO_MODE_OUTPUT);

    int cnt = 0;
    while (1) {
        // ESP_LOGI(TAG, "cnt: %i", cnt++);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        if (cnt % 2) {
            // ESP_LOGI(TAG, "Pin %i OFF", gpioPin);
            io.clear();
        } else {
            // ESP_LOGI(TAG, "Pin %i ON", gpioPin);
            io.set();
        }
        cnt++;
    }
}

void i2cTest() {
    ESP_LOGI(TAG, "Starting i2c master...");
    EspI2CMaster i2cMaster = EspI2CMaster();
    ESP_LOGI(TAG, "Starting IMU Driver...");
    BMX055Driver imu = BMX055Driver(&i2cMaster);
    double accData[3] = {0};
    double gyrData[3] = {0};
    
    while (true) {
        imu.getAccAngle(accData);
        imu.getGyroRelativeAngle(gyrData);
        ESP_LOGI(TAG, "GYROMETER: x: %f , y: %f , z: %f  ", gyrData[0] * 180 / PI, gyrData[1]* 180 / PI, gyrData[2]* 180 / PI);
        ESP_LOGI(TAG, "ACCELEROMETER: x: %f , y: %f , z: %f  ", accData[0]*180/PI, accData[1]*180/PI, accData[2]*180/PI);
        vTaskDelay(10);
    }
}

// # Work in progress 
// * Github build check
// * Filters(kalman) for IMU driver
// * data logging/plotting remote
// * iot


void app_main(void) {
    i2cTest();
}
