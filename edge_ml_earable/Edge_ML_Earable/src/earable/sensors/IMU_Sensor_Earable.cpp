//
// Created by Tobias King on 26.10.2022.
//

#include "IMU_Sensor_Earable.h"

extern TwoWire Wire1;

IMU_Sensor_Earable::IMU_Sensor_Earable() {
    IMU = new DFRobot_BMX160(&Wire1);
}

void IMU_Sensor_Earable::start() {
    if (available) {
        return;
    }

    Wire1.begin();

    if (IMU->begin()) {
        available = true;
    }
}

void IMU_Sensor_Earable::end() {
    if (!available) {
        return;
    }
    available = false;
}

void IMU_Sensor_Earable::get_acc(float& x, float& y, float& z) {
    if (!available) {
        return;
    }

    IMU->getAllData(&magno_data, &gyro_data, &accel_data);

    x = (float) accel_data.x;
    y = (float) accel_data.y;
    z = (float) accel_data.z;

}

void IMU_Sensor_Earable::get_gyro(float& x, float& y, float& z) {
    if (!available) {
        return;
    }

    IMU->getAllData(&magno_data, &gyro_data, &accel_data);

    x = (float) gyro_data.x;
    y = (float) gyro_data.y;
    z = (float) gyro_data.z;
}

void IMU_Sensor_Earable::get_magno(float& x, float& y, float& z) {
    if (!available) {
        return;
    }

    IMU->getAllData(&magno_data, &gyro_data, &accel_data);

    x = (float) magno_data.x;
    y = (float) magno_data.y;
    z = (float) magno_data.z;
}


