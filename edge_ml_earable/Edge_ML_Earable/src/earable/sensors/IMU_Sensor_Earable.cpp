//
// Created by Dylan Ray Roodt on 23.05.2022.
//

#include "IMU_Sensor_Earable.h"

extern TwoWire Wire1;

IMU_Sensor_Earable::IMU_Sensor_Earable() {
    IMU = new LSM6DSRSensor(&Wire1, LSM6DSR_I2C_ADD_L);
}

void IMU_Sensor_Earable::start() {
    if (available) {
        return;
    }

    Wire1.begin();

    if (IMU->begin() == 0) {
        available = true;
        IMU->Enable_X();
        IMU->Enable_G();
    }
}

void IMU_Sensor_Earable::end() {
    if (!available) {
        return;
    }
    IMU->end();
    available = false;
}

void IMU_Sensor_Earable::get_acc(float& x, float& y, float& z) {
    // [-4, +4]g -/+0.122 mg
    if (!available) {
        return;
    }

    int32_t accelerometer[3];
    IMU->Get_X_Axes(accelerometer);

    x = (float)accelerometer[0];
    y = (float)accelerometer[1];
    z = (float)accelerometer[2];
}

void IMU_Sensor_Earable::get_gyro(float& x, float& y, float& z) {
    // [-2000, +2000] dps +/-70 mdps
    if (!available) {
        return;
    }

    int32_t gyroscope[3];
    IMU->Get_G_Axes(gyroscope);

    x = (float)gyroscope[0];
    y = (float)gyroscope[1];
    z = (float)gyroscope[2];
}
