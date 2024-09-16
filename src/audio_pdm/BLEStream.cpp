#include "BLEStream.h"
#include "EdgeML.h"
#include "../custom_sensor/PDM_Sensor.h"
#include "Recorder.h" // for debugging only

#define FILT_ORDER 2

/*const float a[FILT_ORDER][3] = {{1        , -1.82509601,  0.83392686},
                                {1        , -1.91841076,  0.92769313}};
const float b[FILT_ORDER][3] = {{5.12320597e-06, 1.02464119e-05, 5.12320597e-06},
                                {1.00000000e+00, 2.00000000e+00, 1.00000000e+00}};*/

const float a[FILT_ORDER][3] = {{1, -1.36511724,  0.47759225},
                                {1, -1.6117271 ,  0.74452084}};
const float b[FILT_ORDER][3] = {{9.33498613e-04, 1.86699723e-03, 9.33498613e-04},
                                {1, 2, 1}};

int16_t ble_buffer[16];
int16_t sos_buffer[DEFAULT_CBB_BLOCK_SIZE];
int index_ble = 0;
//const int down_sample = 32;
const int down_sample = 8;

BLEStream::BLEStream() {
    //_wavWriter = new WAVWriter();
    filter = new SOSFilter(FILT_ORDER, b, a);
}

BLEStream::~BLEStream() {
    //delete _wavWriter;
    delete filter;
}

bool BLEStream::begin() {
    (*stream)->open();
    
    //_wavWriter->setSampleRate(_sampleRate);
    //_wavWriter->setChannels(_channels);
    //const bool writer_begin = _wavWriter->begin();
    //if (writer_begin) start();
    //return writer_begin;

    start();
    return true;
}

void BLEStream::setSampleRate(int sampleRate) {
    _sampleRate = sampleRate;
}

void BLEStream::setChannels(int channels) {
    _channels = channels;
}

bool BLEStream::available() {
    return _stream;
}

void BLEStream::end() {
    if (_stream) stop();
    
    (*stream)->close();
}

void BLEStream::setStream(BufferedStream ** stream) {
    if (_stream) end();
    this->stream = stream;
}

int BLEStream::provide(int max_cont) {
    if (!_stream) return 0;

    int cont = min((*stream)->get_contiguous_blocks(), max_cont);

    if (!cont) return 0;

    if (initial_drop > 0) {
        const int n = min(initial_drop, cont);
        (*stream)->provide(n);
        initial_drop -= n;
        cont -= n;

        if (!cont) return n;
    }

    uint8_t *read_pointer = (*stream)->buffer.getReadPointer();
    const int block_size = (*stream)->buffer.getBlockSize();
    //const int size = block_size * cont;

    //Serial.print("PDM-1: ");
    //Serial.println(recorder.available() ? (*recorder.target->stream)->remaining() : -1);

    for (int i = 0; i<cont; i++) {
        memcpy(sos_buffer, read_pointer + i * block_size, block_size);
        //filter->update((int16_t *)(read_pointer + i * block_size), block_size / 2);
        filter->update((int16_t *) sos_buffer, block_size / 2);
        for (int k = 0; k < block_size; k+=down_sample * sizeof(int16_t)) {
            //ble_buffer[index_ble++] = *((int16_t *)(read_pointer + i * block_size + k));
            ble_buffer[index_ble++] = *((int16_t *)(sos_buffer + k));
            if (index_ble == 16) {
                index_ble = 0;

                sensorProvider.update_manager();
                pdm_sensor.setBuffer((uint8_t*) ble_buffer);
                sensorProvider.update_sensor(PDM_MIC, true);
                bleHandler_G.update();
                BLE.poll();
            }
        }
    }

    /*
    for (int i = 0; i<size; i+=16*sizeof(int16_t)) {
        sensorProvider.update_manager();
        pdm_sensor.setBuffer(read_pointer + i);
        sensorProvider.update_sensor(PDM_MIC, true);
        bleHandler_G.update();
        BLE.poll();
    }*/

    Serial.print("PDM: ");
    Serial.println(recorder.available() ? (*recorder.target->stream)->remaining() : -1);

    //bool written = _wavWriter->writeChunk(read_pointer, size);

    /*bool written = true;

    if (!written) {
        (*stream)->close();
        return 0;
    }*/

    (*stream)->provide(cont);

    return cont;
}

void BLEStream::start() {
    if (_stream) return;

    filter->reset();
    index_ble = 0;

    _stream = true;
}

void BLEStream::stop() {
    if (!_stream) return;

    _stream = false;
}