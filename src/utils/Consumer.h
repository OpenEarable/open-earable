#ifndef CONSUMER_H
#define CONSUMER_H

#include "BufferedStream.h"

class Consumer {
public:
    virtual bool consume(int n) = 0;
//protected: // make protected in the end
    BufferedStream * stream;
};
#endif