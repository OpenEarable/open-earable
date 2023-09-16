#ifndef OPEN_EARABLE_AUDIO_PLAYER_H
#define OPEN_EARABLE_AUDIO_PLAYER_H

#include <custom_sensor/SensorID_Earable.h>
#include "EdgeML_Custom.h"

#include "Play_Service.h"
#include "I2S_Player.h"
#include "Tone.h"

#include "utils/SDManager.h"

#include "utils/Provider.h"
#include "utils/BufferedInputStream.h"
#include "Equalizer.h"


const int audio_b_size = 4096;
const int audio_b_count = 8;
extern uint8_t AUDIO_BUFFER[audio_b_size * audio_b_count] __attribute__((aligned (16)));

#define MAX_WAV_NAME_LENGTH 64
#define EQ_ORDER 3
//5

struct __attribute__((packed)) WAVConfigurationPacket {
    uint8_t state{};    // 0 => don't start; 1 => start; 2 => pause; 3 => unpause;
    uint8_t size{};     // size of name; max 64
    char name[MAX_WAV_NAME_LENGTH]{};
};

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
    void start_tone(int frequency);

    void end();

    void play();
    void stop();
    void pause();

    //int get_max_frequency();
    //int get_min_frequency();

    //bool is_mode_tone();

    unsigned int get_sample_rate();
    unsigned int get_size();

    /*void set_name(String name);

    int ready_blocks();
    int remaining_blocks();

    WAVConfigurationPacket make_wav_config();*/
    void ble_configuration(WAVConfigurationPacket& configuration);

    static void config_callback(SensorConfigurationPacket * config);

    void set_ready();
    void set_ready(int frequency);

    bool _ready = false;
    
    Provider * source;
private:
    //bool _tone = false;
    //int _last_frequency = 0;

    static bool _paused;

    bool _running = false;

    //ExFatFile _file;

    //Tone * _tone_player;

    //void i2s_setup();

    bool check_completed();

    void _check_ready();
};

extern Audio_Player audio_player;


#endif //OPEN_EARABLE_AUDIO_PLAYER_H
