#ifndef EDGE_ML_EARABLE_IMU_SENSOR_H
#define EDGE_ML_EARABLE_IMU_SENSOR_H

#include <DFRobot_BMX160.h>
#include <Wire.h>

#include "EdgeML_Custom.h"
#include "SensorID_Earable.h"

class IMU_Sensor : public SensorInterface {
public:
    IMU_Sensor();

    void start() override;
    void end() override;

    void get_data(int ID, byte * data) override;

    int get_sensor_count() override;

    static const int sensor_count = 3;

    void get_acc(float& x, float& y, float& z);
    void get_gyro(float& x, float& y, float& z);
    void get_mag(float& x, float& y, float& z);
private:
    bool available = false;

    DFRobot_BMX160 * IMU;

    sBmx160SensorData_t accel_data;
    sBmx160SensorData_t gyro_data;
    sBmx160SensorData_t magno_data;

    void get_all();
};


#endif //EDGE_ML_EARABLE_IMU_SENSOR_H
