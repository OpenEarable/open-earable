#ifndef EDGE_ML_EARABLE_H
#define EDGE_ML_EARABLE_H

#include "BLEHandler_Earable.h"
#include "Sensor_Provider_Earable.h"

class Edge_ML_Earable {
public:
    Edge_ML_Earable();
    bool begin();
    void update();
    void debug(Stream &stream);
private:
    Stream *_debug;
};



extern Edge_ML_Earable edge_ml_earable;

#endif //EDGE_ML_EARABLE_H
