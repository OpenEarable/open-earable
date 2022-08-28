/*
Here are the configurations that are sent to the sensors.
Each sensor needs to be enabled with a configuration command containing the sensor ID, Sampling rate and a latency.
(Here latency is automatically set to 0)
*/


var SENS_minimal = [
    [0, 10],
]

var SENS_test = [
    [0, 1],
    [1, 1],
    [2, 1],
    [3, 1]
]

var SENS_operation = [
    [0, 6],                // Accelerometer corrected
    [1, 6],                // Gyroscope corrected
    [2, 6],                // Barometer
    [3, 6]                 // Temperature
]

export var EXCLUDE_DETECT = [

];

export var SENS = SENS_operation;
