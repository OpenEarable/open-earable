#ifndef WAV_PLAYER_H
#define WAV_PLAYER_H

#include "utils/Provider.h"
#include "utils/SDManager.h"
#include "Play_Service.h"

/**
* TODO: check file format
*/

class WavPlayer : public Provider {
public:
    WavPlayer(BufferedStream ** stream);
    ~WavPlayer();
    int provide(int n) override;
    bool available() override;
    void begin() override;

    WAVConfigurationPacket make_wav_config();
    //void ble_configuration(WAVConfigurationPacket& configuration);

    void set_name(String name);
    unsigned int get_sample_rate();
    unsigned int get_size();
private:
    ExFatFile _file;

    bool _available = false;
    bool _opened = false;
    
    String _name = "play_file.wav"; //"Play.wav";

    int _default_offset = 44;
    unsigned int _cur_read_sd = _default_offset;

    int _preload_blocks = 6; // 12

    bool sd_setup();

    void preload_buffer();
    //unsigned int sd_to_buffer();
    unsigned int sd_to_buffer(int multi);

    bool open_file();
};

#endif