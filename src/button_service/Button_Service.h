#ifndef OPEN_EARABLE_BUTTON_SERVICE_H
#define OPEN_EARABLE_BUTTON_SERVICE_H

#include "ArduinoBLE.h"
#include "ble_config/ble_config_earable.h"

class Button_Service {
public:
    void begin();
    void write_state(int state);
private:
    BLEService * buttonService{};
    BLEUnsignedCharCharacteristic * buttonStateC{};
};

extern Button_Service button_service;


#endif //OPEN_EARABLE_BUTTON_SERVICE_H
