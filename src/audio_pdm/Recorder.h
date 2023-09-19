#ifndef RECORDER_H
#define RECORDER_H

#include <custom_sensor/SensorID_Earable.h>
#include "EdgeML_Custom.h"

#include "utils/SDManager.h"

#include "AudioTarget.h"
#include "utils/BufferedInputStream.h"
#include "InputDevice.h"

const int pdm_b_size = 4096; //4096;
const int pdm_b_count = 8;
extern uint8_t PDM_BUFFER[pdm_b_size * pdm_b_count] __attribute__((aligned (16)));

const int sampleRate_default = 41667;  // 16000 Hz / 41667 Hz / 62500 Hz  // Default

/*
State Controller
*/
class Recorder {
public:
    Recorder();
    ~Recorder();

    bool begin();
    void end();

    void record();
    void stop();
    //void pause();

    void setTarget(AudioTarget * target);
    void setDevice(InputDevice * device);

    //void setSampleRate(int sampleRate);

    static void config_callback(SensorConfigurationPacket * config);
    
    AudioTarget * target;
    InputDevice * device;
private:
    int _sampleRate;
    bool _running = false;
    bool _available = false;

    bool pdm_setup();

    int _gain = 80; //50; // 20
};

extern Recorder recorder;

#endif //OPEN_EARABLE_AUDIO_PLAYER_H