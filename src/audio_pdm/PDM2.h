#ifndef _PDM2_H_INCLUDED
#define _PDM2_H_INCLUDED

#include <Arduino.h>
#include "Earable_Pins.h"
#include "InputDevice.h"

#ifndef analogPinToPinName
#include <pinDefinitions.h>
#endif

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

class PDMClass2 : public InputDevice {
public:
    PDMClass2();
    virtual ~PDMClass2();

    void setBuffer(uint8_t * buffer, int blockSize, int blockCount);

    bool begin(bool high=false);
    bool begin(int channels, int sampleRate, bool high=false);

    void start() override;
    void stop() override;

    bool consume(int n) override;

    void end();

    /*virtual int available();
    virtual int remaining();
    virtual int read(void* buffer, size_t size);*/

    void onReceive(void(*)(void));

    //PORTENTA_H7 min -12 max 51
    //NANO 33 BLE SENSe min 0 max 80
    //NICLA_VISION min 0 max 8
    void setGain(int gain);

    void setPins(int dinPin, int clkPin);
    void setChannels(int channels);
    void setSampleRate(int sampleRate);

    int getSampleRate();

    void setBlockBufferSizes(int blockSize, int blockCount);

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

    void (*_onReceive)(void);

    bool _available = false;
};

extern PDMClass2 PDM2;

#endif
