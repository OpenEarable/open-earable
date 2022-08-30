//
// Created by Dylan Ray Roodt on 29.08.2022.
//

#ifndef EDGE_ML_EARABLE_BLECONFIG_H
#define EDGE_ML_EARABLE_BLECONFIG_H

const int DATA_FIXED_LENGTH = 120;

struct __attribute__((packed)) DataPacket {
    short size;
    short data[DATA_FIXED_LENGTH];
};

struct __attribute__((packed)) InfoPacket {
    short packages;
    short state;
};



#endif //EDGE_ML_EARABLE_BLECONFIG_H