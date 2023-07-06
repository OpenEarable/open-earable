#include "PDM_MIC_Sensor.h"

int PDM_MIC_Sensor::_sampleRate = sampleRate_default;;
bool PDM_MIC_Sensor::_send_serial = false;

PDM_MIC_Sensor::PDM_MIC_Sensor() {
}

bool PDM_MIC_Sensor::init() {
    pdm_setup();

    if (_send_serial) return true;
    return sd_setup();
}

void PDM_MIC_Sensor::update() {
    if (!_stream) return;

    while (PDM2.available()) {
        uint8_t *read_pointer = PDM2.getReadPointer();
        int size = (int) PDM2.getBlockSize();

        if (_send_serial) {
            Serial.write(read_pointer, size);
        } else {
            _sdWriter->writeChunk(read_pointer, size);
        }
        PDM2.incrementReadPointer();

        if (_chunks_disabled) break;
    }
}

void PDM_MIC_Sensor::start() {
    if (_stream) {
        return;
    }
    _stream = true;
    _sdWriter->cleanFile();
    _sdWriter->writeHeader();
    PDM2.clearBuffer();
    PDM2.start();
    if (!_send_serial) {
        _sdWriter->startRecording();
    }
}

void PDM_MIC_Sensor::end() {
    if (!_stream) {
        return;
    }
    _stream = false;
    PDM2.end();
    if (!_send_serial) {
        _sdWriter->endRecording();
    }
}

void PDM_MIC_Sensor::set_name(String name) {
    _sdWriter->setName(std::move(name));
}

void PDM_MIC_Sensor::setSampleRate(int sampleRate) {
    _sampleRate = sampleRate;
    PDM2.setSampleRate(sampleRate);
    _sdWriter->setSampleRate(sampleRate);
}

void PDM_MIC_Sensor::setGain(int gain) {
    _gain = gain;
}

bool PDM_MIC_Sensor::sd_setup() {
    _sdWriter = new WAVWriter();
    _sdWriter->setName(_name);
    _sdWriter->setSampleRate(_sampleRate);
    return _sdWriter->begin();
}

bool PDM_MIC_Sensor::pdm_setup() {
    PDM2.setBlockBufferSizes(_blockSize, _blockCount);
    PDM2.setSampleRate(_sampleRate);
    PDM2.setGain(_gain);
    return true;
}

void PDM_MIC_Sensor::enable_serial_data() {
    _send_serial = true;
}

void PDM_MIC_Sensor::disable_serial_data() {
    _send_serial = false;
}

void PDM_MIC_Sensor::enable_chunks() {
    _chunks_disabled = false;
}

void PDM_MIC_Sensor::disable_chunks() {
    _chunks_disabled = true;
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
    // Set sample rate in PMD Sensor
    pdm_mic_sensor.setSampleRate(sample_rate);

    // Make sure that pdm mic is not running already!
    pdm_mic_sensor.end();

    // Start pdm mic
    pdm_mic_sensor.start();
}

PDM_MIC_Sensor pdm_mic_sensor;
