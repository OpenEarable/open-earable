#ifndef OPEN_EARABLE_PDM_MIC_SENSOR_H
#define OPEN_EARABLE_PDM_MIC_SENSOR_H


#include "EdgeML_Custom.h"
#include <custom_sensor/SensorID_Earable.h>

#include "utils/WAVWriter.h"
#include "PDM2.h"


const int pdm_b_size = 4096;
const int pdm_b_count = 16;
extern uint8_t PDM_BUFFER[pdm_b_size * pdm_b_count] __attribute__((aligned (16)));

const int sampleRate_default = 41667;  // 16000 Hz / 41667 Hz / 62500 Hz  // Default

class PDM_MIC_Sensor {
public:
    PDM_MIC_Sensor();

    bool init();
    void update();
    int update_contiguous(int max_cont);

    void start();
    void end();

    void set_name(String name);

    void setSampleRate(int sampleRate);
    void setGain(int gain);

    void enable_serial_data();
    void disable_serial_data();

    void enable_chunks();
    void disable_chunks();

    int ready_blocks();

    static void config_callback(SensorConfigurationPacket * config);
private:
    static int _sampleRate;
    static bool _send_serial;

    bool _stream = false;
    bool _chunks_enabled = false;
    bool _first_ignore = true;

    int _gain = 20; // 20

    String _name = "Recording.wav";

    WAVWriter * _wavWriter;

    bool sd_setup();
    bool pdm_setup();
};

extern PDM_MIC_Sensor pdm_mic_sensor;


#endif //OPEN_EARABLE_PDM_MIC_SENSOR_H
