#include "Play_Service.h"

WAVConfigurationPacket Play_Service::_current;
uint8_t Play_Service::_player_state = 0;
//BLECharacteristic * Play_Service::_wavPlayC_static;

void Play_Service::begin() {
    _wavPlayService = new BLEService(wavPlayServiceUuid);
    _wavPlayC = new BLECharacteristic(wavPlayUuid, BLEWrite|BLERead|BLENotify, sizeof(WAVConfigurationPacket));
    _playerStateC = new BLECharacteristic(playerStateUuid, BLEWrite|BLERead|BLENotify, 1);

    //_wavPlayC_static = _wavPlayC_static;

    BLE.setAdvertisedService(*_wavPlayService);
    _wavPlayService->addCharacteristic(*_wavPlayC);
    _wavPlayService->addCharacteristic(*_playerStateC);
    BLE.addService(*_wavPlayService);
    _wavPlayC->setEventHandler(BLEWritten, receiveWavConfig);
    _playerStateC->setEventHandler(BLEWritten, receivePlayerState);

    _current = audio_player.make_wav_config();
    _wavPlayC->writeValue(&_current, sizeof(WAVConfigurationPacket));

    _available = true;
}

void Play_Service::receiveWavConfig(BLEDevice central, BLECharacteristic characteristic) {
    characteristic.readValue(&_current, sizeof(_current));
    audio_player.ble_configuration(_current);

    Serial.println(String(_current.name, _current.size));

    //_wavPlayC_static->writeValue(&_current, sizeof(_current)); // not working yet... there is a weird problem
}

void Play_Service::receivePlayerState(BLEDevice central, BLECharacteristic characteristic) {
    characteristic.readValue(&_player_state, sizeof(_player_state));
    audio_player.set_state(_player_state);
}

void Play_Service::writePlayerState(uint8_t state) {
    if (!_available) return;

    _player_state = state;
    _playerStateC->writeValue(&_player_state, sizeof(_player_state));
}

Play_Service play_service;
