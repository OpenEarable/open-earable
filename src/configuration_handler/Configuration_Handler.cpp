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

    /*
    while (PDM2.available()) {
        pdm_mic_sensor.update();
    }

    while (audio_player.buffer_available()) {
        audio_player.update();
    }
     */

    update_edge_ml();

    if (check_overlap()) return;
    bool overlapped;
    counter = 0;
    //if (_cycle++ % 2) {
    if (_cycle++ % 2) {
        unsigned long t1 = millis();

        overlapped = update_pdm();
        if (counter != 0) {
            float avg = float(millis() - t1) / counter;
            _apx_pdm = 0.5 * (avg + _apx_pdm);
            _apx_pdm = max(_apx_pdm, _apx_buf_time_min);
            //if (avg > 0.5) Serial.println("F PDM: " + String(avg));
        }

        if (overlapped) {
            if (!check_overlap_prepopen()) {
                if (audio_player.ready_blocks() < _min_ready_play) return;
                audio_player.pre_open_file();
            }
            return;
        }
        counter = 0;
        t1 = millis();
        update_play();
        if (counter != 0) {
            float avg = float(millis() - t1) / counter;
            //if (avg > 0.5) Serial.println("Play: " + String(avg));
        }
    } else {
        unsigned long t1 = millis();
        overlapped = update_play();
        if (counter != 0) {
            float avg = float(millis() - t1) / counter;
            _apx_play = 0.5 * (avg + _apx_play);
            _apx_play = max(_apx_play, _apx_buf_time_min);
            //if (avg > 0.5) Serial.println("F Play: " + String(avg));
        }
        if (overlapped) {
            //if (!check_overlap_prepopen()) pdm_mic_sensor.pre_open_file();
            return;
        }
        counter = 0;
        t1 = millis();

        update_pdm();
        if (counter != 0) {
            float avg = float(millis() - t1) / counter;
            //if (avg > 0.5) Serial.println("PDM: " + String(avg));
        }
    }
    BLE.poll();
}

unsigned long last = 0;

void Configuration_Handler::update_edge_ml() {
    unsigned int now = millis();
    if (now - _edge_ml_last > _edge_ml_delay) {
        _edge_ml_last = now;

        CircularBlockBuffer * pdm = PDM2.get_buffer();
        CircularBlockBuffer * i2s = i2s_player.get_buffer();


        // PDM: Write Big / Read Small
        // I2S: Write Small / Read Big
        Serial.println("PDM:  " + String(pdm->available_write()) + "  " + String(pdm->available_read()));
        Serial.println("I2S:  " + String(i2s->available_write()) + "  " + String(i2s->available_read()) + "\n");

        Serial.println("Time " + String(now-last));
        last = now;

        edge_ml_generic.update();
    }
}

bool Configuration_Handler::update_pdm() {
    if (pdm_mic_sensor.ready_blocks() < _min_ready_pdm) return check_overlap();

    int remaining = int(_edge_ml_delay - (millis() - _edge_ml_last));
    int max_buffers = remaining / _apx_pdm;

    int cont = pdm_mic_sensor.update_contiguous(max_buffers);
    counter += cont;
    if (check_overlap()) return true;

    cont = max_buffers - cont;
    if (!cont || max_buffers == cont) return false;
    pdm_mic_sensor.update_contiguous(cont);
    counter += cont;

    return check_overlap();
}

bool Configuration_Handler::update_play() {
    if (audio_player.ready_blocks() < _min_ready_play) return check_overlap();

    int remaining = int(_edge_ml_delay - (millis() - _edge_ml_last));
    int max_buffers = remaining / _apx_play;

    int cont = audio_player.update_contiguous(max_buffers);

    counter += cont;
    if (check_overlap()) return true;

    cont = max_buffers - cont;
    if (!cont || max_buffers == cont) return false;
    audio_player.update_contiguous(cont);
    counter += cont;

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
    Serial.println("Here");
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

    config.sensorId = PLAYER;
    config.sampleRate = float(conf->Play);
    Audio_Player::config_callback(&config);

    config.sensorId = PDM_MIC;
    config.sampleRate = float(conf->PDM_rate);
    PDM_MIC_Sensor::config_callback(&config);

    float edge_rate = max(conf->BARO_rate, conf->IMU_rate);

    if (edge_rate == 0) {
        edge_rate = alternate_loop_rate;
    }

    _edge_ml_delay = (int)(1000.0/edge_rate);
    _edge_ml_last = millis();

    _buffer_interval_time = _edge_ml_delay - _overlap;
    _overlap_prepopen = _edge_ml_delay - _overlap_prepopen;

    Serial.println(_buffer_interval_time);
    Serial.println(_overlap_prepopen);
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


