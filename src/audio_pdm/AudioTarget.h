#ifndef AUDIO_TARGET_H
#define AUDIO_TARGET_H

#include "utils/Provider.h"

class AudioTarget : public Provider {
public:
    virtual void setSampleRate(int sample_rate) = 0;
};

#endif