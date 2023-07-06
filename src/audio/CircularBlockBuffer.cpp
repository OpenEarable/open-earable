#include "CircularBlockBuffer.h"

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
    _buffer = (uint8_t*)realloc(_buffer, _totalSize);

    memset(_buffer, 0x00, _totalSize);

    _readBlock = 0;
    _writeBlockCur = 0;
    _writeBlockNext = 1;

    _readOffset = 0;
    _writeOffsetCur = 0;
    _writeOffsetNext = _blockSize;
}

size_t CircularBlockBuffer::writeBlock(const uint8_t *buffer, size_t size) {
    if (size > _blockSize) {
        size = _blockSize;
    }

    if (size == 0) {
        return 0;
    }

    if (_collision) {
        _collision_error = true;
        return 0;
    }

    memcpy(&_buffer[_writeOffsetCur], buffer, size);

    incrementWritePointer();

    _collision_test();

    return size;
}

size_t CircularBlockBuffer::readBlock(uint8_t *buffer, size_t size) {
    if (!available()) {
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

    if (_collision) {
        _collision = false;
        _collision_error = false;
    }

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

void CircularBlockBuffer::clear() {
    _readOffset = _writeOffsetCur;
    _readBlock = _writeBlockCur;
}

int CircularBlockBuffer::available() const {
    if (_readBlock == _writeBlockCur) return 0;

    long diff = (long)(_writeBlockCur - _readBlock);

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

bool CircularBlockBuffer::check_collision_error() const {
    return _collision_error;
}

bool CircularBlockBuffer::check_collision() const {
    return _collision;
}

void CircularBlockBuffer::_collision_test() {
    if (_writeBlockNext == _readBlock) {
        _collision = true;
    }
}


