#include "SensorManager_Earable.h"

BARO_Sensor_Earable baro_sensor_earable;
IMU_Sensor_Earable imu_sensor_earable;



SensorManager_Earable::SensorManager_Earable() {
    ModuleActive act{};
    for (int i=0; i < MODULE_COUNT_PHYSICAL; i++) {
        act = active[i];

        for (int u=0; u < ModuleActive_BUF; u++) {
            act.active[u] = false;
        }

        act.module = i;

        switch (i) {
            case MODULE_IMU : {
                act.count = imu_sensor_earable.sensor_count;
                init_act_map(act, IMU_MAP);
                init_sensor_module_pos(act.count, IMU_MAP);
                break;
            }
            default: {
                break;
            }
        }
    }
}

void SensorManager_Earable::start_sensor(int ID) {
    // Start sensor
    if (_debug) {
        _debug->print("Starting Sensor: ");
        _debug->println(ID);
    }
    int pos = sensor_module_pos[ID];
    int module = module_assignment[ID];

    switch(ID) {
        case IMU_ACCELERATION : {
            activate_pos(module, pos);
            imu_sensor_earable.start();
            break;
        }
        case IMU_GYROSCOPE : {
            activate_pos(module, pos);
            imu_sensor_earable.start();
            break;
        }
        case IMU_MAGNETOMETER : {
            activate_pos(module, pos);
            imu_sensor_earable.start();
            break;
        }
        case BARO_PRESSURE : {
            activate_pos(module, pos);
            baro_sensor_earable.start();
            break;
        }
        case BARO_TEMP : {
            activate_pos(module, pos);
            baro_sensor_earable.start();
            break;
        }
        default:
            break;
    }
    if (_debug) {
        _debug->println("Sensor start success");
    }
}

void SensorManager_Earable::end_sensor(int ID) {
    // End sensor
    int pos = sensor_module_pos[ID];
    int module = module_assignment[ID];

    switch(ID) {
        case IMU_ACCELERATION : {
            deactivate_pos(module, pos);
            if (all_inactive(module, pos)) {
                imu_sensor_earable.end();
            }
            break;
        }
        case IMU_GYROSCOPE : {
            deactivate_pos(module, pos);
            if (all_inactive(module, pos)) {
                imu_sensor_earable.end();
            }
            break;
        }
        case IMU_MAGNETOMETER : {
            deactivate_pos(module, pos);
            if (all_inactive(module, pos)) {
                imu_sensor_earable.end();
            }
            break;
        }
        case BARO_PRESSURE : {
            deactivate_pos(module, pos);
            if (all_inactive(module, pos)) {
                baro_sensor_earable.end();
            }
            break;
        }
        case BARO_TEMP : {
            deactivate_pos(module, pos);
            if (all_inactive(module, pos)) {
                baro_sensor_earable.end();
            }
            break;
        }
        default:
            break;
    }
}

int * SensorManager_Earable::get_int_data(int ID) {
    int* data = new int[4];

    switch(ID) {
        // No Sensors
    }
    return data;
}

float * SensorManager_Earable::get_float_data(int ID){
    float* data = new float[4];

    switch(ID) {
        case IMU_ACCELERATION : {
            data[0] = 3;
            float x,y,z;
            imu_sensor_earable.get_acc(x, y, z);
            data[1] = x;
            data[2] = y;
            data[3] = z;
            break;
        }
        case IMU_GYROSCOPE : {
            data[0] = 3;
            float x,y,z;
            imu_sensor_earable.get_gyro(x, y, z);
            data[1] = x;
            data[2] = y;
            data[3] = z;
            break;
        }
        case IMU_MAGNETOMETER: {
            data[0] = 3;
            float x,y,z;
            imu_sensor_earable.get_magno(x, y, z);
            data[1] = x;
            data[2] = y;
            data[3] = z;
            break;
        }
        case BARO_PRESSURE : {
            data[0] = 1;
            float pressure;
            pressure = baro_sensor_earable.get_pressure();
            data[1] = pressure;
            break;
        }
        case BARO_TEMP : {
            data[0] = 1;
            float temperature;
            temperature = baro_sensor_earable.get_temperature();
            data[1] = temperature;
            break;
        }
        default:
            break;
    }

    return data;
}

void SensorManager_Earable::init_act_map(ModuleActive& act, const int *MAP) {
    for (int i=0; i < act.count; i++) {
        act.map[i] = MAP[i];
        init_assignment(MAP[i], act.module);
    }
}

void SensorManager_Earable::init_assignment(const int ID, int MODULE) {
    module_assignment[ID] = MODULE;
}

void SensorManager_Earable::init_sensor_module_pos(int length, const int *MAP) {
    for (int i=0; i<length; i++) {
        sensor_module_pos[MAP[i]] = i;
    }
}

void SensorManager_Earable::activate_pos(int module, int pos) {
    ModuleActive act;
    act = active[module];
    act.active[pos] = true;
}

void SensorManager_Earable::deactivate_pos(int module, int pos) {
    ModuleActive act;
    act = active[module];
    act.active[pos] = false;
}

bool SensorManager_Earable::all_inactive(int module, int pos) {
    ModuleActive act = active[module];
    for (int i=0; i < act.count; i++) {
        if (act.active[pos]) {
            return false;
        }
    }
    return true;
}

void SensorManager_Earable::debug(Stream &stream)
{
    _debug = &stream;
}
