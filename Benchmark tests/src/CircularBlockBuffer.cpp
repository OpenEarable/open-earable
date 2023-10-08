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
        Serial.println("SHIT!");
    }

    memset(_buffer, 0x00, _totalSize);

    _readBlock = 0;
    _writeBlockCur = 0;
    _writeBlockNext = 1;

    _readOffset = 0;
    _writeOffsetCur = 0;
    _writeOffsetNext = _blockSize;

    _collision_w_count = 0;
    _collision_r_count = 0;
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

    memcpy(&_buffer[_writeOffsetCur], buffer, size);

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

    memcpy(buffer, &_buffer[_readOffset], size);

    incrementReadPointer();

    return size;
}

uint8_t *CircularBlockBuffer::getCurWritePointer() {
    return &(_buffer[_writeOffsetCur]);
}

uint8_t *CircularBlockBuffer::getNextWritePointer() {
    return &(_buffer[_writeOffsetNext]);
}


uint8_t *CircularBlockBuffer::getReadPointer() {
    return &(_buffer[_readOffset]);
}

int CircularBlockBuffer::get_contiguous_read_blocks() const {
    if (_readBlock == _writeBlockCur) return 0;

    long diff = (long)(_writeBlockCur - _readBlock);

    if (diff < 0) {
        return (int)(_blockCount - _readBlock);
    }
    return diff;
}

int CircularBlockBuffer::get_contiguous_write_blocks() const {
    if (_writeBlockNext == _readBlock) return 0;

    long diff = (long)(_readBlock - _writeBlockNext);

    if (diff < 0) {
        return (int)(_blockCount - _writeBlockNext);
    }
    return diff;
}

void CircularBlockBuffer::clear() {
    _readOffset = _writeOffsetCur;
    _readBlock = _writeBlockCur;
}

int CircularBlockBuffer::available_read() const {
    if (_readBlock == _writeBlockCur) return 0;

    long diff = (long)(_writeBlockCur - _readBlock);

    if (diff < 0) {
        return (int)(_blockCount + diff);
    }
    return diff;
}

int CircularBlockBuffer::available_write() const {
    if (_writeBlockNext == _readBlock)return 0;

    long diff = (long)(_readBlock - _writeBlockNext);

    if (diff < 0) {
        return (int)(_blockCount + diff);
    }
    return diff;
}


void CircularBlockBuffer::incrementWritePointer() {
    ++_writeBlockCur;
    _writeOffsetCur += _blockSize;

    ++_writeBlockNext;
    _writeOffsetNext += _blockSize;

    if (_writeBlockCur >= _blockCount) {
        _writeBlockCur = 0;
        _writeOffsetCur = 0;
    }

    if (_writeBlockNext >= _blockCount) {
        _writeBlockNext = 0;
        _writeOffsetNext = 0;
    }
}

void CircularBlockBuffer::incrementReadPointer() {
    ++_readBlock;
    _readOffset += _blockSize;

    if (_readBlock >= _blockCount) {
        _readBlock = 0;
        _readOffset = 0;
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
