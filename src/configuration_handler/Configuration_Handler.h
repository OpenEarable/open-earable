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
};

extern const int range_extension;
extern const int max_config;
extern const configuration_bundle CONFS[];

class Configuration_Handler {
public:
    bool check_active();

    void update();

    void configure(int config_num, int config_info);
    void stop_all();

    static void config_callback(SensorConfigurationPacket * config);
private:
    bool _config_active = false;

    int _current_conf_num;

    float _rate_factor = 1.25;

    unsigned int _edge_ml_delay;
    unsigned int _edge_ml_last;

    unsigned int _cycle = 0;

    unsigned int _overlap = 15; // Overlap time in ms
    unsigned int _buffer_interval_time;

    bool _buffer_flag = false;

    int _pdm_min_blocks = 5;
    int _play_min_blocks = 5; // MUST be 1 bigger than play_update

    int _pdm_update_blocks = 4;
    int _play_update_blocks = 4;

    float _alternate_loop_rate = 20;

    void update_edge_ml();
    bool update_pdm();
    bool update_play();
    bool check_overlap();
};

extern Configuration_Handler conf_handler;

#endif //OPEN_EARABLE_CONFIGURATION_HANDLER_H
