#ifndef TEST_WAVWRITER_H
#define TEST_WAVWRITER_H

#include "Arduino.h"

#include "SDManager.h"


class WAVWriter {
public:
    WAVWriter();
    ~WAVWriter();
    bool begin();
    void end();

    void cleanFile();

    void setName(String name);

    void setSampleRate(int sampleRate);
    void setChannels(int channels);

    bool writeHeader();

    bool startRecording();
    bool writeChunk(uint8_t *block, int size);
    bool endRecording();

private:
    ExFatFile file;

    String _name = "Recording.wav";

    // Wav Header def
    char _chunkID[4] = {'R', 'I', 'F', 'F'};
    char _format[4] = {'W', 'A', 'V', 'E'};
    char _subChunk1ID[4] = {'f', 'm', 't', ' '};
    uint16_t _audioFormat = 1; // indicates compression
    ///: For PCM == 16, since audioFormat == uint16_t
    uint32_t _subChunk1Size = 16;
    uint16_t _bitsPerSample = 16;
    char _subChunk2ID[4] = {'d', 'a', 't', 'a'};

    // Set with functions
    uint16_t _channels = 1;
    uint32_t _sampleRate = 16000;

    // Calculated
    ///: SampleRate * NumChannels * BitsPerSample/8
    uint32_t _byteRate = _channels * _sampleRate * _bitsPerSample/8;
    ///: The number of byte for one frame NumChannels * BitsPerSample/8
    uint16_t _blockAlign = _channels * _bitsPerSample/8;
    uint32_t _subChunk2Size = 0;
    uint32_t _chunkSize = 36;

    unsigned int _header_size = 44;

    bool writeHeaderSizes();
};


#endif //TEST_WAVWRITER_H
