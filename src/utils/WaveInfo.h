#ifndef WAVE_INFO_H
#define WAVE_INFO_H

#include "Arduino.h"
#define PCM 1

struct WaveInfo {
    char chunkID[4] = {'R', 'I', 'F', 'F'};
    uint32_t chunkSize = 36;
    char format[4] = {'W', 'A', 'V', 'E'};
    char subchunk1ID[4] = {'f', 'm', 't', ' '};
    uint32_t subchunk1Size = 16;
    uint16_t formatTag = PCM;
    uint16_t numChannels = 1;
    uint32_t sampleRate = 44100;
    uint32_t byteRate = sampleRate * numChannels * bitsPerSample/8;
    uint16_t blockAlign = numChannels * ((bitsPerSample + 7) / 8);
    uint16_t bitsPerSample = 16;
    //uint16_t extraParamSize = 16;
    char subchunk2ID[4] = {'d', 'a', 't', 'a'};
    uint32_t subchunk2Size = 0;

public:
    void setSampleRate(int sample_rate) {
        sampleRate = sample_rate;
        byteRate = sampleRate * numChannels * bitsPerSample/8;
    };

    void setNumChannels(int num_channels) {
        numChannels = num_channels;
        byteRate = sampleRate * numChannels * bitsPerSample/8;
        blockAlign = numChannels * ((bitsPerSample + 7) / 8);
    };

    void setBitsPerSample(int bits_per_sample) {
        bitsPerSample = bits_per_sample;
        byteRate = sampleRate * numChannels * bitsPerSample/8;
        blockAlign = numChannels * ((bitsPerSample + 7) / 8);
    };
};

const WaveInfo PLAYER_FORMAT;

#endif //WAVE_INFO_H