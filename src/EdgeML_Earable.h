#ifndef EDGE_ML_EARABLE_EDGEML_EARABLE_H
#define EDGE_ML_EARABLE_EDGEML_EARABLE_H

#include "EdgeML.h"
#include <custom_sensor/SensorManager_Earable.h>
#include <battery_service/Battery_Service.h>

#include <audio_pdm/PDM_Mic.h>
#include <audio_pdm/Recorder.h>
#include <audio_pdm/WavRecorder.h>

#include <audio_play/Audio_Player.h>
#include <audio_play/WavPlayer.h>
#include <audio_play/Tone.h>
#include <configuration_handler/Configuration_Handler.h>

#include <sd_logger/SD_Logger.h>

#include "button_service/Button.h"
#include "led_service/LED_Service.h"

#include <utility>

bool _data_logger_flag = true;

void data_callback(int id, unsigned int timestamp, uint8_t * data, int size);
void config_callback(SensorConfigurationPacket *config);

class EdgeML_Earable {
public:
    EdgeML_Earable() = default;

    void begin() {
        _interface = new SensorManager_Earable();
        _battery = new Battery_Service();

        edge_ml_generic.set_config_callback(config_callback);
        //edge_ml_generic.set_data_callback(data_callback);

        if (_debug) {
            _battery->debug(*_debug);
        }

        if (_data_logger_flag) {
            SD_Logger::begin();
        }

        // Can both be initialized without extra cost
        //bool success = pdm_mic_sensor.init();
        recorder.setDevice(&pdm_mic);
        bool success = recorder.begin();
        if (_debug) success ? _debug->println("PDM Ready!") : _debug->println("PDM FAIL!");

        success = audio_player.begin();
        if (_debug) success ? _debug->println("Player Ready!") : _debug->println("Player FAIL!");

        edge_ml_generic.ble_manual_advertise();
        edge_ml_generic.set_ble_config("Earable", "2.0.0");
        edge_ml_generic.set_custom(_interface);
        edge_ml_generic.begin();

        _battery->begin();
        play_service.begin();
        button_service.begin();
        led_service.begin();

        BLE.advertise();
    };

    void update() {
        _battery->update();

        conf_handler.update();

        earable_btn.update();
    };

    void debug(Stream &stream) {
        _debug = &stream;
        edge_ml_generic.debug(stream);
    };

    // SD LOGGING
    void set_sd_logging(bool enabled) {
        _data_logger_flag = enabled;
    }

    void set_logger_file_name(String name) {
        if (!_data_logger_flag) return;
        SD_Logger::set_name(std::move(name));
    }

    void set_player_file_name(String name) {
        audio_player.setSource(new WavPlayer(name));
    }

    void set_tone_freq(float frequency) {
        audio_player.setSource(new Tone(frequency));
    }

    void set_recorder_file_name(String name) {
        //pdm_mic_sensor.set_name(std::move(name));
        recorder.setTarget(new WavRecorder(name));
    }

    // Possibly not needed (done with config)
    void setSampleRate(int sampleRate) {
        recorder.setSampleRate(sampleRate);
    };

    void setGain(int gain) {
        pdm_mic.setGain(gain);
    };

    void use_serial_data_transmission(bool enabled) {
        /* TODO: replace
        */
        //if (enabled) pdm_mic_sensor.enable_serial_data();
        //else pdm_mic_sensor.disable_serial_data();
    }

    void configure_sensor(SensorConfigurationPacket& config) {
        edge_ml_generic.configure_sensor(config);
    };

    String parse_to_string(int sensorID, byte * data) {
        return edge_ml_generic.parse_to_string(sensorID, data);
    }

private:
    SensorManager_Earable * _interface{};   // Created new
    Battery_Service * _battery{};           // Created new

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
    Recorder::config_callback(config);
    Audio_Player::config_callback(config);
    Configuration_Handler::config_callback(config);
}



#endif //EDGE_ML_EARABLE_EDGEML_EARABLE_H
