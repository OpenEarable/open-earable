#include "BARO_Sensor.h"

extern TwoWire Wire1;

BARO_Sensor::BARO_Sensor() {
    _sensors_ids = BARO_MAP;
    _module_id = MODULE_BARO;

    Baro = new Adafruit_BMP280(&Wire1);

    for (bool & status : _active) {
        status = false;
    }
}

void BARO_Sensor::start() {
    if (available) {
        return;
    }

    Wire1.begin(); // Call if neccessary

    if (Baro->begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
        available = true;
        Baro->setSampling(Adafruit_BMP280::MODE_NORMAL,
                          Adafruit_BMP280::SAMPLING_X2,
                          Adafruit_BMP280::SAMPLING_X1,
                          Adafruit_BMP280::FILTER_OFF,
                          Adafruit_BMP280::STANDBY_MS_1);
    }
}

void BARO_Sensor::end() {
    if (!available) {
        return;
    }
    available = false;
    Baro->reset();
}

void BARO_Sensor::get_float_data(float *floatArray, int sensorID) {
    float val;
    switch (sensorID) {
        case BARO_PRESSURE:
            get_pressure(val);
            break;
        case BARO_TEMP:
            get_temperature(val);
            break;
        default:
            break;
    }

    floatArray[0] = 1; // 1 Values
    floatArray[1] = val;
}

void BARO_Sensor::get_int_data(int *intArray, int sensorID) {
    // Not implemented since no int data
}

int BARO_Sensor::get_sensor_count() {
    return sensor_count;
}

void BARO_Sensor::get_pressure(float &value) {
    if (!available) {
        value = 0.0;
        return;
    }

    value = Baro->readPressure();
}

void BARO_Sensor::get_temperature(float &value) {
    if (!available) {
        value = 0.0;
        return;
    }
    value = Baro->readTemperature();
}
