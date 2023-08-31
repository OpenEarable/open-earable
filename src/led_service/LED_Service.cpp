#include "LED_Service.h"

void LED_Service::begin() {
    _ledService = new BLEService(ledServiceUuid);
    _ledSetStateC = new BLECharacteristic(ledSetStatusUuid, BLEWrite, 1);

    BLE.setAdvertisedService(*_ledService);
    _ledService->addCharacteristic(*_ledSetStateC);
    BLE.addService(*_ledService);
    _ledSetStateC->setEventHandler(BLEWritten, receiveWavConfig);
}

void LED_Service::receiveState(BLEDevice central, BLECharacteristic characteristic) {
    uint8_t data;
    characteristic.readValue(&data, sizeof(data));
    earable_led.set_color((Color)data);
}



LED_Service button_service;
