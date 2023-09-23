#ifndef OPEN_EARABLE_EQUALIZER_H
#define OPEN_EARABLE_EQUALIZER_H

#include "Arduino.h"

#define EQ_ORDER 3

class Equalizer {
public:
    Equalizer();
    ~Equalizer();

    void reset();
    void update(int16_t * data, int length);
private:
    float buffer[EQ_ORDER][2] = {0};
};

#endif //OPEN_EARABLE_AUDIO_PLAYER_H
