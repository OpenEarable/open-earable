#ifndef OPEN_EARABLE_TONE_H
#define OPEN_EARABLE_TONE_H

#include "Arduino.h" // Needed for PI
#include "I2S_Player.h"
#include "utils/CircularBlockBuffer.h"
#include "utils/Provider.h"

class Tone : public Provider {
public:
    Tone();
    ~Tone();
    void setup();
    void start(int frequency);
    void end();
    void update();

    int get_max_frequency();
    int get_min_frequency();

    int provide(int n) override;

private:
    const int _frequency_low = 300;
    const int _frequency_high = 22000;

    const short _short_max = 32767;
    const float _sample_rate = 45454.5; // Determined by i2s config

    //CircularBlockBuffer * _circ;

    int _original_blocks{};
    int _block_size{};

    int _extra_blocks = 6;
    int _max_buffer{};
    short * _buffer;

    int _cur_pos = 0;
    int _end_index;

    int _current_frequency = 500;

    double _multiplier{};

    short _sinewave(int tick);
    void _calc_multi();

};


#endif //OPEN_EARABLE_TONE_H
