/* WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "peripherals/Display/EspOledDriver.h"
#include "peripherals/GPIO/EspGPIO.h"
#include "peripherals/Sensor/Bmx055Driver.h"
#include "peripherals/Serial/EspI2CMaster.h"

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
    //double magData[4] = {0};
    while (true) {
        vTaskDelay(10);
        imu.getAcc(accData);
        ESP_LOGI(TAG, "ACCELEROMETER: x: %f , y: %f , z: %f  ", accData[0], accData[1], accData[2]);
        memset(accData, 0, sizeof(accData));

        vTaskDelay(10);
        imu.getGyro(gyrData);
        ESP_LOGI(TAG, "GYROMETER: x: %f , y: %f , z: %f  ", gyrData[0], gyrData[1], gyrData[2]);
        memset(gyrData, 0, sizeof(gyrData));

        //vTaskDelay(10);
        //imu.getMag(magData);
        //ESP_LOGI(TAG, "MAGNETOMETER: x: %f , y: %f , z: %f , h: %f ", magData[0], magData[1], magData[2], magData[3]);
        //memset(magData, 0, sizeof(magData));
    }
}

void writeToOled(void) {
    EspI2CMaster i2cMaster = EspI2CMaster();
    EspOledDriver oled = EspOledDriver(&i2cMaster);
}

void app_main(void) {
    //wifi_example();
    // schedule gpio task
    //xTaskCreate(gpioTest, "Toggle LED", 4096, NULL, 1, NULL);
    // run the i2c task
    //i2cTest();

    writeToOled();
    while (true) {
        vTaskDelay(10);
    }
}