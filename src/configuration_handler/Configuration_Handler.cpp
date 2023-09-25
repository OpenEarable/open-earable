#include "Configuration_Handler.h"


const int max_config = 16;

const configuration_bundle CONFS[max_config]= {
        {30, 30, 62500},     // 1
        {30, 30, 41667},     // 2
        {30, 30, 16000},     // 3
        {30, 30, 0},         // 4

        {20, 20, 62500},     // 5
        {20, 20, 41667},     // 6
        {20, 20, 16000},     // 7
        {20, 20, 0},         // 8

        {10, 10, 62500},     // 9
        {10, 10, 41667},     // 10
        {10, 10, 16000},    // 11
        {10, 10, 0},        // 12

        {0, 0, 62500},     // 13
        {0, 0, 41667},     // 14
        {0, 0, 16000},     // 15
};

/*
const configuration_bundle CONFS[max_config]= {
        {30, 30, 62500},     // 1
        {30, 30, 41667},     // 2
        {30, 30, 0},         // 3
        {20, 20, 62500},     // 4
        {20, 20, 41667},     // 5
        {20, 20, 0},         // 6
};
*/


// NOTE! IMU_rate and BARO_rate must be compatible


bool Configuration_Handler::check_active() {
    return _config_active;
}

void Configuration_Handler::update() {
    update_edge_ml();
    BLE.poll();

    if (_buffer_flag) return;
    _buffer_flag = true;

    if (_cycle++ % 2) {
        if (update_pdm()) return;
        update_play();
    } else {
        if (update_play()) return;
        update_pdm();
    }
}

void Configuration_Handler::update_edge_ml() {
    unsigned int now = millis();
    if (now - _edge_ml_last > _edge_ml_delay) {
        _edge_ml_last = now;
        _buffer_flag = false;
        edge_ml_generic.update();
    }
}

bool Configuration_Handler::update_pdm() {
    if (pdm_mic_sensor.ready_blocks() < _pdm_min_blocks) return false;

    int cont = pdm_mic_sensor.update_contiguous(_pdm_update_blocks);
    if (check_overlap()) return true; // Make sure that time limit is not reached
    cont = _pdm_update_blocks - cont; // Compute rest: rest 0 => good; rest == total blocks => bad and return
    if (!cont || _pdm_update_blocks == cont) return false;
    pdm_mic_sensor.update_contiguous(cont);
    return check_overlap();
}

bool Configuration_Handler::update_play() {
    if (audio_player.is_mode_tone()) {
        for (int i = 0; i < _play_update_blocks; ++i) {
            audio_player.update();
        }
    }

    if (audio_player.ready_blocks() < _play_min_blocks) return false;

    int cont = audio_player.update_contiguous(_play_update_blocks);

    if (check_overlap()) return true; // Make sure that time limit is not reached
    cont = _play_update_blocks - cont; // Compute rest: rest 0 => good; rest == total blocks => bad and return
    if (!cont || _play_update_blocks == cont) return false;
    audio_player.update_contiguous(cont);
    return check_overlap();
}

void Configuration_Handler::stop_all() {
    SensorConfigurationPacket config;
    config.sampleRate = 0;
    config.latency = 0;

    int other_sensor_count = 4;

    for (int i=0; i<other_sensor_count; i++) {
        config.sensorId = i;
        edge_ml_generic.configure_sensor(config);
    }
}

void Configuration_Handler::configure(int config_num, int config_info) {
    const configuration_bundle * conf;
    SensorConfigurationPacket config;

    check_audioplayback(config_info);

    // config_num == 0 --> STOP
    // config_num > 0  --> config_num is configuration
    if (config_num < 0 || config_num >= max_config) return;

    stop_all();
    _current_conf_num = config_num;
    _cycle = 0;

    if (config_num == 0) {
        _config_active = false;
        return;
    }

    _config_active = true;
    config_num -= 1;

    conf = &CONFS[config_num];

    config.sensorId = ACC_GYRO_MAG;
    config.sampleRate = conf->IMU_rate * _rate_factor;
    edge_ml_generic.configure_sensor(config);
    edge_ml_generic.update();

    config.sensorId = BARO_TEMP;
    config.sampleRate = conf->BARO_rate * _rate_factor;
    edge_ml_generic.configure_sensor(config);
    edge_ml_generic.update();

    config.sensorId = PDM_MIC;
    config.sampleRate = float(conf->PDM_rate);
    PDM_MIC_Sensor::config_callback(&config);

    float edge_rate = max(conf->BARO_rate, conf->IMU_rate);

    if (edge_rate == 0) {
        edge_rate = _alternate_loop_rate;
    }

    _edge_ml_delay = (int)(1000.0/edge_rate);
    _edge_ml_last = millis();

    _buffer_interval_time = _edge_ml_delay - _overlap;
}

void Configuration_Handler::check_audioplayback(int config_info) {
    if (!config_info) return;
    SensorConfigurationPacket config;
    // Use latency to control player

    config.sensorId = PLAYER;
    if (config_info<=4) config_info -= 1; // adjust
    config.sampleRate = float(config_info);
    Audio_Player::config_callback(&config);

}

bool Configuration_Handler::check_overlap() {
    return (millis() - _edge_ml_last > _buffer_interval_time);
}

void Configuration_Handler::config_callback(SensorConfigurationPacket *config) {
    // Check for Configuration ID
    if (config->sensorId != CONFIGURATION) return;

    int config_num = int(config->sampleRate);
    int config_info = int(config->latency); // Additional extra info

    Serial.println("CONFIG");
    Serial.println(String(config_num));
    Serial.println(String(config_info));

    conf_handler.configure(config_num, config_info);
}



Configuration_Handler conf_handler;
