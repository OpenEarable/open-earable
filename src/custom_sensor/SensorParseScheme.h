#ifndef OPEN_EARABLE_SENSORPARSESCHEME_H
#define OPEN_EARABLE_SENSORPARSESCHEME_H

#include <boards/generic_boards/SensorScheme.h>

// Parse schemes and types in order of sensors ascending of ID
// Scheme
const int PARSE_SCHEME_C[] = {
        PARSE_TYPE_FLOAT,       // Sensor 0
        PARSE_TYPE_FLOAT,       // Sensor 1
        PARSE_TYPE_FLOAT,       // Sensor 2
        PARSE_TYPE_FLOAT,       // Sensor 3
        PARSE_TYPE_FLOAT,       // Sensor 4
};

// Data type
const int PARSE_TYPE_C[] = {
        SCHEME_XYZ,       // Sensor 0
        SCHEME_XYZ,       // Sensor 1
        SCHEME_VAL,       // Sensor 2
        SCHEME_VAL,       // Sensor 3
        SCHEME_XYZ        // Sensor 4
};

#endif //OPEN_EARABLE_SENSORPARSESCHEME_H
