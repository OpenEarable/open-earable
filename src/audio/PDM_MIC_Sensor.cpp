#include "PDM_MIC_Sensor.h"

PDM_MIC_Sensor::PDM_MIC_Sensor() {

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

void PDM_MIC_Sensor::get_float_data(float *floatArray, int sensorID) {
    // not needed
}

void PDM_MIC_Sensor::get_int_data(int *intArray, int sensorID) {
    // Get status
    if (sensorID != PDM_MIC) return;

    intArray[0] = 1; // 1 Values
    intArray[1] = 1; // Recording
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




