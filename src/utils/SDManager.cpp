#include "SDManager.h"
#include "Earable_Pins.h"

SDManager::SDManager() {
    sd = new SdExFat();
}

SDManager::~SDManager() {
    delete sd;
}

bool SDManager::begin() {
    if (_available) return true;
    pinMode(EPIN_SD_STATE, INPUT_PULLUP);

    _read_sd_state();

    // prevent multiattachment of the interrupt
    detachInterrupt(digitalPinToInterrupt(EPIN_SD_STATE));
    attachInterrupt(digitalPinToInterrupt(EPIN_SD_STATE), _read_sd_state, CHANGE);

    if (!sd_inserted) {
        Serial.println("No SD card inserted.");
        return false;
    }

    _available = sd->begin(SPI_CC, SPI_SPEED);
    return _available;
}

void SDManager::end() {
    if (!_available) return;
    if (_lastFile) {
        closeFile(_lastFile);
    }
    sd->end();

    detachInterrupt(digitalPinToInterrupt(EPIN_SD_STATE));
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
    if (!_available) return;
    sd->mkdir(name);
}

void SDManager::remove(const String& name) {
    if (!_available) return;
    sd->remove(name);
}

unsigned int SDManager::write_block_at(ExFatFile *file, int offset, uint8_t *block, int size) {
    if (!_available) return 0;
    _lastFile = file;
    file->seekSet(offset);
    return write_block(file, block, size);
}

unsigned int SDManager::write_block(ExFatFile *file, uint8_t *block, int size) {
    if (!_available) return 0;
    _lastFile = file;
    return file->write(block, size);
}

unsigned int SDManager::read_block_at(ExFatFile *file, int offset, uint8_t *block, int size) {
    _lastFile = file;
    file->seekSet(offset);
    return file->read(block, size);
}

unsigned int SDManager::read_block(ExFatFile *file, uint8_t *block, int size) {
    if (!_available) return 0;
    _lastFile = file;
    return file->read(block, size);
}

void SDManager::_read_sd_state() {
    sd_manager.sd_inserted = digitalRead(EPIN_SD_STATE);
    if (!sd_manager.sd_inserted) sd_manager._available = false;
    //Serial.print("SD: ");
    //Serial.println(sd_manager.sd_inserted);
}

SDManager sd_manager;
