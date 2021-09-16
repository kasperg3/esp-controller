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
    double result[3] = {0, 0, 0};
    while (true) {
        vTaskDelay(100);
        imu.getAcc(result);
        double accX = result[0];
        double accY = result[1];
        double accZ = result[2];

        ESP_LOGI(TAG, "Acc value x: %f ", accX);
        ESP_LOGI(TAG, "Acc value y: %f ", accY);
        ESP_LOGI(TAG, "Acc value z: %f ", accZ);
    }
}

void app_main(void) {
    //wifi_example();
    // schedule gpio task
    //xTaskCreate(gpioTest, "Toggle LED", 4096, NULL, 1, NULL);
    // run the i2c task
    i2cTest();
    while (true) {
        vTaskDelay(10);
    }
}