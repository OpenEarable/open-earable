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
    /*_sampleRate = sampleRate;
    _byteRate = _channels * _sampleRate * _bitsPerSample/8;*/
    info.setSampleRate(sampleRate);
}

void WAVWriter::setChannels(int channels) {
    /*_channels = channels;
    _byteRate = _channels * _sampleRate * _bitsPerSample/8;
    _blockAlign = _channels * _bitsPerSample/8;*/

    info.setNumChannels(channels);
}

void WAVWriter::setName(String name) {
    sd_manager.closeFile(&file);
    _name = std::move(name);
}

bool WAVWriter::writeHeader() {
    file = sd_manager.openFile(_name);

    if (!file) return false;

    file.seekSet(0);

    file.write((byte*)&info,sizeof(info));

    sd_manager.closeFile(&file);
    return true;
}

bool WAVWriter::writeHeaderSizes() {
    sd_manager.write_block_at(&file, 40, (byte*)&info.subchunk2Size,4);

    info.chunkSize = 36 + info.subchunk2Size;
    sd_manager.write_block_at(&file, 4, (byte*)&info.chunkSize,4);

    return true;
}

bool WAVWriter::writeChunk(uint8_t *block, int size) {
    unsigned int written = sd_manager.write_block(&file, block, size);
    info.subchunk2Size += written;
    return written;
}

bool WAVWriter::startRecording() {
    file = sd_manager.openFile(_name);

    info.subchunk2Size = 0;
    info.chunkSize = 36;
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
