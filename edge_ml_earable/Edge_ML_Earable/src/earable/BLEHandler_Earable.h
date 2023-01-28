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

    void check_battery();
    void update_battery();

    unsigned long _last = -1;
    unsigned long _battery_interval = 1000;

    int _battery_pin = A0;
    int _old_battery_level = 0;

    int _battery_offset_min = 734;
    int _battery_offset_max = 1023;
};

extern BLEHandler_Earable bleHandler_earable;

#endif //BLEHANDLER_EARABLE_H
