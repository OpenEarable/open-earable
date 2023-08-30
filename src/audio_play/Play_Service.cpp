#include "Play_Service.h"


auto wavPlayServiceUuid = "5669146e-476d-11ee-be56-0242ac120002";
auto wavPlayUuid = "566916a8-476d-11ee-be56-0242ac120002";


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
