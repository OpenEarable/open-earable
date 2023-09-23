#ifndef OPEN_EARABLE_TONE_H
#define OPEN_EARABLE_TONE_H

#include "Arduino.h" // Needed for PI
#include "AudioSource.h"

struct ToneSetting {
    float frequency;
    float amplitude;
};

class Tone : public AudioSource {
public:
    Tone(float frequency = 440, float amplitude = 0.5);
    Tone(ToneSetting (*modulation)(), int call_every = 16);
    ~Tone();

    void set_frequency(float frequency);
    void set_amplitude(float amplitude);

    int provide(int n) override;
    bool available() override;
    bool begin() override;
    void end() override;
    void setStream(BufferedStream ** stream) override;

    int get_max_frequency();
    int get_min_frequency();

    WAVConfigurationPacket get_config() override;

private:
    bool _available = false;

    const float _frequency_low = 300;
    const float _frequency_high = 22000;

    const float _sample_rate = (float)1e6/23; // Determined by i2s config

    int _block_size;
    float _delta;

    ToneSetting _tone;
    ToneSetting (*_modulation)() = NULL;
    int call_every = 16;

    //const float _2_PI = 2.f * PI;

    float _t = 0;
    int t_call = 0;

    const float MAX_INT16 = ((1 << 15)-1);

    void update();
};

#endif //OPEN_EARABLE_TONE_H
