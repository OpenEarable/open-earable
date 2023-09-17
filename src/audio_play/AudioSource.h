#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "utils/Provider.h"

#define MAX_WAV_NAME_LENGTH 64

struct __attribute__((packed)) WAVConfigurationPacket {
    uint8_t state{};    // 0 => don't start; 1 => start; 2 => pause; 3 => unpause;
    uint8_t size{};     // size of name; max 64
    char name[MAX_WAV_NAME_LENGTH]{};
};

class AudioSource : public Provider {
public:
    virtual WAVConfigurationPacket get_config() = 0;
};

#endif