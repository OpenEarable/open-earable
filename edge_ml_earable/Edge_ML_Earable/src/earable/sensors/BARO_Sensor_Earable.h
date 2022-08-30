//
// Created by Dylan Ray Roodt on 01.08.2022.
//

#ifndef EDGE_ML_EARABLE_BARO_SENSOR_EARABLE_H
#define EDGE_ML_EARABLE_BARO_SENSOR_EARABLE_H

#include "Baro.h"

class BARO_Sensor_Earable {
public:
    BARO_Sensor_Earable();
    void start();
    void end();

    float get_pressure();
    float get_temperature();

    const int sensor_count = 2;

private:
    bool available = false;
};


#endif //EDGE_ML_EARABLE_BARO_SENSOR_EARABLE_H
