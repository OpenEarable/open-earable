#ifndef EDGE_ML_EARABLE_SENSORMANAGER_EARABLE_H
#define EDGE_ML_EARABLE_SENSORMANAGER_EARABLE_H

#include "SensorID_Earable.h"
#include "IMU_Sensor.h"
#include "BARO_Sensor.h"

class SensorManager_Earable : public SensorManagerInterface {
public:
    void setup() override {
        IMU_Sensor * imu = new IMU_Sensor();
        BARO_Sensor * baro = new BARO_Sensor();

        SensorInterface ** modules = new SensorInterface * [MODULE_COUNT_PHYSICAL] {imu, baro, _audio_interface};

        int audio_disabled = 0;
        if (!_audio_interface) {
            audio_disabled = -1;
        }

        SensorManagerInterface::set_modules(modules);
        SensorManagerInterface::set_sensor_counts(SENSOR_COUNT+audio_disabled, MODULE_COUNT_PHYSICAL+audio_disabled);

        SensorManagerInterface::set_sensor_configs(CONFIG);
    }

    void set_audio_interface(SensorInterface * interface) {
        _audio_interface = interface;
    }

private:
    SensorInterface * _audio_interface = nullptr;
};


#endif //EDGE_ML_EARABLE_SENSORMANAGER_EARABLE_H
