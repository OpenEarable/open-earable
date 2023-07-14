#include "IMU_Sensor.h"

extern TwoWire Wire1;

IMU_Sensor::IMU_Sensor() {
    IMU = new DFRobot_BMX160(&Wire1);
}

void IMU_Sensor::start() {
    if (available) {
        return;
    }
    Wire1.begin();
    if (IMU->begin()) {
        available = true;
    }

    reset_has_data();
}

void IMU_Sensor::end() {
    if (!available) {
        return;
    }
    IMU->softReset();
    available = false;

    reset_has_data();
}

void IMU_Sensor::get_data(int sensorID, byte *data) {
    float x, y, z;
    switch (sensorID) {
        case IMU_ACCELERATION:
            get_acc(x,y,z);
            break;
        case IMU_GYROSCOPE:
            get_gyro(x,y,z);
            break;
        case IMU_MAGNETOMETER:
            get_mag(x,y,z);
            break;
        default:
            break;
    }
    float * floatArray = (float*)data;
    floatArray[0] = x;
    floatArray[1] = y;
    floatArray[2] = z;
}

void IMU_Sensor::get_acc(float &x, float &y, float &z) {
    if (!available) {
        return;
    }

    age_check();

    const int index = 1;
    if (!has_data[index]) {
        get_all();
    }
    has_data[index] = false;

    x = (float) accel_data.x;
    y = (float) accel_data.y;
    z = (float) accel_data.z;
}

void IMU_Sensor::get_gyro(float &x, float &y, float &z) {
    if (!available) {
        return;
    }

    age_check();

    const int index = 2;
    if (!has_data[index]) {
        get_all();
    }
    has_data[index] = false;

    x = (float) gyro_data.x;
    y = (float) gyro_data.y;
    z = (float) gyro_data.z;
}

void IMU_Sensor::get_mag(float &x, float &y, float &z) {
    if (!available) {
        return;
    }

    age_check();

    const int index = 3;
    if (!has_data[index]) {
        get_all();
    }
    has_data[index] = false;

    x = (float) magno_data.x;
    y = (float) magno_data.y;
    z = (float) magno_data.z;
}

int IMU_Sensor::get_sensor_count() {
    return sensor_count;
}

void IMU_Sensor::get_all() {
    IMU->getAllData(&magno_data, &gyro_data, &accel_data);

    for (bool & i : has_data) {
        i = true;
    }
}

void IMU_Sensor::reset_has_data() {
    for (bool & i : has_data) {
        i = false;
    }
}

void IMU_Sensor::age_check() {
    if (millis() - _last >= max_age) {
        get_all();
        _last = millis();
    }
}
