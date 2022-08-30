//
// Created by Dylan Ray Roodt on 29.08.2022.
//

#ifndef EDGE_ML_EARABLE_BLESENDER_H
#define EDGE_ML_EARABLE_BLESENDER_H

#include "ArduinoBLE.h"
#include "BLEconfig.h"

class BLESender {
public:
    BLESender();
    bool begin();
    void end();
    void send_buffer(short* value, int size);
    void update();

    bool connected();
    bool check_sending();
    bool check_sending_complete();
private:
    int chunk_points = DATA_FIXED_LENGTH;       // points per chunk
    int chunk_bytes = chunk_points * 2;         // bytes per chunk
    int frequency = 3200;

    int package_total_count;
    int package_count;
    int package_size;
    int package_offset;
    short * package_array;

    unsigned long last = 0;
    unsigned long time_wait;

    bool sending_active = false;
    bool send_info_flag = false;

    bool completed = false;

    BLEService * service;
    BLECharacteristic * dataCharacteristic;
    BLECharacteristic * packageInfoCharacteristic;

    void send_next();
    void send_info();
    void check_activity();
    void finalize();
};


#endif //EDGE_ML_EARABLE_BLESENDER_H
