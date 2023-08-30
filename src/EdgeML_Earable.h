#ifndef EDGE_ML_EARABLE_EDGEML_EARABLE_H
#define EDGE_ML_EARABLE_EDGEML_EARABLE_H

#include "EdgeML.h"
#include <custom_sensor/SensorManager_Earable.h>
#include <battery_service/Battery_Service.h>

#include <audio_pdm/PDM_MIC_Sensor.h>
#include <audio_play/Audio_Player.h>
#include <configuration_handler/Configuration_Handler.h>

#include <sd_logger/SD_Logger.h>

#include "button_service/Button.h"

#include <utility>

bool _data_logger_flag = false;

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
        }

        // Can both be initialized without extra cost
        bool success = pdm_mic_sensor.init();
        if (_debug) success ? _debug->println("PDM Ready!") : _debug->println("PDM FAIL!");

        success = audio_player.init();
        if (_debug) success ? _debug->println("Player Ready!") : _debug->println("Player FAIL!");

        _battery->begin();
        button_service.begin();

        edge_ml_generic.set_ble_config("Earable", "2.0.0");
        edge_ml_generic.set_custom(_interface);
        edge_ml_generic.begin();
    };

    void update() {
        _battery->update();

        if (conf_handler.check_active()) {
            conf_handler.update();
        } else {
            // Possibly rewrite
            edge_ml_generic.update();

            // Auto return if not active
            pdm_mic_sensor.update();
            audio_player.update();
        }

        earable_btn.update();
    };

    void debug(Stream &stream) {
        _debug = &stream;
        edge_ml_generic.debug(stream);
    };

    // SD LOGGING
    void enable_sd_logging() {
        _data_logger = new SD_Logger();
        _data_logger_flag = true;
    }

    void set_logger_file_name(String name) {
        if (!_data_logger) return;
        _data_logger->set_name(std::move(name));
    }

    void set_playerfile_prefix(String name) {
        audio_player.set_name(std::move(name));
    }

    void set_recorder_file_name(String name) {
        pdm_mic_sensor.set_name(std::move(name));
    }

    // Possibly not needed (done with config)
    void setSampleRate(int sampleRate) {
        pdm_mic_sensor.setSampleRate(sampleRate);
    };

    void setGain(int gain) {
        pdm_mic_sensor.setGain(gain);
    };

    void enable_serial_data() {
        pdm_mic_sensor.enable_serial_data();
    };

    void disable_serial_data() {
        pdm_mic_sensor.disable_serial_data();
    };

    void configure_sensor(SensorConfigurationPacket& config) {
        edge_ml_generic.configure_sensor(config);
    };

    String parse_to_string(int sensorID, byte * data) {
        return edge_ml_generic.parse_to_string(sensorID, data);
    }

private:
    SensorManager_Earable * _interface{};   // Created new
    Battery_Service * _battery{};           // Created new
    SD_Logger * _data_logger{};             // Created new on demand

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
    PDM_MIC_Sensor::config_callback(config);
    Audio_Player::config_callback(config);
    Configuration_Handler::config_callback(config);
}



#endif //EDGE_ML_EARABLE_EDGEML_EARABLE_H
