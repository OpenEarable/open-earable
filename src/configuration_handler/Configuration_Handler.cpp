#include "Configuration_Handler.h"
#include "task_manager/TaskManager.h"


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

// NOTE! IMU_rate and BARO_rate must be compatible

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

void Configuration_Handler::configure(int config_num) {
    const configuration_bundle * conf;
    SensorConfigurationPacket config;

    //check_audioplayback(config_info);

    // config_num == 0 --> STOP
    // config_num > 0  --> config_num is configuration
    if (config_num < 0 || config_num >= max_config) return;

    stop_all();
    _current_conf_num = config_num;

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

    task_manager.begin(max(conf->BARO_rate, conf->IMU_rate));
}

void Configuration_Handler::config_callback(SensorConfigurationPacket *config) {
    // Check for Configuration ID
    if (config->sensorId != CONFIGURATION) return;

    int config_num = int(config->sampleRate);
    int config_info = int(config->latency); // Additional extra info

    Serial.print("CONFIGURATION: ");
    Serial.println(String(config_num));
    /*Serial.print("info: ");
    Serial.println(String(config_info));*/

    conf_handler.configure(config_num);
}

Configuration_Handler conf_handler;
