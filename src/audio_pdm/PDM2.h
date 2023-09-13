#ifndef _PDM2_H_INCLUDED
#define _PDM2_H_INCLUDED

#include <Arduino.h>
#include "Earable_Pins.h"

#ifndef analogPinToPinName
#include <pinDefinitions.h>
#endif

#include "utils/CircularBlockBuffer.h"

const int valid_sample_rates[] = {
        16000,
        20000,
        25000,
        31250,
        33333,
        40000,
        41667,
        50000,
        62500
};

class PDMClass2
{
public:
    PDMClass2();
    virtual ~PDMClass2();

    void setBuffer(uint8_t * buffer, int blockSize, int blockCount);

    int start(bool high=false);
    int start(int channels, int sampleRate, bool high=false);

    void end();

    virtual int available();
    virtual int remaining();
    virtual int read(void* buffer, size_t size);

    void onReceive(void(*)(void));

    //PORTENTA_H7 min -12 max 51
    //NANO 33 BLE SENSe min 0 max 80
    //NICLA_VISION min 0 max 8
    void setGain(int gain);

    void setPins(int dinPin, int clkPin);
    void setChannels(int channels);
    void setSampleRate(int sampleRate);

    void setBlockBufferSizes(int blockSize, int blockCount);

    size_t getTotalSize() const;
    size_t getBlockSize() const;
    size_t getBlockCount() const;

    int get_contiguous_blocks() const;

    uint8_t * getReadPointer();
    void incrementReadPointer();

    void clearBuffer();
    void resetBuffer();

    bool checkSampleRateValid(int sampleRate);

    unsigned long get_buffer_hits();

    CircularBlockBuffer * get_buffer();

// private:
    void IrqHandler(bool halftranfer);

private:
    int _dinPin = EPIN_PDMDIN; //29
    int _clkPin = EPIN_PDMCLK; //28

    int _channels = 1;
    int _sampleRate = 16000;

    int _gain = 20;

    bool _first = true;

    unsigned long _buffer_hits = 0;

    CircularBlockBuffer _blockBuffer;

    void (*_onReceive)(void);
};

extern PDMClass2 PDM2;

#endif
