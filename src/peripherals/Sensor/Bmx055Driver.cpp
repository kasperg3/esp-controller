//
// Created by kasper on 24/02/2021.
//

#include "Bmx055Driver.h"

#include <bitset>
#include <climits>
#include <iostream>
#include <ostream>

// #########################################################################
// #                           PinSetup                                    #
// #########################################################################

BMX055Driver::BMX055Driver(EspI2CMaster *serial) {
    i2c = serial;
    // #########################################################################
    // #                           Accelerometer                               #
    // #########################################################################
    uint8_t buf[] = {0x3};
    // Select PMU_Range register to 2g
    i2c->writeRegister(buf, ADDR_ACCEL, PMU_RANGE, 2);
    // Select PMU_BW register Bandwidth = 500 Hz
    buf[0] = {0xE};
    i2c->writeRegister(buf, ADDR_ACCEL, PMU_BW, 2);
    // Select PMU_LPW register Normal mode, Sleep duration = 0.5ms
    buf[0] = {0x00};
    i2c->writeRegister(buf, ADDR_ACCEL, PMU_LPW, 2);

    // #########################################################################
    // #                               GYRO                                    #
    // #########################################################################
    // Select gyro range. +-250degrees/s
    buf[0] = {0x03};
    i2c->writeRegister(buf, ADDR_GYRO, PMU_RANGE, 2);
    // Select bandwidth register Bandwidth = 1000 Hz (00100000) filtered bw=116
    buf[0] = {0x02};
    i2c->writeRegister(buf, ADDR_GYRO, PMU_BW, 2);
    // Select power mode register.  Normal mode, Sleep duration = 2ms (0x00)
    buf[0] = {0x00};
    i2c->writeRegister(buf, ADDR_GYRO, PMU_LPW, 2);

    // #########################################################################
    // #                               MAGNET                                  #
    // #########################################################################
    //buf[0] = {0x00};
    //i2c->writeRegister(buf, ADDR_MAGNET, 0x4C, 2);
}

// bit composition
// lsb: bit 7 to 4 is acc data, bit 1 contain new data flag. This bit is high if there this is new data since last read
// msb: bit 11 to 0 is acc data
// see docs, each measurement is 12 bits. We can read consequently at register 0x18
// which will increment for each read of 16bit
void BMX055Driver::sampleAccData(int *result) {
    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
    i2c->readRegister(data, ADDR_ACCEL, BURST_DATA_REGISTER, 6);
    for (int i = 0, j = 0; i < (sizeof(data) / sizeof(char)); i = i + 2, j++) {
        auto lsb = (int8_t)(data[i]);
        auto msb = (int8_t)data[i + 1];
        int combined = (((int16_t)msb) << 4) + (lsb >> 4);
        result[j] = combined;
    }
}

void BMX055Driver::sampleGyroData(int *result) {
    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
    i2c->readRegister(data, ADDR_GYRO, BURST_DATA_REGISTER, 6);
    for (int i = 0, j = 0; i < (sizeof(data) / sizeof(char)); i = i + 2, j++) {
        auto lsb = (int8_t)(data[i]);
        auto msb = (int8_t)data[i + 1];
        int combined = (((int16_t)msb) << 8) + lsb;
        result[j] = combined;
    }
}

// TODO make mag and hall sensor work
void BMX055Driver::sampleMagData(int *result) {
    uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    i2c->readRegister(data, ADDR_MAGNET, 0x42, 8);
    result[0] = (data[1] << 5) + (3 >> data[0]);  // magX
    result[1] = (data[3] << 5) + (3 >> data[2]);  // magY
    result[2] = (data[5] << 7) + (1 >> data[4]);  // magZ
    result[3] = (data[7] << 6) + (2 >> data[6]);  // hall
}

void BMX055Driver::getAcc(double *result) {
    int data[3] = {0, 0, 0};
    // The accelerometer only uses 12 bit of the 16 read
    sampleAccData(data);
    for (int i = 0; i < 3; i++) {
        // the range is +-2048=+-2g: Scale with 1024 to get the numbers in +-g's(9.82m/s²)
        result[i] = (double)data[i] / 1024;
    }
}

void BMX055Driver::getGyro(double *result) {
    int data[3] = {0, 0, 0};
    sampleGyroData(data);
    for (int i = 0; i < 3; i++) {
        // The data is 16 bit two complement (32768 is max integer)
        // The gyro outputs +-250*/s
        result[i] = ((double)data[i] / 32768) * 250;
    }
}

void BMX055Driver::getMag(double *result) {
    int data[3] = {0, 0, 0};
    sampleMagData(data);
    // The Magnetometer outputs 13 bits twos complement
    result[0] = ((double)data[0] / 4096);
    result[1] = ((double)data[1] / 4096);
    result[2] = ((double)data[2] / 16384);
    result[3] = ((double)data[3] / 8192);
}

BMX055Driver::~BMX055Driver() {
}
