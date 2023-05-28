#ifndef TEST_CIRCULARBLOCKBUFFER_H
#define TEST_CIRCULARBLOCKBUFFER_H


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

    void setSizes(int blockSize, int blockCount);

    size_t getTotalSize() const;
    size_t getBlockSize() const;
    size_t getBlockCount() const;

    void reset();

    size_t writeBlock(const uint8_t *buffer, size_t size);
    size_t readBlock(uint8_t *buffer, size_t size);

    uint8_t * getCurWritePointer();
    uint8_t * getNextWritePointer();
    uint8_t * getReadPointer();

    void incrementWritePointer();
    void incrementReadPointer();

    int available() const; // Blocks available to read

    void clear();

    bool check_collision_error() const;
    bool check_collision() const;
private:
    uint8_t* _buffer __attribute__((aligned (16)));

    size_t _blockCount{};
    size_t _blockSize{};
    int _totalSize{};

    volatile unsigned int _readBlock{};
    volatile unsigned int _writeBlockCur{};
    volatile unsigned int _writeBlockNext{};

    volatile unsigned int _readOffset{};
    volatile unsigned int _writeOffsetCur{};
    volatile unsigned int _writeOffsetNext{};

    volatile bool _collision = false;
    volatile bool _collision_error = false;

    void _collision_test();
};


#endif //TEST_CIRCULARBLOCKBUFFER_H
