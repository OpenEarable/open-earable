#include "Battery_Service.h"

auto batteryServiceUuid = "180F";
auto batteryUuid = "2A19";

void Battery_Service::begin() {
    battery = new Battery_Earable();
    battery->begin();

    _setup_ble_service();
}

void Battery_Service::update() {
    if (battery->check_battery()) {
        int battery_level = battery->get_battery_level();
        if (_debug) {
            _debug->print("Battery Level % is now: ");
            _debug->println(battery_level);
        }
        batteryLevelC->setValue(battery_level);
    }
}

void Battery_Service::_setup_ble_service() {
    batteryService = new BLEService(batteryServiceUuid);
    batteryLevelC = new BLEUnsignedCharCharacteristic(batteryUuid, BLERead | BLENotify);

    // Battery level
    BLE.setAdvertisedService(*batteryService);
    batteryService->addCharacteristic(*batteryLevelC);
    BLE.addService(*batteryService);
    batteryLevelC->setValue(0);
}
