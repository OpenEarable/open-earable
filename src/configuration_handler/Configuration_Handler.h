#ifndef OPEN_EARABLE_CONFIGURATION_HANDLER_H
#define OPEN_EARABLE_CONFIGURATION_HANDLER_H

#include "EdgeML.h"

#include <custom_sensor/SensorID_Earable.h>

#include <audio_pdm/Recorder.h>
#include "audio_play/Audio_Player.h"

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
    void configure(int config_num, int config_info);
    void stop_all();

    static void config_callback(SensorConfigurationPacket * config);
private:
    bool _config_active = false;

    int _current_conf_num;

    float _rate_factor = 1.25;

    //void check_audioplayback(int config_info);
};

extern Configuration_Handler conf_handler;

#endif //OPEN_EARABLE_CONFIGURATION_HANDLER_H
