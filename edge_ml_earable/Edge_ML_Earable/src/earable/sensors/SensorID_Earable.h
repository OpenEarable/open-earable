//
// Created by Dylan Ray Roodt on 23.05.2022.
//
#include "Arduino.h"

/*
IMU:
 - Acceleration
 - Gyroscope
 */

#ifndef SENSORID_EARABLE_H
#define SENSORID_EARABLE_H

const int IMU_ACCELERATION = 0;
const int IMU_GYROSCOPE = 1;
const int IMU_MAGNETOMETER = 4;

const int BARO_PRESSURE = 2;
const int BARO_TEMP = 3;


const int SENSOR_COUNT = 5;
const int MODULE_COUNT_PHYSICAL = 2;

const int MODULE_IMU = 0;
const int MODULE_BARO = 1;

const int IMU_MAP[] = {IMU_ACCELERATION, IMU_GYROSCOPE, IMU_MAGNETOMETER};
const int BARO_MAP[] = {BARO_PRESSURE, BARO_TEMP};

const int TYPE_INT = 0;
const int TYPE_FLOAT = 1;

const int INT_TYPE_COUNT = 0;
const int FLOAT_TYPE_COUNT = 5;
const int RETURN_TYPE_INT[] = {};
const int RETURN_TYPE_FLOAT[] = {IMU_ACCELERATION, IMU_GYROSCOPE, IMU_MAGNETOMETER, BARO_PRESSURE, BARO_TEMP};

#endif //SENSORID_EARABLE_H
