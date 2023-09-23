#ifndef BUFFERED_OUTPUT_STEAM_H
#define BUFFERED_OUTPUT_STEAM_H

#include "BufferedStream.h"

class BufferedOutputStream : public BufferedStream {
public:
    BufferedOutputStream();
    ~BufferedOutputStream();
    void provide(int n) override;
    void consume(bool blocking) override;
    int remaining() override;
    int ready() override;
    int get_contiguous_blocks() override;
};

#endif