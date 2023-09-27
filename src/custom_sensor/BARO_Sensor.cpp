#include "BARO_Sensor.h"

extern TwoWire Wire1;

BARO_Sensor::BARO_Sensor() {
    Baro = new Adafruit_BMP280(&Wire1);
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

void BARO_Sensor::get_data(int sensorID, byte *data) {
    float val;

    if (sensorID != BARO_TEMP) {
        memset(data, 0, 8);
        return;
    }

    /*float * floatArray = (float*)data;
    get_pressure(val);
    floatArray[0] = val;

    get_temperature(val);
    floatArray[1] = val;*/

    float * floatArray = (float*)data;
    get_pressure_and_temperature(floatArray);
    /*get_pressure(val);
    floatArray[0] = val;

    get_temperature(val);
    floatArray[1] = val;*/
}

int BARO_Sensor::get_sensor_count() {
    return sensor_count;
}

void BARO_Sensor::get_pressure_and_temperature(float * value) {
    if (!available) {
        value[0] = 0;
        value[1] = 0;
        return;
    }

    value[0] = Baro->getTemperature();
    value[1] = Baro->readPressure();
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
