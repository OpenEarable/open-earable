//
// Created by Dylan Ray Roodt on 30.08.2022.
//

#ifndef EDGE_ML_EARABLE_AUDIOMANAGER_H
#define EDGE_ML_EARABLE_AUDIOMANAGER_H

#include "PDM2.h"

class AudioManager {
public:
    AudioManager();
    void begin();

    void start();
    void stop();

    void warmup();

    void enable();
    void disable();

    void update();

    void clear();

    bool almost_full();
    bool full();

    short * get_fullbuffer();
    int get_fullbuffer_size();
private:
    int channels = 1;
    int frequency = 62500;
    //int frequency = 16000;

    const int pin_pdm_in = 22;
    const int pin_pdm_clk = 29;

    bool started = false;
    bool enabled = false;
    bool ready_flag = false;
    bool almost_full_flag = false;

    void copy_to_full();
};


#endif //EDGE_ML_EARABLE_AUDIOMANAGER_H
