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
    buf[0] = {0x01};
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
}

// bit composition
// lsb: bit 7 to 4 is acc data, bit 1 contain new data flag. This bit is high if there this is new data since last read
// msb: bit 11 to 0 is acc data
// see docs, each measurement is 12 bits. We can read consequently at register 0x18
// which will increment for each read of 16bit
void BMX055Driver::sampleAccData(int *result, uint8_t addr, char dataReg) {
    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
    i2c->readRegister(data, ADDR_ACCEL, dataReg, 6);
    for (int i = 0, j = 0; i < (sizeof(data) / sizeof(char)); i = i + 2, j++) {
        auto lsb = (int8_t)(data[i]);
        auto msb = (int8_t)data[i + 1];
        int combined = (((int16_t)msb) << 4) + (lsb >> 4);
        result[j] = combined;
    }
}

void BMX055Driver::sampleGyroData(int *result, uint8_t addr, char dataReg) {
    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
    i2c->readRegister(data, ADDR_GYRO, dataReg, 6);
    for (int i = 0, j = 0; i < (sizeof(data) / sizeof(char)); i = i + 2, j++) {
        auto lsb = (int8_t)(data[i]);
        auto msb = (int8_t)data[i + 1];
        int combined = (((int16_t)msb) << 8) + lsb;
        result[j] = combined;
    }
}

void BMX055Driver::getAcc(double *result) {
    int data[3] = {0, 0, 0};
    // The accelerometer only uses 12 bit of the 16 read
    sampleAccData(data, ADDR_ACCEL, BURST_DATA_REGISTER);
    for (int i = 0; i < 3; i++) {
        // the range is +-2048=+-2g: Scale with 1024 to get the numbers in +-g's(9.82m/s²)
        result[i] = (double)data[i] / 1024;
    }
}

void BMX055Driver::getGyro(double *result) {
    int data[3] = {0, 0, 0};
    // The accelerometer only uses 12 bit of the 16 read
    sampleGyroData(data, ADDR_GYRO, BURST_DATA_REGISTER);
    for (int i = 0; i < 3; i++) {
        // the range is +-2048=+-2g: Scale with 1024 to get the numbers in +-g's(9.82m/s²)
        result[i] = ((double)data[i] / 32768) * 500;
    }
}

void BMX055Driver::getMag(double *) {
}

BMX055Driver::~BMX055Driver() {
}
