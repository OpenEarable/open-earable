#include "BufferedOutputStream.h"

BufferedOutputStream::BufferedOutputStream() {
    //buffer = new CircularBlockBuffer();
}

BufferedOutputStream::~BufferedOutputStream() {
    //delete buffer;
}

void BufferedOutputStream::provide(int n) {
    buffer.incrementReadPointer(n);
}

void BufferedOutputStream::consume(bool blocking) {
    buffer.incrementWritePointer(1,blocking);
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