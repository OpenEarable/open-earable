//
// Created by Tobias King on 26.10.2022.
//

#ifndef IMU_SENSOR_EARABLE_H
#define IMU_SENSOR_EARABLE_H

#include <DFRobot_BMX160.h>
#include <Wire.h>

class IMU_Sensor_Earable {
public:
    IMU_Sensor_Earable();
    void start();
    void end();

    void get_acc(float& x, float& y, float& z);
    void get_gyro(float& x, float& y, float& z);
    void get_magno(float& x, float& y, float& z);

    const int sensor_count = 3;

private:
    bool available = false;
    DFRobot_BMX160 * IMU;

    sBmx160SensorData_t accel_data;
    sBmx160SensorData_t gyro_data;
    sBmx160SensorData_t magno_data;

};

#endif //IMU_SENSOR_EARABLE_H
