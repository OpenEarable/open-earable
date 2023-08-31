#include "Play_Service.h"

void Play_Service::begin() {
    _wavPlayService = new BLEService(wavPlayServiceUuid);
    _wavPlayC = new BLECharacteristic(wavPlayUuid, BLEWrite, sizeof(WAVConfigurationPacket));

    BLE.setAdvertisedService(*_wavPlayService);
    _wavPlayService->addCharacteristic(*_wavPlayC);
    BLE.addService(*_wavPlayService);
    _wavPlayC->setEventHandler(BLEWritten, receiveWavConfig);
}

void Play_Service::receiveWavConfig(BLEDevice central, BLECharacteristic characteristic) {
    WAVConfigurationPacket data;
    characteristic.readValue(&data, sizeof(data));
    audio_player.ble_configuration(data);
}

Play_Service play_service;
