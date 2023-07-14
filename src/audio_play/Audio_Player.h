#ifndef OPEN_EARABLE_AUDIO_PLAYER_H
#define OPEN_EARABLE_AUDIO_PLAYER_H

#include <custom_sensor/SensorID_Earable.h>

#include "utils/FileWriter.h"
#include "I2S_Player.h"

class Audio_Player {
public:
    Audio_Player();
    ~Audio_Player();

    bool init();
    void update();

    void start();
    void end();

    void play();
    void stop();
    void pause();

    unsigned int get_sample_rate();
    unsigned int get_size();

    void set_name(String name);

private:
    bool play_mode_file = true;

    bool _stream = false;

    int _default_offset = 44;
    unsigned int _cur_read_sd = _default_offset;

    const int _blockSize = 4096;
    int _blockCount = 5;

    String _name = "Play.wav";

    FileWriter * _fileWriter;

    void i2s_setup();
    bool sd_setup();

    void preload_buffer();
    unsigned int sd_to_buffer();
};

extern Audio_Player audio_player;


#endif //OPEN_EARABLE_AUDIO_PLAYER_H
