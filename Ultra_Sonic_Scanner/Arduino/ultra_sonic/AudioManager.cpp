//
// Created by Dylan Ray Roodt on 30.08.2022.
//

#include "AudioManager.h"

const int buffer_size = 2024;        // Bytes
const int buffer_points = buffer_size / 2;
short sampleBuffer[buffer_points];   // Buffer to read samples into, each sample is 16-bits
volatile int samplesRead;

int fullBuffer_offset = 0;           // Elements (2 Byte per element)
const int fullBuffer_size = 133120;   // Bytes
const int fullBuffer_points = fullBuffer_size / 2;
short fullBuffer[fullBuffer_points]; // Buffer to read all samples into, each sample is 16-bits

void onPDMdata();

void onPDMdata() {
    // Query the number of available bytes
    int bytesAvailable = PDM2.available();

    // Read into the sample buffer
    PDM2.read(sampleBuffer, bytesAvailable);

    // 16-bit, 2 bytes per sample
    samplesRead = bytesAvailable / 2;
}


AudioManager::AudioManager() {

}

void AudioManager::begin() {
    PDM2.init(pin_pdm_in, pin_pdm_clk);
    PDM2.setBufferSize(buffer_size);
    
    // Configure the data receive callback
    PDM2.onReceive(onPDMdata);

    if (!PDM2.begin(channels, frequency)) {
        while (1);
    }
}

void AudioManager::start() {
    if (started) return;
    started = true;
}

void AudioManager::stop() {
    if (!started) return;
    started = false;

    PDM2.end();
}

void AudioManager::warmup() {
    int wait = (int)(5*buffer_size * (1000. / frequency)) + 5;
    delay(wait);
    update();
    enable();
}

void AudioManager::enable() {
    if (enabled) return;
    enabled = true;
}

void AudioManager::disable() {
    if (!enabled) return;
    enabled = false;
}

void AudioManager::update() {
    if (!started) return;
    if (!enabled) {
        samplesRead = 0;
        return;
    }
    
    if (samplesRead) {
        copy_to_full();
        samplesRead = 0;
    }
}

void AudioManager::copy_to_full() {
    if (fullBuffer_offset >= fullBuffer_points - buffer_points) {
        int max_read = fullBuffer_points - fullBuffer_offset;

        for (int i = 0; i < max_read; i++) {
            fullBuffer[i + fullBuffer_offset] = sampleBuffer[i];
        }
        fullBuffer_offset += max_read;

        ready_flag = true;
        disable();
    } else {
        for (int i = 0; i < samplesRead; i++) {
            fullBuffer[i + fullBuffer_offset] = sampleBuffer[i];
        }
        fullBuffer_offset += samplesRead;
    }

    almost_full_flag = fullBuffer_offset > fullBuffer_points - 2*buffer_points;
}

void AudioManager::clear() {
    fullBuffer_offset = 0;
    ready_flag = false;
    almost_full_flag = false;

    for (short & i : fullBuffer) {
        i = 0;
    }
}

bool AudioManager::full() {
    return ready_flag;
}

bool AudioManager::almost_full() {
    return almost_full_flag;
}

short * AudioManager::get_fullbuffer() {
    return fullBuffer;
}

int AudioManager::get_fullbuffer_size() {
    return fullBuffer_offset * 2;
}
