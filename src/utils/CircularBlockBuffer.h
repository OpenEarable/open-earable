#ifndef CIRCULAR_BLOCKBUFFER_H
#define CIRCULAR_BLOCKBUFFER_H


#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CBB_TOTAL_COUNT 10
#define DEFAULT_CBB_BLOCK_SIZE 1024

class CircularBlockBuffer {
public:
    CircularBlockBuffer();
    explicit CircularBlockBuffer(int blockSize, int totalSize);
    ~CircularBlockBuffer();

    void set_buffer(uint8_t * buffer, int blockSize, int blockCount);

    void setSizes(int blockSize, int blockCount);
    uint8_t * get_buffer();

    size_t getTotalSize() const;
    size_t getBlockSize() const;
    size_t getBlockCount() const;

    void reset();
    void clear();

    uint8_t * const getWritePointer(int n = 0);
    uint8_t * const getReadPointer(int n = 0);

    int get_contiguous_read_blocks() const;
    int get_contiguous_write_blocks() const;

    void incrementWritePointer(int n = 1, bool blocking = false);
    void incrementReadPointer(int n = 1, bool blocking = false);

    int available_read() const; // Blocks available to read until underflow
    int available_write() const; // Blocks available to write until overflow

    int get_num_underflow() const;
    int get_num_overflow() const;

private:
    bool _external_buffer = false;
    uint8_t* _buffer __attribute__((aligned (16)));

    size_t _blockCount{};
    size_t _blockSize{};
    int _totalSize{};

    volatile int _readBlock{};
    volatile int _writeBlock{};

    volatile int _reserve_write = 0;
    volatile int _reserve_read = 0;
    volatile int _reserve_write_total = 0;
    volatile int _reserve_read_total = 0;

    volatile int _underflow_count = 0;
    volatile int _overflow_count = 0;

    volatile int _buffer_fill = 0;
};


#endif //CIRCULAR_BLOCKBUFFER_H
