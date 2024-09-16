#include "PDM_Sensor.h"
#include "../audio_pdm/BLEStream.h"

PDM_Sensor::PDM_Sensor() {

}

void PDM_Sensor::start() {
    if (available) return;
    
    /*Wire1.begin();
    if (IMU->begin()) {
        available = true;
    }*/
   available = true;
}

void PDM_Sensor::end() {
    if (!available) return;
    
    /*IMU->softReset();
    available = false;*/
    available = false;
}

void PDM_Sensor::get_data(int sensorID, byte *data) {
    if (sensorID != PDM_MIC) {
        memset(data, 0, sizeof(int16_t) * 16); /////??????
        return;
    }

    memcpy(data, _buffer, sizeof(int16_t) * 16);
}

void PDM_Sensor::setBuffer(uint8_t * buffer) {
    _buffer = buffer;
}

int PDM_Sensor::get_sensor_count() {
    return sensor_count;
}

PDM_Sensor pdm_sensor;