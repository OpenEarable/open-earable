#ifndef OPEN_EARABLE_PLAY_SERVICE_H
#define OPEN_EARABLE_PLAY_SERVICE_H

#include "ArduinoBLE.h"
#include "Audio_Player.h"

class Play_Service {
public:
    void begin();
private:
    BLEService * _wavPlayService{};
    BLECharacteristic * _wavPlayC{};

    void static receiveWavConfig(BLEDevice central, BLECharacteristic characteristic);
};

extern Play_Service play_service;


#endif //OPEN_EARABLE_PLAY_SERVICE_H
