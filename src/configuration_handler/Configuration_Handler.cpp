#include "Configuration_Handler.h"

 const int max_config = 6;
configuration_bundle CONFS[max_config]= {
{20, 20, 62500, 1},     // 1
{20, 20, 41667, 1},     // 2
{0, 0, 41667, 1},       // 3
{0, 0, 62500, 1},       // 4
{0, 0, 0, 1},           // 5
{0, 0, 62500, 0}        // 6
};

// NOTE! IMU_rate and BARO_rate must be compatible


bool Configuration_Handler::check_active() {
    return _config_active;
}

void Configuration_Handler::update() {
    update_edge_ml();

    if (_buffer_flag) return;
    _buffer_flag = true;

    if (_cycle++ % 2) {
        if (update_pdm()) return;
        update_play();
    } else {
        if (update_play()) return;
        update_pdm();
    }
    BLE.poll();
}

unsigned long last = 0;

void Configuration_Handler::update_edge_ml() {
    unsigned int now = millis();
    if (now - _edge_ml_last > _edge_ml_delay) {
        _edge_ml_last = now;
        _buffer_flag = false;
        edge_ml_generic.update();

        return;

        CircularBlockBuffer * pdm = PDM2.get_buffer();
        CircularBlockBuffer * i2s = i2s_player.get_buffer();


        // PDM: Write Big / Read Small
        // I2S: Write Small / Read Big
        Serial.println("PDM:  " + String(pdm->available_write()) + "  " + String(pdm->available_read()));
        Serial.println("I2S:  " + String(i2s->available_write()) + "  " + String(i2s->available_read()) + "\n");
        Serial.println("Time " + String(now-last));
        last = now;
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
    edge_ml_generic.update();

    config.sensorId = BARO_TEMP;
    config.sampleRate = conf->BARO_rate * _rate_factor;
    edge_ml_generic.configure_sensor(config);
    edge_ml_generic.update();

    config.sensorId = PLAYER;
    config.sampleRate = float(conf->Play);
    Audio_Player::config_callback(&config);

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
    last = millis();
}

bool Configuration_Handler::check_overlap() {
    return (millis() - _edge_ml_last > _buffer_interval_time);
}

void Configuration_Handler::config_callback(SensorConfigurationPacket *config) {
    // Check for Configuration ID
    if (config->sensorId != CONFIGURATION) return;

    int config_num = int(config->sampleRate);

    conf_handler.configure(config_num);
}

Configuration_Handler conf_handler;


