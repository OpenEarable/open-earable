#include "SDManager.h"

SDManager::SDManager() {
    sd = new SdExFat();
}

SDManager::~SDManager() {
    delete sd;
}

bool SDManager::begin() {
    if (_available) return true;
    _available = sd->begin(SPI_CC, SPI_SPEED);
    return _available;
}

void SDManager::end() {
    if (!_available) return;
    if (_lastFile) {
        closeFile(_lastFile);
    }
    sd->end();
}

ExFatFile SDManager::openFile(const String& name, bool write) {
    if (!_available) begin();
    return write ? sd->open(name, FILE_WRITE) : sd->open(name, FILE_READ);
}

void SDManager::closeFile(ExFatFile *file) {
    if (!file->isOpen()) return;
    file->close();
    _lastFile = nullptr;
}

bool SDManager::exists(const String& name) {
    if (!_available) begin();
    return sd->exists(name);
}

void SDManager::mkdir(const String& name) {
    sd->mkdir(name);
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
