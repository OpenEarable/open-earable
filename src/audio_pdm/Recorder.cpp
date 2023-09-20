#include "Recorder.h"
#include "PDM2.h"

uint8_t PDM_BUFFER[pdm_b_size * pdm_b_count] __attribute__((aligned (16)));

Recorder::Recorder() {

}

Recorder::~Recorder() {
    
}

bool Recorder::begin() {
    if (_available) return true;
    PDM2.setBuffer(PDM_BUFFER, pdm_b_size, pdm_b_count);
    if (!target) return false;
    target->begin();
    if (!target->available()) return false;
    _available = pdm_setup();
    if(!_available) target->end();
    return _available;
}

void Recorder::end() {
    PDM2.end();
    _available = false;
}

void Recorder::record() {
    if (!_available) begin();
    if (_available) PDM2.start();
}

/**
 * TODO: Buffer fertig abbarbeiten
*/
void Recorder::stop() {
    if (!_available) return;
    PDM2.end();
    target->end();
    _available = false;
}

void Recorder::setDevice(InputDevice * device) {
    this->device = device;
    if (target) this->target->setStream(&(device->stream));
}

void Recorder::setTarget(AudioTarget * target) {
    this->target = target;
    if (device) this->target->setStream(&(device->stream));
}

bool Recorder::pdm_setup() {
    //PDM2.setBlockBufferSizes(_blockSize, _blockCount);
    PDM2.setBuffer(PDM_BUFFER, pdm_b_size, pdm_b_count);
    PDM2.setSampleRate(_sampleRate);
    PDM2.setGain(_gain);
    return PDM2.begin();
}

void Recorder::config_callback(SensorConfigurationPacket *config) {
    // Check for PDM MIC ID
    if (config->sensorId != PDM_MIC) return;

    // Get sample rate
    int sample_rate = int(config->sampleRate);

    // End sensor if sample rate is 0
    if (sample_rate == 0) {
        recorder.stop();
        return;
    }

    // Check for valid sample rate
    if (!PDM2.checkSampleRateValid(sample_rate)) {
        sample_rate = sampleRate_default;
    }

    // Set sample rate
    recorder._sampleRate = sample_rate;

    // Make sure that pdm mic is not running already!
    recorder.stop();

    // Start pdm mic
    recorder.record();
}

Recorder recorder;