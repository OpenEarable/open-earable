#ifndef OPEN_EARABLE_CONFIGURATION_HANDLER_H
#define OPEN_EARABLE_CONFIGURATION_HANDLER_H

#include "EdgeML.h"

#include <custom_sensor/SensorID_Earable.h>

#include <audio_pdm/PDM_MIC_Sensor.h>
#include <audio_play/Audio_Player.h>

#include "ArduinoBLE.h"

struct configuration_bundle {
    float IMU_rate;
    float BARO_rate;
    int PDM_rate;
    int Play;
};

extern const int max_config;
extern configuration_bundle CONFS[];

class Configuration_Handler {
public:
    bool check_active();

    void update();

    void configure(int config_num);
    void stop_all();

    static void config_callback(SensorConfigurationPacket * config);
private:
    bool _config_active = false;

    int _current_conf_num;

    float _rate_factor = 1.25;

    unsigned int _edge_ml_delay;
    unsigned int _edge_ml_last;

    unsigned int _cycle;

    int _min_ready = 4;
    int _buffer_loops = 4;

    unsigned int _overlap = 3; // Overlap time in ms
    unsigned int _buffer_interval_time;

    unsigned int _overlap_prepopen = 2; // Overlap time in ms
    unsigned int _buffer_interval_time_prepopen;

    int _min_ready_pdm = 3;
    int _min_ready_play = 4;

    int _apx_buf_time_init = 14;
    int _apx_buf_time_min = 5;
    int _apx_pdm = _apx_buf_time_init;
    int _apx_play = _apx_buf_time_init;

    int counter = 0;

    float alternate_loop_rate = 10;

    void update_edge_ml();
    bool update_pdm();
    bool update_play();

    bool check_overlap();
    bool check_overlap_prepopen();
};

extern Configuration_Handler conf_handler;

#endif //OPEN_EARABLE_CONFIGURATION_HANDLER_H
