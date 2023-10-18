#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "utils/Provider.h"

#define MAX_WAV_NAME_LENGTH 64

struct __attribute__((packed)) WAVConfigurationPacket {
    uint8_t mode = 0;     // 0 => IDLE, 1 => WAV, 2 => FREQ, 3 => JINGLE
    //uint8_t state = 0;    // 0 => idle; 1 => start; 2 => pause; 3 => stop;
    uint8_t size = 0;     // size of name; max 64
    char name[MAX_WAV_NAME_LENGTH]{};
};

class AudioSource : public Provider {
public:
    virtual WAVConfigurationPacket get_config() = 0;
    virtual float getSampleRate() = 0;
};

#endif