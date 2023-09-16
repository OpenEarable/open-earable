#include "Play_Service.h"

WAVConfigurationPacket Play_Service::_current;
BLECharacteristic * Play_Service::_wavPlayC_static;

void Play_Service::begin() {
    _wavPlayService = new BLEService(wavPlayServiceUuid);
    _wavPlayC = new BLECharacteristic(wavPlayUuid, BLEWrite|BLERead, sizeof(WAVConfigurationPacket));

    _wavPlayC_static = _wavPlayC_static;

    BLE.setAdvertisedService(*_wavPlayService);
    _wavPlayService->addCharacteristic(*_wavPlayC);
    BLE.addService(*_wavPlayService);
    _wavPlayC->setEventHandler(BLEWritten, receiveWavConfig);

    //TODO: chang back
    //_current = audio_player.make_wav_config();
    //_wavPlayC->writeValue(&_current, sizeof(WAVConfigurationPacket));
}

void Play_Service::receiveWavConfig(BLEDevice central, BLECharacteristic characteristic) {
    characteristic.readValue(&_current, sizeof(_current));
    audio_player.ble_configuration(_current);

    Serial.println(String(_current.name, _current.size));

    //_wavPlayC_static->writeValue(&_current, sizeof(_current)); // not working yet... there is a weird problem
}

Play_Service play_service;
