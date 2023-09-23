#ifndef BUFFERED_STEAM_H
#define BUFFERED_STEAM_H

#include "CircularBlockBuffer.h"

class BufferedStream {
public:
    virtual void provide(int n) = 0;
    virtual void consume(bool blocking) = 0;
    virtual int remaining() = 0;
    virtual int ready() = 0;
    virtual int get_contiguous_blocks() = 0;
    CircularBlockBuffer buffer;
    void open() {
        _available = true;
    };
    void close() {
        _available = false;
    };
    bool available() {
        return _available;
    };
private:
    bool _available = false;
};
#endif