#include "Configuration_Handler.h"

 const int max_config = 1;
configuration_bundle CONFS[max_config]= {
{20, 20, 62500, true}
};

// NOTE! IMU_rate and BARO_rate must be compatible


bool Configuration_Handler::check_active() {
    return _config_active;
}

void Configuration_Handler::update() {
    update_edge_ml();

    bool overlapped;
    if (_cycle++ % 2) {
        overlapped = update_pdm();
        if (overlapped) {
            if (!check_overlap_prepopen()) audio_player.pre_open_file();
            return;
        }
        update_play();
    } else {
        overlapped = update_play();
        if (overlapped) {
            if (!check_overlap_prepopen()) pdm_mic_sensor.pre_open_file();
            return;
        }
        update_pdm();
    }
}

void Configuration_Handler::update_edge_ml() {
    unsigned int now = millis();
    if (now - _edge_ml_last > _edge_ml_delay) {
        _edge_ml_last = now;
        edge_ml_generic.update();
    }
}

bool Configuration_Handler::update_pdm() {
    for (int i=0; i<_buffer_loops; i++) {
        if (check_overlap()) return true;
        pdm_mic_sensor.update();
    }
    return false;
}

bool Configuration_Handler::update_play() {
    for (int i=0; i<_buffer_loops; i++) {
        if (check_overlap()) return true;
        audio_player.update();
    }
    return false;
}

void Configuration_Handler::stop_all() {
    SensorConfigurationPacket config;
    config.sampleRate = 0;

    int other_sensor_count = 4;

    for (int i=0; i<other_sensor_count; i++) {
        config.sensorId = i;
        edge_ml_generic.configure_sensor(config);
    }
}

void Configuration_Handler::configure(int config_num) {
    configuration_bundle * conf;
    SensorConfigurationPacket config;

    // config_num == 0 --> STOP; config_num > 0  --> config_num - 1 is index

    if (config_num < 0 || config_num > max_config + 1) return;

    stop_all();
    _current_conf_num = config_num;
    _cycle = 0;

    if (config_num == 0) {
        _config_active = false;
        return;
    }

    _config_active = true;

    conf = &CONFS[config_num-1];

    config.sensorId = ACC_GYRO_MAG;
    config.sampleRate = conf->IMU_rate * _rate_factor;
    edge_ml_generic.configure_sensor(config);



    config.sensorId = BARO_TEMP;
    config.sampleRate = conf->BARO_rate * _rate_factor;
    edge_ml_generic.configure_sensor(config);



    config.sensorId = PDM_MIC;
    config.sampleRate = float(conf->PDM_rate);
    PDM_MIC_Sensor::config_callback(&config);

    config.sensorId = PLAYER;
    config.sampleRate = float(conf->Play);
    PDM_MIC_Sensor::config_callback(&config);

    float edge_rate = max(conf->BARO_rate, conf->IMU_rate);
    _edge_ml_delay = (int)(1000.0/edge_rate);
    _edge_ml_last = millis();

    _buffer_interval_time = _edge_ml_delay - _overlap;
    _overlap_prepopen = _edge_ml_delay - _overlap_prepopen;
}


bool Configuration_Handler::check_overlap() {
    return (millis() - _edge_ml_last > _buffer_interval_time);
}

bool Configuration_Handler::check_overlap_prepopen() {
    return (millis() - _edge_ml_last > _buffer_interval_time_prepopen);;
}

void Configuration_Handler::config_callback(SensorConfigurationPacket *config) {
    // Check for Configuration ID
    if (config->sensorId != CONFIGURATION) return;

    int config_num = int(config->sampleRate);

    conf_handler.configure(config_num);
}

Configuration_Handler conf_handler;


