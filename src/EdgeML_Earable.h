#ifndef EDGE_ML_EARABLE_EDGEML_EARABLE_H
#define EDGE_ML_EARABLE_EDGEML_EARABLE_H

#include "EdgeML.h"
#include <custom_sensor/SensorManager_Earable.h>
#include <battery_service/Battery_Service.h>

#include <audio/PDM_MIC_Sensor.h>

class EdgeML_Earable {
public:
    EdgeML_Earable() = default;

    void begin() {
        _interface = new SensorManager_Earable();
        _battery = new Battery_Service();

        if (_audio_interface) {
            _interface->set_audio_interface(_audio_interface);
        }

        _battery->begin();

        edge_ml_generic.set_ble_config("Earable", "2.0.0");
        edge_ml_generic.set_custom(_interface);
        edge_ml_generic.begin();
    };

    void update() {
        _battery->update();
        edge_ml_generic.update();

        if (_audio_interface) {
            _audio_interface->update();
        }
    };

    void debug(Stream &stream) {
        _battery->debug(stream);
        edge_ml_generic.debug(stream);
    };

    void enable_audio() {
        _audio_interface = new PDM_MIC_Sensor();
    }

    void setSampleRate(int sampleRate) {
        if (!_audio_interface) return;
        _audio_interface->setSampleRate(sampleRate);
    };

    void setGain(int gain) {
        if (!_audio_interface) return;
        _audio_interface->setGain(gain);
    };

    void enable_serial_data() {
        if (!_audio_interface) return;
        _audio_interface->enable_serial_data();
    };

    void disable_serial_data() {
        if (!_audio_interface) return;
        _audio_interface->disable_serial_data();
    };
private:
    SensorManager_Earable * _interface{};
    Battery_Service * _battery{};
    PDM_MIC_Sensor * _audio_interface{};
};

EdgeML_Earable edge_ml_earable;

#endif //EDGE_ML_EARABLE_EDGEML_EARABLE_H
