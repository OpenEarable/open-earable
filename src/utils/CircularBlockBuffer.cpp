#include "CircularBlockBuffer.h"
#include "Arduino.h"

CircularBlockBuffer::CircularBlockBuffer() :
        _blockCount(DEFAULT_CBB_TOTAL_COUNT),
        _blockSize(DEFAULT_CBB_BLOCK_SIZE)
{
    // reset();
}

CircularBlockBuffer::CircularBlockBuffer(int blockSize, int blockCount) {
    setSizes(blockSize, blockCount); // reset in setSize
}

CircularBlockBuffer::~CircularBlockBuffer() {
    free(_buffer);
}

void CircularBlockBuffer::setSizes(int blockSize, int blockCount) {
    if (!blockSize) return;
    if (!blockCount) return;

    _blockSize = blockSize;
    _blockCount = blockCount;
    _totalSize = blockSize * blockCount;
    reset();
}

uint8_t *CircularBlockBuffer::get_buffer() {
    return _buffer;
}

size_t CircularBlockBuffer::getTotalSize() const {
    return _totalSize;
}

size_t CircularBlockBuffer::getBlockSize() const {
    return _blockSize;
}

size_t CircularBlockBuffer::getBlockCount() const {
    return _blockCount;
}

void CircularBlockBuffer::reset() {
    if (!_external_buffer) {
        _buffer = (uint8_t*)realloc(_buffer, _totalSize);
    }

    if (!_buffer) {
        Serial.println("Reallocation failed!");
    }

    memset(_buffer, 0x00, _totalSize);

    _readBlock = 0;
    _writeBlockCur = 0;
    _writeBlockNext = 1;

    /*_readOffset = 0;
    _writeOffsetCur = 0;
    _writeOffsetNext = _blockSize;*/

    _collision_w_count = 0;
    _collision_r_count = 0;
    _empty = true;
}

size_t CircularBlockBuffer::writeBlock(const uint8_t *buffer, size_t size) {
    if (size > _blockSize) {
        size = _blockSize;
    }

    if (size == 0) {
        return 0;
    }

    if (check_collision_r_next()) {
        _collision_w_count++;
        return 0;
    }

    memcpy(getCurWritePointer(), buffer, size);

    incrementWritePointer();

    return size;
}

size_t CircularBlockBuffer::readBlock(uint8_t *buffer, size_t size) {
    // Check Blocks between read and write_cur
    if (!available_read()) {
        _collision_r_count++;
        return 0;
    }

    if (size > _blockSize) {
        size = _blockSize;
    }

    if (size == 0) {
        return 0;
    }

    memcpy(buffer, getReadPointer(), size);

    incrementReadPointer();

    return size;
}

uint8_t *CircularBlockBuffer::getCurWritePointer() {
    return &(_buffer[_writeBlockCur * _blockSize]);
}

uint8_t *CircularBlockBuffer::getNextWritePointer() {
    return &(_buffer[_writeBlockNext * _blockSize]);
}


uint8_t *CircularBlockBuffer::getReadPointer() {
    return &(_buffer[_readBlock * _blockSize]);
}

int CircularBlockBuffer::get_contiguous_read_blocks() const {
    return min(available_read(), _blockCount - _readBlock);
}

int CircularBlockBuffer::get_contiguous_write_blocks() const {
    return min(available_write(), _blockCount - _writeBlockCur);
}

void CircularBlockBuffer::clear() {
    //_readOffset = _writeOffsetCur;
    _readBlock = _writeBlockCur;
    _empty = true;
}

int CircularBlockBuffer::available_read() const {
    if (_readBlock == _writeBlockCur) return _empty ? 0 : _blockCount;

    int diff = _writeBlockCur - _readBlock;

    return diff < 0 ? _blockCount + diff : diff;
}

int CircularBlockBuffer::available_write() const {
    if (_writeBlockCur == _readBlock) return _empty ? _blockCount : 0;

    int diff = _readBlock - _writeBlockCur;
    return diff < 0 ? _blockCount + diff : diff;
}

/*int CircularBlockBuffer::getWritePointer(int offset) const {
    int _offset_pointer = _readBlock + offset;
    

}*/


void CircularBlockBuffer::incrementWritePointer(int reserve) {
    //if (_readBlock == _writeBlockCur + reserve) _empty = true;
    if (_readBlock == _writeBlockCur) _empty = false;

    _writeBlockCur = _writeBlockNext++; // set and post increment
    //_writeOffsetCur = _writeOffsetNext;

    //++_writeBlockCur;
    //_writeOffsetCur += _blockSize;

    //++_writeBlockNext;
    //_writeOffsetNext += _blockSize;

    /*if (_writeBlockCur >= _blockCount) {
        _writeBlockCur = 0;
        _writeOffsetCur = 0;
    }*/

    if (_writeBlockNext >= _blockCount) {
        _writeBlockNext = 0;
        //_writeOffsetNext = 0;
    }
}

void CircularBlockBuffer::incrementReadPointer() {
    ++_readBlock;
    //_readOffset += _blockSize;

    if (_readBlock == _writeBlockCur) _empty = true;

    if (_readBlock >= _blockCount) {
        _readBlock = 0;
        //_readOffset = 0;
    }
}

// check_collision
bool CircularBlockBuffer::check_collision_r_next() const {
    return _writeBlockNext == _readBlock;
}

bool CircularBlockBuffer::check_collision_r_cur() const {
    return _writeBlockCur == _readBlock;
}

unsigned long CircularBlockBuffer::get_r_collisions() const {
    return _collision_r_count;
}

unsigned long CircularBlockBuffer::get_w_collisions() const {
    return _collision_w_count;
}

void CircularBlockBuffer::set_buffer(uint8_t *buffer, int blockSize, int blockCount) {
    _buffer = buffer;
    _blockSize = blockSize;
    _blockCount = blockCount;
    _totalSize = blockSize * blockCount;

    _external_buffer = true;
    reset();
}