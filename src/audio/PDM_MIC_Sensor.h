#ifndef OPEN_EARABLE_PDM_MIC_SENSOR_H
#define OPEN_EARABLE_PDM_MIC_SENSOR_H

#include "EdgeML_Custom.h"
#include <custom_sensor/SensorID_Earable.h>

#include "WAVWriter.h"
#include "PDM2.h"

class PDM_MIC_Sensor : public SensorInterface {
public:
    PDM_MIC_Sensor();

    bool init();
    void update();

    void start() override;
    void end() override;

    void get_float_data(float floatArray[], int sensorID) override;
    void get_int_data(int intArray[], int sensorID) override;

    int get_sensor_count() override;

    void setSampleRate(int sampleRate);
    void setGain(int gain);

    void enable_serial_data();
    void disable_serial_data();

    const int sensor_count = 1;
private:
    bool send_serial = false;
    bool stream = false;

    const int _blockSize = 4096;
    int _blockCount = 5;

    int _sampleRate = 41667; // 16000 Hz / 41667 Hz / 62500 Hz
    int _gain = 20;

    String name = "Recording.wav";

    WAVWriter * sdWriter;

    bool sd_setup();
    bool pdm_setup();
};


#endif //OPEN_EARABLE_PDM_MIC_SENSOR_H
