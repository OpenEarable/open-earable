#include "SDManager.h"

SDManager::SDManager() {
    sd = new SdExFat();
}

SDManager::~SDManager() {
    delete sd;
}

bool SDManager::begin() {
    usage_count++;
    if (ready) return true;
    ready = true;
    return sd->begin(SPI_CC, SPI_SPEED);
}

void SDManager::end() {
    usage_count--;
    if (usage_count > 0) return;
    if (!ready) return;
    ready = false;
    if (_lastFile) {
        closeFile(_lastFile);
    }
    sd->end();
}

ExFatFile SDManager::openFile(const String& name, bool write) {
    return write ? sd->open(name, FILE_WRITE) : sd->open(name, FILE_READ);
}

void SDManager::closeFile(ExFatFile *file) {
    file->close();
    _lastFile = nullptr;
}

void SDManager::remove(const String& name) {
    sd->remove(name);
}

unsigned int SDManager::write_block_at(ExFatFile *file, int offset, uint8_t *block, int size) {
    _lastFile = file;
    file->seekSet(offset);
    return write_block(file, block, size);
}

unsigned int SDManager::write_block(ExFatFile *file, uint8_t *block, int size) {
    _lastFile = file;
    return file->write(block, size);
}

unsigned int SDManager::read_block_at(ExFatFile *file, int offset, uint8_t *block, int size) {
    _lastFile = file;
    file->seekSet(offset);
    return file->read(block, size);
}

unsigned int SDManager::read_block(ExFatFile *file, uint8_t *block, int size) {
    _lastFile = file;
    return file->read(block, size);
}

SDManager sd_manager;
