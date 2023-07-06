#ifndef EDGE_ML_EARABLE_SENSORID_EARABLE_H
#define EDGE_ML_EARABLE_SENSORID_EARABLE_H

#include "EdgeML_Custom.h"

const int SENSOR_COUNT = 6;
const int MODULE_COUNT_PHYSICAL = 3;
const int SPECIAL_SENSOR_COUNT = 1;

// In correct order ID ascending
enum SensorID {
    IMU_ACCELERATION,
    IMU_GYROSCOPE,
    BARO_PRESSURE,
    BARO_TEMP,
    IMU_MAGNETOMETER,
    PDM_MIC
};

const int SpecialSensors[SPECIAL_SENSOR_COUNT] = {
        PDM_MIC
};

// In correct order ID ascending
enum ModuleID {
    MODULE_IMU,
    MODULE_BARO,
    MODULE_PDM_MIC
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
        },
        {
            "PDM MIC",
            PDM_MIC,
            MODULE_PDM_MIC,
            R_TYPE_INT,
            SCHEME_VAL,
            PARSE_TYPE_INT16
        }
};

#endif //EDGE_ML_EARABLE_SENSORID_EARABLE_H
