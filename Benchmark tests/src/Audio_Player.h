#ifndef OPEN_EARABLE_AUDIO_PLAYER_H
#define OPEN_EARABLE_AUDIO_PLAYER_H

//#include <custom_sensor/SensorID_Earable.h>
//#include "EdgeML_Custom.h"

#include "FileWriter.h"
#include "I2S_Player.h"

const int audio_b_size = 4096;
const int audio_b_count = 8;
extern uint8_t AUDIO_BUFFER[audio_b_size * audio_b_count] __attribute__((aligned (16)));

class Audio_Player {
public:
    Audio_Player();
    ~Audio_Player();

    bool init();
    void update();
    int update_contiguous(int max_cont);

    void start();
    void end();

    void play();
    void stop();
    void pause();

    unsigned int get_sample_rate();
    unsigned int get_size();

    void set_name(String name);

    void pre_open_file();

    int ready_blocks();

    //static void config_callback(SensorConfigurationPacket * config);

private:
    bool _stream = false;

    int _default_offset = 44;
    unsigned int _cur_read_sd = _default_offset;

    const int _blockSize = 4096;
    int _blockCount = 10;  // 10   Old not needed

    int _preload_blocks = 6; // 12

    String _name = "Play.wav";

    FileWriter * _fileWriter;

    void i2s_setup();
    bool sd_setup();

    void preload_buffer();
    unsigned int sd_to_buffer();
    unsigned int sd_to_buffer(int multi);

    bool check_completed();
};

extern Audio_Player audio_player;


#endif //OPEN_EARABLE_AUDIO_PLAYER_H
