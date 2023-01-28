#ifndef SENSORTYPES_EARABLE_H
#define SENSORTYPES_EARABLE_H

#include <cstdint>

const int ModuleActive_BUF = 5;

#define SENSOR_DATA_FIXED_LENGTH (12)

struct __attribute__((packed)) SensorConfigurationPacket {
    uint8_t sensorId;
    // sample rate is used also to enable/disable the sensor
    // 0 for disable, else for enable
    float sampleRate;
    // latency indicates how much ms time a new value is retained in its fifo
    // before a notification to the host is sent via interrupt
    // expressed in 24 bit
    uint32_t latency;
};

struct __attribute__((packed)) SensorDataPacket {
  uint8_t sensorId;
  uint8_t size;
  uint32_t millis;
  uint8_t data[SENSOR_DATA_FIXED_LENGTH];
};

struct PacketElement {
    bool ready;
    SensorConfigurationPacket data;
};

struct Sensor {
    // ID of sensor
    int ID;
    // State of sensor first checked
    bool state;
    // bool to track if sensor is actually active
    bool active;
    // delay in millisecond the inverse of the sample rate
    // rate (float) needs to be converted
    unsigned long delay;

    // last saved activation time
    unsigned long last;
};

struct ModuleActive {
    int count;
    bool active[ModuleActive_BUF];
    int map[ModuleActive_BUF];
    int module;
};

#endif //SENSORTYPES_EARABLE_H
