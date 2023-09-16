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

    void set_buffer(uint8_t * buffer, int blockSize, int blockCount);

    void setSizes(int blockSize, int blockCount);
    uint8_t * get_buffer();

    size_t getTotalSize() const;
    size_t getBlockSize() const;
    size_t getBlockCount() const;

    void reset();

    size_t writeBlock(const uint8_t *buffer, size_t size);
    size_t readBlock(uint8_t *buffer, size_t size);

    uint8_t * const getCurWritePointer();
    uint8_t * const getNextWritePointer();
    uint8_t * const getReadPointer();

    int get_contiguous_read_blocks() const;
    //int get_contiguous_write_blocks_cur() const;
    int get_contiguous_write_blocks() const;
    //int get_contiguous_write_blocks_next() const;

    void incrementWritePointer(int reserve = 0);
    void incrementReadPointer();

    int available_read() const; // Blocks available_read to read
    int available_write() const; // Blocks available_read to write

    void clear();

    bool check_collision_r_next() const;
    bool check_collision_r_cur() const;

    unsigned long get_r_collisions() const;
    unsigned long get_w_collisions() const;

private:
    bool _external_buffer = false;
    uint8_t* _buffer __attribute__((aligned (16)));

    size_t _blockCount{};
    size_t _blockSize{};
    int _totalSize{};

    bool _empty = true;
    //int _buffer_fill = 0;

    volatile unsigned int _readBlock{};
    volatile unsigned int _writeBlockCur{};
    volatile unsigned int _writeBlockNext{};

    /*volatile unsigned int _readOffset{};
    volatile unsigned int _writeOffsetCur{};
    volatile unsigned int _writeOffsetNext{};*/

    volatile unsigned long _collision_w_count = 0;
    volatile unsigned long _collision_r_count = 0;
};


#endif //TEST_CIRCULARBLOCKBUFFER_H
