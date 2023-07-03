#include "PDM_MIC_Sensor.h"

PDM_MIC_Sensor::PDM_MIC_Sensor() {
    _sampleRate = sampleRate_default;
}

bool PDM_MIC_Sensor::init() {
    pdm_setup();

    if (send_serial) return true;
    return sd_setup();
}

void PDM_MIC_Sensor::update() {
    if (!stream) return;

    while (PDM2.available()) {
        uint8_t *read_pointer = PDM2.getReadPointer();
        int size = (int) PDM2.getBlockSize();

        if (send_serial) {
            Serial.write(read_pointer, size);
        } else {
            sdWriter->writeChunk(read_pointer, size);
        }
        PDM2.incrementReadPointer();

        if (chunks_disabled) break;
    }
}

void PDM_MIC_Sensor::start() {
    if (stream) {
        return;
    }
    stream = true;
    sdWriter->cleanFile();
    sdWriter->writeHeader();
    PDM2.clearBuffer();
    PDM2.start();
    if (!send_serial) {
        sdWriter->startRecording();
    }
}

void PDM_MIC_Sensor::end() {
    if (!stream) {
        return;
    }
    stream = false;
    PDM2.end();
    if (!send_serial) {
        sdWriter->endRecording();
    }
}

void PDM_MIC_Sensor::set_name(String name) {
    sdWriter->setName(std::move(name));
}

void PDM_MIC_Sensor::get_float_data(float *floatArray, int sensorID) {
    // not needed
}

void PDM_MIC_Sensor::get_int_data(int *intArray, int sensorID) {
    // Get status
    if (sensorID != PDM_MIC) return;

    intArray[0] = 1; // 1 Values
    intArray[1] = _active; // Recording
}

int PDM_MIC_Sensor::get_sensor_count() {
    return sensor_count;
}

void PDM_MIC_Sensor::setSampleRate(int sampleRate) {
    _sampleRate = sampleRate;
}

void PDM_MIC_Sensor::setGain(int gain) {
    _gain = gain;
}

bool PDM_MIC_Sensor::sd_setup() {
    sdWriter = new WAVWriter();
    sdWriter->setName(name);
    sdWriter->setSampleRate(_sampleRate);

    if (!sdWriter->begin()) {
        return false;
    }
    return true;
}

bool PDM_MIC_Sensor::pdm_setup() {
    PDM2.setBlockBufferSizes(_blockSize, _blockCount);
    PDM2.setSampleRate(_sampleRate);
    PDM2.setGain(_gain);
    return true;
}

void PDM_MIC_Sensor::enable_serial_data() {
    send_serial = true;
}

void PDM_MIC_Sensor::disable_serial_data() {
    send_serial = false;
}

void PDM_MIC_Sensor::enable_chunks() {
    chunks_disabled = false;
}

void PDM_MIC_Sensor::disable_chunks() {
    chunks_disabled = true;
}

void PDM_MIC_Sensor::set_active(int active) {
    _active = active;
}

void PDM_MIC_Sensor::config_callback(SensorConfigurationPacket *config) {
    // Check for PDM MIC ID
    if (config->sensorId != PDM_MIC) return;

    // Get sample rate
    int sample_rate = int(config->sampleRate);

    // End sensor if sample rate is 0
    if (sample_rate == 0) {
        pdm_mic_sensor.end();
        return;
    }

    // Check for valid sample rate
    if (!PDM2.checkSampleRateValid(sample_rate)) {
        sample_rate = sampleRate_default;
    }
    // Set sample rate in PDM2
    _sampleRate = sample_rate;
    PDM2.setSampleRate(_sampleRate);

    // Make sure that pdm mic is not running already!
    pdm_mic_sensor.end();

    // Start pdm mic
    pdm_mic_sensor.start();
}

PDM_MIC_Sensor pdm_mic_sensor;
