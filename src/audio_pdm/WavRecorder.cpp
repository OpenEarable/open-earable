#include "WavRecorder.h"

WavRecorder::WavRecorder(String name) : _name(name) {
    _wavWriter = new WAVWriter();
}

WavRecorder::~WavRecorder() {
    delete _wavWriter;
}

bool WavRecorder::begin() {
    (*stream)->open();
    _wavWriter->setName(_name);
    _wavWriter->setSampleRate(PDM2.getSampleRate());
    const bool writer_begin = _wavWriter->begin();
    if (writer_begin) start();
    return writer_begin;  
}

bool WavRecorder::available() {
    return _stream;
}

void WavRecorder::end() {
    if (_stream) stop();
    _wavWriter->end();
    (*stream)->close();
}

void WavRecorder::setStream(BufferedStream ** stream) {
    if (_stream) end();
    this->stream = stream;
}

int WavRecorder::provide(int max_cont) {
    if (!_stream) return 0;

    int cont = min((*stream)->get_contiguous_blocks(), max_cont);

    if (!cont) return 0;

    if (initial_drop > 0) {
        (*stream)->provide(min(initial_drop, cont));
        initial_drop -= min(initial_drop, cont);
    }

    uint8_t *read_pointer = (*stream)->buffer.getReadPointer();
    int size = (int) (*stream)->buffer.getBlockSize() * cont;

    _wavWriter->writeChunk(read_pointer, size);

    (*stream)->provide(cont);

    return cont;
}

void WavRecorder::start() {
    if (_stream) return;

    _stream = true;
    _wavWriter->cleanFile();
    _wavWriter->writeHeader();
    _wavWriter->startRecording();
}

void WavRecorder::stop() {
    if (!_stream) return;

    _stream = false;
    _wavWriter->endRecording();
}