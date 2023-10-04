#include "LED_Service.h"
#include "Arduino.h"

void LED_Service::begin() {
    _ledService = new BLEService(ledServiceUuid);
    _ledSetStateC = new BLECharacteristic(ledSetStatusUuid, BLEWrite | BLERead, 3);

    BLE.setAdvertisedService(*_ledService);
    _ledService->addCharacteristic(*_ledSetStateC);
    BLE.addService(*_ledService);
    _ledSetStateC->setEventHandler(BLEWritten, receiveState);

    earable_led.init();
}

void LED_Service::receiveState(BLEDevice central, BLECharacteristic characteristic) {
    uint8_t data[3];
    characteristic.readValue(&data, sizeof(data));
    earable_led.set_color(data);
}



LED_Service led_service;
