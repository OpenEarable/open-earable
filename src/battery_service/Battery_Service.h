#ifndef EDGE_ML_EARABLE_BATTERY_SERVICE_H
#define EDGE_ML_EARABLE_BATTERY_SERVICE_H

#include "ArduinoBLE.h"
#include "Battery_Earable.h"
#include "ble_config/ble_config_earable.h"
#include <generic/Debug.h>

class Battery_Service : public Debug {
public:
    void begin();
    void update();
private:
    Battery_Earable * battery;

    BLEService * batteryService;
    BLEUnsignedCharCharacteristic * batteryLevelC;
    BLEUnsignedCharCharacteristic * chargingStateC;

    void _setup_ble_service();
};


#endif //EDGE_ML_EARABLE_BATTERY_SERVICE_H
