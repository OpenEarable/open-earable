#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include "utils/Consumer.h"

class InputDevice : public Consumer {
public:
    virtual void start() = 0;
    virtual void stop() = 0;
};

#endif