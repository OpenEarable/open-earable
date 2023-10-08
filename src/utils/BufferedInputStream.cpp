#include "BufferedInputStream.h"

BufferedInputStream::BufferedInputStream() {
    //buffer = new CircularBlockBuffer();
}

BufferedInputStream::~BufferedInputStream() {
    //delete buffer;
}

void BufferedInputStream::provide(int n) {
    buffer.incrementWritePointer(n);
}

void BufferedInputStream::consume(bool blocking) {
    buffer.incrementReadPointer(1,blocking);
}

int BufferedInputStream::remaining() {
    return buffer.available_read();
}

int BufferedInputStream::ready() {
    return buffer.available_write();
}

int BufferedInputStream::get_contiguous_blocks() {
    return buffer.get_contiguous_write_blocks(); // Use cur block
}