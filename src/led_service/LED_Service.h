#ifndef OPEN_EARABLE_LED_SERVICE_H
#define OPEN_EARABLE_LED_SERVICE_H

#include "ArduinoBLE.h"
#include "LED.h"
#include "ble_config/ble_config_earable.h"

class LED_Service {
public:
    void begin();

    void static receiveState(BLEDevice central, BLECharacteristic characteristic);
private:
    BLEService * _ledService{};
    BLECharacteristic * _ledSetStateC{};
};

extern LED_Service led_service;


#endif //OPEN_EARABLE_LED_SERVICE_H
