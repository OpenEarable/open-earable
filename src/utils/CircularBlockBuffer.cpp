#include "CircularBlockBuffer.h"
#include "Arduino.h"

#define positiveOffset(p,n) (p + n >= _blockCount ? p + n - _blockCount : p + n)
#define negativeOffset(p,n) (p - n < 0 ? p - n + _blockCount : p - n)

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
    if (!blockSize || !blockCount) return;

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
    _writeBlock = 0;

    _overflow_count = 0;
    _underflow_count = 0;

    clear();
}

uint8_t * const CircularBlockBuffer::getWritePointer(int n) {
    return &(_buffer[positiveOffset(_writeBlock, n) * _blockSize]);
}

uint8_t * const CircularBlockBuffer::getReadPointer(int n) {
    return &(_buffer[positiveOffset(_readBlock, n) * _blockSize]);
}

int CircularBlockBuffer::get_contiguous_read_blocks() const {
    return min(available_read(), _blockCount - _readBlock);
}

int CircularBlockBuffer::get_contiguous_write_blocks() const {
    return min(available_write(), _blockCount - _writeBlock);
}

void CircularBlockBuffer::clear() {
    _readBlock = _writeBlock;
    _buffer_fill = 0;

    _reserve_write = 0;
    _reserve_read = 0;
    _reserve_write_total = 0;
    _reserve_read_total = 0;
}

int CircularBlockBuffer::available_read() const {
    return _buffer_fill - _reserve_write_total;
}

int CircularBlockBuffer::available_write() const {
    return _blockCount - _buffer_fill - _reserve_read_total;
}

// reserve <-- w --> n
// reserved Blocks are asynchronously written from the moment the method is called
void CircularBlockBuffer::incrementWritePointer(int n, bool blocking) {
    int _available = available_write();

    if (_available < n) {
        _overflow_count += n - _available;
    }
    
    _reserve_write_total = max(0, _reserve_write_total - _reserve_write);
    _reserve_write = blocking ? n : 0;
    _reserve_write_total += _reserve_write;

    // points to next available write Block
    _writeBlock = positiveOffset(_writeBlock, n);
    _buffer_fill += n;
}

void CircularBlockBuffer::incrementReadPointer(int n, bool blocking) {
    int _available = available_read();

    if (_available < n) {
        _underflow_count += n - _available;
    }

    _reserve_read_total = max(0, _reserve_read_total - _reserve_read);
    _reserve_read = blocking ? n : 0;
    _reserve_read_total += _reserve_read;

    // points to next available read Block
    _readBlock = positiveOffset(_readBlock, n);
    _buffer_fill -= n;
}

int CircularBlockBuffer::get_num_underflow() const {
    return _underflow_count;
}

int CircularBlockBuffer::get_num_overflow() const {
    return _overflow_count;
}

void CircularBlockBuffer::set_buffer(uint8_t *buffer, int blockSize, int blockCount) {
    _buffer = buffer;
    _blockSize = blockSize;
    _blockCount = blockCount;
    _totalSize = blockSize * blockCount;

    _external_buffer = true;
    reset();
}