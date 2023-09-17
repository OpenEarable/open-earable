#ifndef OPEN_EARABLE_I2S_PLAYER_H
#define OPEN_EARABLE_I2S_PLAYER_H

#include <Arduino.h>

#include "Earable_Pins.h"
#include "Equalizer.h"

#ifndef analogPinToPinName
#include <pinDefinitions.h>
#endif

#include <nrf_i2s.h>
#include "utils/CircularBlockBuffer.h"
#include "utils/BufferedInputStream.h"
#include "OutputDevice.h"

#define NC 0xFFFFFFFF   //Not connected
#define WORD_SIZE 4     // 1 word = 4 bytes

struct sampling_mode {
    nrf_i2s_mck_t nrf_i2s_mck;
    nrf_i2s_ratio_t nrf_i2s_ratio;
};

extern sampling_mode file_mode;
extern sampling_mode const_freq;

class I2S_Player : public OutputDevice {
public:
    I2S_Player();
    ~I2S_Player();

    Equalizer * eq;

    void setBlockBufferSizes(int blockSize, int blockCount);
    void setBuffer(uint8_t * buffer, int blockSize, int blockCount);

    void start() override;
    void stop() override;

    void reset_buffer();

    void begin();
    void end();
    //void uninit();

    bool check_config_status();
    void clear_buffer();

    CircularBlockBuffer * get_buffer();

    bool is_running();

    void i2s_interrupt();

private:
    int _sckPin = EPIN_BCLK;    //23  P0_16
    int _lrckPin = EPIN_LRCLK;  //13  P0_13
    int _sdoutPin = EPIN_DIN;   //32  P1_0

    bool _i2s_config_status = false;

    //bool _end_flag = false;         // End of playback, buffer is empty
    bool _turn_off_flag = false;    // Request end, will play rest of buffer
    //bool _completed_flag = false;

    int count = 0;

    bool consume(int n) override;
};

extern I2S_Player i2s_player;


#endif //OPEN_EARABLE_I2S_PLAYER_H
