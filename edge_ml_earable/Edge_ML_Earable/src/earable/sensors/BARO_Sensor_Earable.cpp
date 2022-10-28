//
// Created by Tobias King on 26.10.2022.
//

#include "BARO_Sensor_Earable.h"

extern TwoWire Wire1;

BARO_Sensor_Earable::BARO_Sensor_Earable() {
    Baro = new Adafruit_BMP280(&Wire1);
}

void BARO_Sensor_Earable::start() {
    if (Baro->begin(BMP280_BARO)) {
        available = true;
        Baro->setSampling(Adafruit_BMP280::MODE_NORMAL,
                Adafruit_BMP280::SAMPLING_X2,
                Adafruit_BMP280::SAMPLING_X1,
                Adafruit_BMP280::FILTER_OFF,
                Adafruit_BMP280::STANDBY_MS_1);
    }
}

void BARO_Sensor_Earable::end() {
    available = false;
}

float BARO_Sensor_Earable::get_pressure() {
    if (!available) {
        return 0.0;
    }

    return Baro->readPressure();
}

float BARO_Sensor_Earable::get_temperature(){
    if (!available) {
        return 0.0;
    }
    return Baro->readTemperature();
}
