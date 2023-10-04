#include "WAVWriter.h"

#include <utility>

WAVWriter::WAVWriter() {
}

WAVWriter::~WAVWriter() {

}

bool WAVWriter::begin() {
    return sd_manager.begin();
}

void WAVWriter::end() {
    sd_manager.closeFile(&file);
    //sd_manager.end();
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
    sd_manager.closeFile(&file);
    _name = std::move(name);
}

bool WAVWriter::writeHeader() {
    file = sd_manager.openFile(_name);

    if (!file) return false;

    uint32_t subChunk2Size = 0;
    uint32_t chunkSize = 36;

    file.seekSet(0);
    file.write(_chunkID,4);                  // Offset 0
    file.write((byte*)&chunkSize,4);         // Offset 4
    file.write(_format,4);                   // Offset 8
    file.write(_subChunk1ID,4);              // Offset 12
    file.write((byte*)&_subChunk1Size,4);    // Offset 16
    file.write((byte*)&_audioFormat,2);      // Offset 20
    file.write((byte*)&_channels,2);         // Offset 22
    file.write((byte*)&_sampleRate,4);       // Offset 24
    file.write((byte*)&_byteRate,4);         // Offset 28
    file.write((byte*)&_blockAlign,2);       // Offset 32
    file.write((byte*)&_bitsPerSample,2);    // Offset 34
    file.write(_subChunk2ID,4);              // Offset 36
    file.write((byte*)&subChunk2Size,4);     // Offset 40

    sd_manager.closeFile(&file);
    return true;
}

bool WAVWriter::writeHeaderSizes() {
    sd_manager.write_block_at(&file, 40, (byte*)&_subChunk2Size,4);

    _chunkSize = 36 + _subChunk2Size;
    sd_manager.write_block_at(&file, 4, (byte*)&_chunkSize,4);

    return true;
}

bool WAVWriter::writeChunk(uint8_t *block, int size) {
    unsigned int written = sd_manager.write_block(&file, block, size);
    _subChunk2Size += written;
    return written;
}

bool WAVWriter::startRecording() {
    file = sd_manager.openFile(_name);
    _subChunk2Size = 0;
    _chunkSize = 36;
    return file;
}

bool WAVWriter::endRecording() {
    writeHeaderSizes();
    sd_manager.closeFile(&file);
    return true;
}

void WAVWriter::cleanFile() {
    sd_manager.remove(_name);
}
