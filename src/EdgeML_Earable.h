#ifndef EDGE_ML_EARABLE_EDGEML_EARABLE_H
#define EDGE_ML_EARABLE_EDGEML_EARABLE_H

#include "EdgeML.h"
#include <custom_sensor/SensorManager_Earable.h>
#include <battery_service/Battery_Service.h>

#include <audio/PDM_MIC_Sensor.h>
#include <sd_logger/SD_Logger.h>

#include <utility>

class EdgeML_Earable {
public:
    EdgeML_Earable() = default;

    void begin() {
        _interface = new SensorManager_Earable();
        _battery = new Battery_Service();

        if (_debug) {
            _battery->debug(*_debug);
        }

        if (_data_logger) {
            _data_logger->begin();
        } else if (_audio_interface) {
            _interface->init();
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
            _audio_interface->set_active(edge_ml_generic.get_active());
            _audio_interface->update();
        }
    };

    void debug(Stream &stream) {
        _debug = &stream;
        edge_ml_generic.debug(stream);
    };

    // warning sd logging and audio are exlusive!
    void enable_sd_logging() {
        _data_logger = new SD_Logger();
        edge_ml_generic.set_data_callback(SD_Logger::data_callback);
    }

    void set_logger_file_name(String name) {
        if (!_data_logger) return;
        _data_logger->set_name(std::move(name));
    }

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

    void enable_chunks() {
        if (!_audio_interface) return;
        _audio_interface->enable_chunks();
    };

    void disable_chunks() {
        if (!_audio_interface) return;
        _audio_interface->disable_chunks();
    };

private:
    SensorManager_Earable * _interface{};
    Battery_Service * _battery{};
    PDM_MIC_Sensor * _audio_interface{};
    SD_Logger * _data_logger{};

    Stream * _debug{};
};

EdgeML_Earable edge_ml_earable;

#endif //EDGE_ML_EARABLE_EDGEML_EARABLE_H
