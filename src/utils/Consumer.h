#ifndef CONSUMER_H
#define CONSUMER_H

#include "BufferedStream.h"

/**
* Device that consumes a BufferedStream. Incase of an OutputStream it reads from the 
* Buffer. For an InputSream it writes to the Buffer.
*/
class Consumer {
public:
    virtual bool consume() = 0;
//protected: // TODO: make protected in the end
    BufferedStream * stream;
};
#endif