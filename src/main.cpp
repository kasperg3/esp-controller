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
    
    int progress = 0;

    while (true) {
        progress += 1;
        imu.getAccAngle(accData);

        imu.getGyroRelativeAngle(gyrData);
        ESP_LOGI(TAG, "GYROMETER: x: %f , y: %f , z: %f  ", gyrData[0] * 180 / PI, gyrData[1]* 180 / PI, gyrData[2]* 180 / PI);
        ESP_LOGI(TAG, "ACCELEROMETER: x: %f , y: %f , z: %f  ", accData[0]*180/PI, accData[1]*180/PI, accData[2]*180/PI);

        vTaskDelay(10);
    }
}

void writeToOled(void) {
    EspI2CMaster i2cMaster = EspI2CMaster();
    EspOledDriver oled = EspOledDriver(&i2cMaster);
}

// #include "lcdgfx.h"

void app_main(void) {
    i2cTest();
    // DisplaySSD1306_128x64_I2C display(-1);

    //  setup
    // display.begin();
    // display.setFixedFont(ssd1306xled_font6x8);
    // display.clear();
    // display.drawWindow(0, 0, 0, 0, "Downloading", true);

    // draw
    // while (true) {
        
    //     // display.drawProgressBar(progress);
    //     progress++;
    //     if (progress > 100) {
    //         progress = 0;
    //         vTaskDelay(2000);
    //     } else {
    //         vTaskDelay(50);
    //     }
    // }
    // wifi_example();
    //  schedule gpio task
    // xTaskCreate(gpioTest, "Toggle LED", 4096, NULL, 1, NULL);
    //  run the i2c task
    // i2cTest();

    // writeToOled();
    while (true) {
        vTaskDelay(10);
    }
}
