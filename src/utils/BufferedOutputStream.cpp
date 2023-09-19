#include "BufferedOutputStream.h"

BufferedOutputStream::BufferedOutputStream() {
    //buffer = new CircularBlockBuffer();
}

BufferedOutputStream::~BufferedOutputStream() {
    //delete buffer;
}

void BufferedOutputStream::provide(int n) {
    for (int i = 0; i < n; i++) {
        buffer.incrementReadPointer();
    }
}

void BufferedOutputStream::consume(bool blocking) {
    buffer.incrementWritePointer();
}

int BufferedOutputStream::remaining() {
    return buffer.available_write();
}

int BufferedOutputStream::ready() {
    return buffer.available_read();
}

int BufferedOutputStream::get_contiguous_blocks() {
    return buffer.get_contiguous_read_blocks();
}