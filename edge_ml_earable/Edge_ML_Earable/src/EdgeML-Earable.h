#ifndef Edge_ML_BASE_EARABLE_H_
#define Edge_ML_BASE_EARABLE_H_

#warning "Using Ble33Nano Earable"
#include <earable/Edge_ML_Earable.h>

class Edge_ML {
    public:
        Edge_ML() {
            Edge_ML_Earable();
        }
        bool begin() {
            edge_ml_earable.begin();
        }
        void update() {
            edge_ml_earable.update();
        }
        void debug(Stream &stream) {
            edge_ml_earable.debug(stream);
        }
};

extern Edge_ML edge_ml;

#endif
