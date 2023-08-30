#ifndef OPEN_EARABLE_AUDIO_PLAYER_H
#define OPEN_EARABLE_AUDIO_PLAYER_H

#include <custom_sensor/SensorID_Earable.h>
#include "EdgeML_Custom.h"

#include "I2S_Player.h"
#include "Tone.h"

#include "utils/SDManager.h"

const int max_file = 10;

const int audio_b_size = 4096;
const int audio_b_count = 16;
extern uint8_t AUDIO_BUFFER[audio_b_size * audio_b_count] __attribute__((aligned (16)));

class Audio_Player {
public:
    Audio_Player();
    ~Audio_Player();

    bool init();
    void update();
    int update_contiguous(int max_cont);

    void set_file(int file_num);

    void start();
    void start_tone(int frequency);

    void end();

    void play();
    void stop();
    void pause();

    int get_max_file();
    int get_max_frequency();
    int get_min_frequency();

    bool is_mode_tone();

    unsigned int get_sample_rate();
    unsigned int get_size();

    void set_name(String name);

    int ready_blocks();

    static void config_callback(SensorConfigurationPacket * config);

private:
    bool _stream = false;
    bool _opened = false;

    bool _tone = false;

    int _selected = 0;

    int _default_offset = 44;
    unsigned int _cur_read_sd = _default_offset;

    int _preload_blocks = 8; // 12

    String _prefix = "Play";
    String _suffix = ".wav";

    ExFatFile _file;

    Tone * _tone_player;

    void i2s_setup();
    bool sd_setup();

    void preload_buffer();
    unsigned int sd_to_buffer();
    unsigned int sd_to_buffer(int multi);

    bool check_completed();

    bool open_file();
};

extern Audio_Player audio_player;


#endif //OPEN_EARABLE_AUDIO_PLAYER_H
