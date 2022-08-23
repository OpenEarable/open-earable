//
// Created by Dylan Ray Roodt on 23.05.2022.
//

#ifndef IMU_SENSOR_EARABLE_H
#define IMU_SENSOR_EARABLE_H

#include "LSM6DSRSensor.h"
#include <Wire.h>

class IMU_Sensor_Earable {
public:
    IMU_Sensor_Earable();
    void start();
    void end();

    void get_acc(float& x, float& y, float& z);
    void get_gyro(float& x, float& y, float& z);

    const int sensor_count = 2;

private:
    bool available = false;
    LSM6DSRSensor * IMU;
};

#endif //IMU_SENSOR_EARABLE_H
