#ifndef PROVIDER_H
#define PROVIDER_H

#include "BufferedStream.h"

class Provider {
public:
    virtual int provide(int n) = 0;
    virtual bool begin() = 0;
    virtual bool available() = 0;
    virtual void end() = 0;
    virtual void setStream(BufferedStream ** stream) = 0;
//protected:
    BufferedStream ** stream;
};
#endif