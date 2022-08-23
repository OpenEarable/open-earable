//
// Created by Dylan Ray Roodt on 23.05.2022.
//

#ifndef BLEHANDLER_EARABLE_H
#define BLEHANDLER_EARABLE_H

#include "Arduino.h"
#include "ArduinoBLE.h"
#include "sensors/SensorTypes_Earable.h"

const int BUFFER_SIZE = 5;

class BLEHandler_Earable {
public:
    BLEHandler_Earable();
    bool begin();
    void update();

    void send(int ID, int *data);
    void send(int ID, float *data);

    void poll(unsigned long timeout);
    void end();

    static void debug(Stream &stream);

    bool bleActive = false;

private:
    static Stream *_debug;
    void static receivedSensorConfig(BLEDevice central, BLECharacteristic characteristic);
    void write_int16_at_pos(int16_t value, uint8_t *data, int pos);
    void write_float_at_pos(float value, uint8_t *data, int pos);
};

extern BLEHandler_Earable bleHandler_earable;

#endif //BLEHANDLER_EARABLE_H
