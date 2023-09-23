#ifndef OUTPUT_DEVICE_H
#define OUTPUT_DEVICE_H

#include "utils/Consumer.h"

class OutputDevice : public Consumer {
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    //BufferedInputStream * stream;
};

#endif