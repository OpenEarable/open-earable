#ifndef EDGE_ML_EARABLE_SENSORID_EARABLE_H
#define EDGE_ML_EARABLE_SENSORID_EARABLE_H

#include "EdgeML_Custom.h"

const int SENSOR_COUNT = 5;
const int MODULE_COUNT_PHYSICAL = 2;

// In correct order ID ascending
enum SensorID {
    IMU_ACCELERATION,
    IMU_GYROSCOPE,
    BARO_PRESSURE,
    BARO_TEMP,
    IMU_MAGNETOMETER
};

// In correct order ID ascending
enum ModuleID {
    MODULE_IMU,
    MODULE_BARO
};

const SensorConfig CONFIG[SENSOR_COUNT] = {
        {
            "ACC",
            IMU_ACCELERATION,
            MODULE_IMU,
            R_TYPE_FLOAT,
            SCHEME_XYZ,
            PARSE_TYPE_FLOAT
        },
        {
            "GYRO",
            IMU_GYROSCOPE,
            MODULE_IMU,
            R_TYPE_FLOAT,
            SCHEME_XYZ,
            PARSE_TYPE_FLOAT
        },
        {
            "MAG",
            IMU_MAGNETOMETER,
            MODULE_IMU,
            R_TYPE_FLOAT,
            SCHEME_XYZ,
            PARSE_TYPE_FLOAT
        },
        {
            "PRESSURE",
            BARO_PRESSURE,
            MODULE_BARO,
            R_TYPE_FLOAT,
            SCHEME_VAL,
            PARSE_TYPE_FLOAT
        },
        {
            "TEMPERATURE",
            BARO_TEMP,
            MODULE_BARO,
            R_TYPE_FLOAT,
            SCHEME_VAL,
            PARSE_TYPE_FLOAT
        }
};

#endif //EDGE_ML_EARABLE_SENSORID_EARABLE_H
