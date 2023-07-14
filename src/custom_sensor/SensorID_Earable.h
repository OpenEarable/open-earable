#ifndef EDGE_ML_EARABLE_SENSORID_EARABLE_H
#define EDGE_ML_EARABLE_SENSORID_EARABLE_H

#include "EdgeML_Custom.h"

const int SENSOR_COUNT = 5;
const int MODULE_COUNT_PHYSICAL = 3;
const int SPECIAL_SENSOR_COUNT = 3;

// In correct order ID ascending
enum SensorID {
    ACC_GYRO_MAG,
    BARO_TEMP,
    PDM_MIC,
    PLAYER,
    CONFIGURATION
};

const int SpecialSensors[SPECIAL_SENSOR_COUNT] = {
        PDM_MIC,
        PLAYER,
        CONFIGURATION
};

// In correct order ID ascending
enum ModuleID {
    MODULE_IMU,
    MODULE_BARO,
    MODULE_DUMMY
};

const SensorConfig CONFIG[SENSOR_COUNT] = {
        {
            "ACC_GYRO_MAG",
            ACC_GYRO_MAG,
            MODULE_IMU,
            3,
            SCHEME_XYZ,
            PARSE_TYPE_FLOAT
        },
        {
            "PRESSURE_TEMP",
            BARO_TEMP,
            MODULE_BARO,
            2,
            SCHEME_VAL,
            PARSE_TYPE_FLOAT
        },
        {
            "PDM MIC",
            PDM_MIC,
            MODULE_DUMMY,
            0,
            SCHEME_VAL,
            PARSE_TYPE_INT8
        },
        {
            "PLAYER",
            PLAYER,
            MODULE_DUMMY,
            0,
            SCHEME_VAL,
            PARSE_TYPE_INT8
        },
        {
            "Configuration",
            CONFIGURATION,
            MODULE_DUMMY,
            0,
            SCHEME_VAL,
            PARSE_TYPE_INT8
        },

};

#endif //EDGE_ML_EARABLE_SENSORID_EARABLE_H
