#include "WAVWriter.h"

#include <utility>

WAVWriter::WAVWriter() {
    sd = new SdExFat();
}

WAVWriter::~WAVWriter() {
    delete sd;
}

bool WAVWriter::begin() {
    return sd->begin(SPI_CC, SPI_SPEED);
}

void WAVWriter::end() {
    closeFile();
    sd->end();
}

void WAVWriter::setSampleRate(int sampleRate) {
    _sampleRate = sampleRate;
    _byteRate = _channels * _sampleRate * _bitsPerSample/8;
}

void WAVWriter::setChannels(int channels) {
    _channels = channels;
    _byteRate = _channels * _sampleRate * _bitsPerSample/8;
    _blockAlign = _channels * _bitsPerSample/8;
}

void WAVWriter::setName(String name) {
    closeFile();
    _name = std::move(name);
}

bool WAVWriter::writeHeader() {
    openFile();

    if (!wavFile) return false;

    uint32_t subChunk2Size = 0;
    uint32_t chunkSize = 36;

    wavFile.seekSet(0);
    wavFile.write(_chunkID,4);                  // Offset 0
    wavFile.write((byte*)&chunkSize,4);         // Offset 4
    wavFile.write(_format,4);                   // Offset 8
    wavFile.write(_subChunk1ID,4);              // Offset 12
    wavFile.write((byte*)&_subChunk1Size,4);    // Offset 16
    wavFile.write((byte*)&_audioFormat,2);      // Offset 20
    wavFile.write((byte*)&_channels,2);         // Offset 22
    wavFile.write((byte*)&_sampleRate,4);       // Offset 24
    wavFile.write((byte*)&_byteRate,4);         // Offset 28
    wavFile.write((byte*)&_blockAlign,2);       // Offset 32
    wavFile.write((byte*)&_bitsPerSample,2);    // Offset 34
    wavFile.write(_subChunk2ID,4);              // Offset 36
    wavFile.write((byte*)&subChunk2Size,4);     // Offset 40

    closeFile();
    return true;
}

void WAVWriter::openFile() {
    if (_is_open) return;
    wavFile = sd->open(_name, FILE_WRITE);
    _is_open = true;
}

void WAVWriter::closeFile() {
    if (!_is_open) return;
    wavFile.close();
    _is_open = false;
}

bool WAVWriter::writeHeaderSizes(bool close) {
    //subChunk2Size = chunk_count * 2; //dont forget
    if (!_is_open) openFile();

    if (!wavFile) return false;

    wavFile.seekSet(40);
    wavFile.write((byte*)&_subChunk2Size,4);

    wavFile.seekSet(4);
    _chunkSize = 36 + _subChunk2Size;
    wavFile.write((byte*)&_chunkSize,4);

    if (close) closeFile();
    return true;
}

bool WAVWriter::writeChunk(uint8_t *block, int size) {
    if (!wavFile) return false;
    wavFile.write(block, size);
    _subChunk2Size += size;
    return true;
}

bool WAVWriter::startRecording() {
    openFile();
    _subChunk2Size = 0;
    _chunkSize = 36;
    return wavFile;
}

bool WAVWriter::endRecording() {
    writeHeaderSizes();
    closeFile();
    return true;
}

void WAVWriter::cleanFile() {
    sd->remove(_name);
}


