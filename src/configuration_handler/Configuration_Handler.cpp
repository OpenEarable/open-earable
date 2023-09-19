#include "Configuration_Handler.h"


const int max_config = 16;

const configuration_bundle CONFS[max_config]= {
        {50, 50, 62500},     // 1
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

    const int min_update = 2;
    const float STD = 0.5;

    /*Serial.print("Play: ");
    Serial.print((*audio_player.source->stream)->remaining());
    Serial.print(" PDM: ");
    Serial.println((*recorder.target->stream)->remaining());*/

    Provider * provider;
    
    while (!check_overlap() && max((*recorder.target->stream)->ready(), (*audio_player.source->stream)->ready()) >= min_update) {
        // check priority
        float diff = (*recorder.target->stream)->remaining() - (*audio_player.source->stream)->remaining() * 1.5;
        //float mean = (pdm_mic_sensor.remaining_blocks() + audio_player.remaining_blocks()) / 2.0;
        int blocks = abs(diff) + min_update / 2; //max(mean * STD, min_update / 2); //min_update / 2;
        //unsigned long t1 = millis();

        if (diff > 0) {
            provider = audio_player.source;
        } else {
            provider = recorder.target;
        }
        update(provider, min(blocks,(*provider->stream)->ready()-1));
    }

    long now = millis();

    Serial.println(now - (long)_edge_ml_last - (long)_edge_ml_delay);
}

void Configuration_Handler::update_edge_ml() {
    unsigned int now = millis();
    if (now - _edge_ml_last >= _edge_ml_delay) {
        //Serial.print("delay: ");
        //Serial.println(now - _edge_ml_last - _edge_ml_delay);
        _edge_ml_last = now;
        _buffer_flag = false;
        //now = millis();
        edge_ml_generic.update();
        //Serial.print(" edge_ml time: ");
        //Serial.println(millis() - now);
    }
}

bool Configuration_Handler::update(Provider * provider, int max_buffers) {
    if (max_buffers <= 0 || (*provider->stream)->ready() == 0) return false;

   int cont = provider->provide(max_buffers);

    if (check_overlap()) return true; // Make sure that time limit is not reached
    cont = max_buffers - cont; // Compute rest: rest 0 => good; rest == total blocks => bad and return
    if (!cont || max_buffers == cont) return false;
    if (check_overlap()) return true; // Make sure that time limit is not reached
    cont = provider->provide(cont);
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
    Recorder::config_callback(&config);

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
