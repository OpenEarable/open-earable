#ifndef EDGE_ML_EARABLE_SENSORMANAGER_EARABLE_H
#define EDGE_ML_EARABLE_SENSORMANAGER_EARABLE_H

#include <custom_sensor/SensorID_Earable.h>



#include "IMU_Sensor.h"
#include "BARO_Sensor.h"

class SensorManager_Earable : public SensorManagerInterface {
public:
    void setup() override {
        IMU_Sensor * imu = new IMU_Sensor();
        BARO_Sensor * baro = new BARO_Sensor();

        SensorInterface ** sensors = new SensorInterface * [MODULE_COUNT_PHYSICAL] {imu, baro};

        SensorManagerInterface::set_sensors(sensors);
        SensorManagerInterface::set_sensor_counts(SENSOR_COUNT, MODULE_COUNT_PHYSICAL);

        SensorManagerInterface::set_type_float(RETURN_TYPE_FLOAT, FLOAT_TYPE_COUNT);

        SensorManagerInterface::init();
    }
};


#endif //EDGE_ML_EARABLE_SENSORMANAGER_EARABLE_H
