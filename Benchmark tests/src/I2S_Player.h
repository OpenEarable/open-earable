#ifndef OPEN_EARABLE_I2S_PLAYER_H
#define OPEN_EARABLE_I2S_PLAYER_H

#include <Arduino.h>

#ifndef analogPinToPinName
#include <pinDefinitions.h>
#endif

#include <nrf_i2s.h>
#include "CircularBlockBuffer.h"

#define NC 0xFFFFFFFF   //Not connected
#define WORD_SIZE 4     // 1 word = 4 bytes

struct sampling_mode {
    nrf_i2s_mck_t nrf_i2s_mck;
    nrf_i2s_ratio_t nrf_i2s_ratio;
};

extern sampling_mode file_mode;
extern sampling_mode const_freq;

class I2S_Player {
public:
    I2S_Player();
    ~I2S_Player();

    void setBlockBufferSizes(int blockSize, int blockCount);

    void setBuffer(uint8_t * buffer, int blockSize, int blockCount);

    void set_mode_file(bool play_file);
    bool get_mode_file();

    void start();
    void end();

    void play();
    void stop();
    void pause();
    void completed();

    bool get_turn_off();
    bool get_end();
    bool get_completed();

    int get_contiguous_blocks() const;

    int available();
    uint8_t * getWritePointer();
    void incrementWritePointer();

    bool check_config_status();

    void clear_buffer();

    void i2s_interrupt();

    CircularBlockBuffer * get_buffer();

private:
    int _sckPin = 23;   // P0_16
    int _lrckPin = 13;  // P0_13
    int _sdoutPin = 32; // P1_0

    bool play_mode_file = true;

    CircularBlockBuffer _blockBuffer;

    bool _i2s_config_status = false;

    bool _end_flag = false;         // End of playback, buffer is empty
    bool _turn_off_flag = false;    // Request end, will play rest of buffer
    bool _completed_flag = false;

    int count = 0;
};

extern I2S_Player i2s_player;


#endif //OPEN_EARABLE_I2S_PLAYER_H
