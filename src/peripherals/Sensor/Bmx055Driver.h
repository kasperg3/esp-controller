//
// Created by kasper on 24/02/2021.
//

#ifndef MYRL_BMX055DRIVER_H
#define MYRL_BMX055DRIVER_H
#include "../Serial/EspI2CMaster.h"
// BMX055 Accl I2C address is 0x18(24)
#define ADDR_ACCEL 0x18
#define BURST_DATA_REGISTER 0x02
// BMX055 Gyro I2C address is 0x68(104)
#define ADDR_GYRO 0x68
// BMX055 Mag I2C address is 0x10(16)
#define ADDR_MAGNET 0x10
#define PMU_BW 0x10
#define PMU_RANGE 0x0F
#define PMU_LPW 0x11
// TODO Improvements
// Add self test
// add calibration

class BMX055Driver {
    EspI2CMaster *i2c;

    void sampleData(int *result, uint8_t addr, char dataReg, uint8_t dataLength);

   public:
    BMX055Driver(EspI2CMaster *serial);

    ~BMX055Driver();

    void getAcc(double *);

    void getGyro(double *);

    void getMag(double *);

    void sampleAccData(int *result, uint8_t addr, char dataReg);

    void sampleGyroData(int *result, uint8_t addr, char dataReg);
};

#endif  //MYRL_BMX055DRIVER_H
