#ifndef OPEN_EARABLE_PDM_MIC_SENSOR_H
#define OPEN_EARABLE_PDM_MIC_SENSOR_H

#include <custom_sensor/SensorID_Earable.h>

#include "utils/WAVWriter.h"
#include "PDM2.h"

const int sampleRate_default = 41667;  // 16000 Hz / 41667 Hz / 62500 Hz  // Default

class PDM_MIC_Sensor {
public:
    PDM_MIC_Sensor();

    bool init();
    void update();

    void start();
    void end();

    void set_name(String name);

    void setSampleRate(int sampleRate);
    void setGain(int gain);

    void enable_serial_data();
    void disable_serial_data();

    void enable_chunks();
    void disable_chunks();

    void pre_open_file();

    static void config_callback(SensorConfigurationPacket * config);
private:
    static int _sampleRate;
    static bool _send_serial;

    bool _stream = false;
    bool _chunks_enabled = false;

    const int _blockSize = 4096;
    int _blockCount = 8;

    int _gain = 20;

    String _name = "Recording.wav";

    WAVWriter * _wavWriter;

    bool sd_setup();
    bool pdm_setup();
};

extern PDM_MIC_Sensor pdm_mic_sensor;


#endif //OPEN_EARABLE_PDM_MIC_SENSOR_H
