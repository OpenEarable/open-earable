#ifndef BUFFERED_INPUT_STEAM_H
#define BUFFERED_INPUT_STEAM_H

#include "BufferedStream.h"

class BufferedInputStream : public BufferedStream {
public:
    BufferedInputStream();
    ~BufferedInputStream();
    void provide(int n) override;
    void consume(bool blocking) override;
    int remaining() override;
    int ready() override;
    int get_contiguous_blocks() override;
};

#endif