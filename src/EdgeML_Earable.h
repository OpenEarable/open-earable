#ifndef EDGE_ML_EARABLE_EDGEML_EARABLE_H
#define EDGE_ML_EARABLE_EDGEML_EARABLE_H

#include "EdgeML.h"
#include <custom_sensor/SensorManager_Earable.h>
#include <battery_service/Battery_Service.h>

#include <audio_pdm//PDM_MIC_Sensor.h>
#include <sd_logger/SD_Logger.h>

#include <utility>

/*
 Typical Loop: 0ms
 Rarely up to 2ms
 */

bool _data_logger_flag = false;
bool _recorder_flag = false;
bool _player_flag = false;

void data_callback(int id, unsigned int timestamp, uint8_t * data, int size);
void config_callback(SensorConfigurationPacket *config);

class EdgeML_Earable {
public:
    EdgeML_Earable() = default;

    void begin() {
        _interface = new SensorManager_Earable();
        _battery = new Battery_Service();

        edge_ml_generic.set_config_callback(config_callback);
        edge_ml_generic.set_data_callback(data_callback);

        if (_debug) {
            _battery->debug(*_debug);
        }

        if (_data_logger) {
            _data_logger->begin();
        } else if (_audio_interface) {
            bool success = _audio_interface->init();
            if (_debug) {
                if (success) {
                    _debug->println("SD Ready!");
                } else {
                    _debug->println("SD FAIL!");
                }
            }
        }

        _battery->begin();

        edge_ml_generic.set_ble_config("Earable", "2.0.0");
        edge_ml_generic.set_custom(_interface);
        edge_ml_generic.begin();
    };

    void update() {
        unsigned int T1, T2, T3, T4;
        unsigned int DT1, DT2, DT3, DTT;

        T1 = millis();

        _battery->update();
        T2 = millis();
        edge_ml_generic.update();
        T3 = millis();

        if (_audio_interface) {
            _audio_interface->update();
        }

        T4 = millis();

        DTT = T4 - T1;
        DT1 = T2 - T1;
        DT2 = T3 - T2;
        DT3 = T4 - T3;
        int limit = 3;
        if (DTT <= limit) return;

        Serial.println("T: " + String(DTT) + "   D1: " + String(DT1) +"   D2: " + String(DT2) +"   D3: " + String(DT3));
    };

    void debug(Stream &stream) {
        _debug = &stream;
        edge_ml_generic.debug(stream);
    };

    // SD LOGGING
    // warning sd logging and audio_pdm are exlusive!
    void enable_sd_logging() {
        _data_logger = new SD_Logger();
        _data_logger_flag = true;
    }

    void set_logger_file_name(String name) {
        if (!_data_logger) return;
        _data_logger->set_name(std::move(name));
    }

    // AUDIO RECORDING
    void enable_audio() {
        _audio_interface = &pdm_mic_sensor;
        _recorder_flag = true;
    }

    void set_audio_file_name(String name) {
        if (!_audio_interface) return;
        _audio_interface->set_name(std::move(name));
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

    void configure_sensor(SensorConfigurationPacket& config) {
        edge_ml_generic.configure_sensor(config);
    };

    String parse_to_string(int sensorID, byte * data) {
        return edge_ml_generic.parse_to_string(sensorID, data);
    }

private:
    SensorManager_Earable * _interface{};
    Battery_Service * _battery{};
    PDM_MIC_Sensor * _audio_interface{};
    SD_Logger * _data_logger{};

    Stream * _debug{};
};

EdgeML_Earable edge_ml_earable;

void data_callback(int id, unsigned int timestamp, uint8_t * data, int size) {
    if (_data_logger_flag) {
        String data_string = edge_ml_earable.parse_to_string(id, data);
        SD_Logger::data_callback(id, timestamp, data_string);
    }
}

void config_callback(SensorConfigurationPacket *config) {
    if (_recorder_flag) {
        PDM_MIC_Sensor::config_callback(config);
    }
    if (_player_flag) {

    }
}



#endif //EDGE_ML_EARABLE_EDGEML_EARABLE_H
