#include "Button_Service.h"

void Button_Service::begin() {
    buttonService = new BLEService(buttonServiceUuid);
    buttonStateC = new BLEUnsignedCharCharacteristic(buttonStateUuid, BLERead | BLENotify);

    // Battery level
    BLE.setAdvertisedService(*buttonService);
    buttonService->addCharacteristic(*buttonStateC);
    BLE.addService(*buttonService);
    buttonStateC->setValue(0);
}

void Button_Service::write_state(int state) {
    buttonStateC->writeValue(state);
}

Button_Service button_service;
