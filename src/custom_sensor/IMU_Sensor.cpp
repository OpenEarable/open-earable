#include "IMU_Sensor.h"

extern TwoWire Wire1;

IMU_Sensor::IMU_Sensor() {
    IMU = new DFRobot_BMX160(&Wire1);
}

void IMU_Sensor::start() {
    if (available) return;
    
    Wire1.begin();
    if (IMU->begin()) {
        available = true;
    }
}

void IMU_Sensor::end() {
    if (!available) return;
    
    IMU->softReset();
    available = false;
}

void IMU_Sensor::get_data(int sensorID, byte *data) {
    float x, y, z;

    if (sensorID != ACC_GYRO_MAG) {
        memset(data, 0, 36);
        return;
    }

    get_all();

    int index = 0;
    float * floatArray = (float*)data;

    get_acc(x,y,z);
    floatArray[0+index] = x;
    floatArray[1+index] = y;
    floatArray[2+index] = z;

    index += 3;

    get_gyro(x,y,z);
    floatArray[0+index] = x;
    floatArray[1+index] = y;
    floatArray[2+index] = z;

    index += 3;

    get_mag(x,y,z);
    floatArray[0+index] = x;
    floatArray[1+index] = y;
    floatArray[2+index] = z;
}

void IMU_Sensor::get_acc(float &x, float &y, float &z) {
    if (!available) return;

    x = (float) -accel_data.x;
    y = (float) accel_data.z;
    z = (float) accel_data.y;
}

void IMU_Sensor::get_gyro(float &x, float &y, float &z) {
    if (!available) return;

    x = (float) -gyro_data.x;
    y = (float) gyro_data.z;
    z = (float) gyro_data.y;
}

void IMU_Sensor::get_mag(float &x, float &y, float &z) {
    if (!available) return;

    x = (float) -magno_data.x;
    y = (float) magno_data.z;
    z = (float) magno_data.y;
}

int IMU_Sensor::get_sensor_count() {
    return sensor_count;
}

void IMU_Sensor::get_all() {
    if (!available) return;
    
    IMU->getAllData(&magno_data, &gyro_data, &accel_data);
}
