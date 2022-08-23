//
// Created by Dylan Ray Roodt on 23.05.2022.
//

#ifndef SENSORMANAGER_EARABLE_H
#define SENSORMANAGER_EARABLE_H

#include "Arduino.h"

#include "SensorTypes_Earable.h"
#include "SensorID_Earable.h"
#include "IMU_Sensor_Earable.h"
#include "BARO_Sensor_Earable.h"

class SensorManager_Earable {
public:
    SensorManager_Earable();

    void start_sensor(int ID);
    void end_sensor(int ID);

    int * get_int_data(int ID);
    float * get_float_data(int ID);

    // Add additional methods

    void debug(Stream &stream);

private:
    Stream *_debug{};
    ModuleActive active[MODULE_COUNT_PHYSICAL]{};
    int module_assignment[SENSOR_COUNT]{};
    int sensor_module_pos[SENSOR_COUNT]{};

    void init_act_map(ModuleActive& act, const int *MAP);
    void init_assignment(const int ID, int MODULE);
    void init_sensor_module_pos(int length, const int *MAP);
    void activate_pos(int module, int pos);
    void deactivate_pos(int module, int pos);
    bool all_inactive(int module, int pos);
};

#endif //SENSORMANAGER_EARABLE_H
