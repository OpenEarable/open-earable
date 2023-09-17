#ifndef OPEN_EARABLE_AUDIO_PLAYER_H
#define OPEN_EARABLE_AUDIO_PLAYER_H

#include <custom_sensor/SensorID_Earable.h>
#include "EdgeML_Custom.h"

#include "Play_Service.h"
#include "I2S_Player.h"
#include "Tone.h"

#include "utils/SDManager.h"

#include "AudioSource.h"
#include "utils/BufferedInputStream.h"
#include "Equalizer.h"


const int audio_b_size = 4096;
const int audio_b_count = 8;
extern uint8_t AUDIO_BUFFER[audio_b_size * audio_b_count] __attribute__((aligned (16)));

#define EQ_ORDER 3
//5

/*
State Controller
*/
class Audio_Player {
public:
    Audio_Player();
    ~Audio_Player();

    bool init();
    void update();
    int update_contiguous(int max_cont);

    void start();
    //void start_tone(int frequency);

    void end();

    void play();
    void stop();
    void pause();

    unsigned int get_sample_rate();
    unsigned int get_size();

    WAVConfigurationPacket make_wav_config();
    void ble_configuration(WAVConfigurationPacket& configuration);

    void setSource(AudioSource * source);

    static void config_callback(SensorConfigurationPacket * config);

    void set_ready();
    void set_ready(int frequency);

    bool _ready = false;
    
    AudioSource * source;
private:
    static bool _paused;
    bool _running = false;

    //bool check_completed();
    void _check_ready();
};

extern Audio_Player audio_player;


#endif //OPEN_EARABLE_AUDIO_PLAYER_H
