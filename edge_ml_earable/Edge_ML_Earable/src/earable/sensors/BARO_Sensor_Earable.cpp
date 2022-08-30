//
// Created by Dylan Ray Roodt on 01.08.2022.
//

#include "BARO_Sensor_Earable.h"

BARO_Sensor_Earable::BARO_Sensor_Earable() {

}

void BARO_Sensor_Earable::start() {
    if (BARO.begin()) {
        available = true;
    }
}

void BARO_Sensor_Earable::end() {
    BARO.end();
    available = false;
}

float BARO_Sensor_Earable::get_pressure() {
    if (!available) {
        return 0.0;
    }

    return BARO.readPressure(PASCAL);
}

float BARO_Sensor_Earable::get_temperature(){
    if (!available) {
        return 0.0;
    }

    return BARO.readTemperature();
}
