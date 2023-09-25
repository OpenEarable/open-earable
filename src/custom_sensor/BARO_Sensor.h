#ifndef EDGE_ML_EARABLE_BARO_SENSOR_H
#define EDGE_ML_EARABLE_BARO_SENSOR_H

#include <Adafruit_BMP280.h>
#include <Wire.h>

#include "EdgeML_Custom.h"
#include "SensorID_Earable.h"

class BARO_Sensor : public SensorInterface {
public:
    BARO_Sensor();

    void start() override;
    void end() override;

    void get_data(int ID, byte * data) override;

    int get_sensor_count() override;

    const int sensor_count = 2;

    void get_pressure(float &value);
    void get_temperature(float &value);
private:
    bool available = false;

    Adafruit_BMP280 *Baro;
};


#endif //EDGE_ML_EARABLE_BARO_SENSOR_H
