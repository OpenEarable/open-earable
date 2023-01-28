#include "BLEHandler_Earable.h"

#include "Arduino.h"
#include "ArduinoBLE.h"
#include "Sensor_Provider_Earable.h"
#include <cstdint>

const char deviceIdentifier_earable[] = "Earable";
const char deviceGeneration_earable[] = "2.0.0";

// Sensor Data channels
BLEService sensorService_earable("34c2e3bb-34aa-11eb-adc1-0242ac120002");
auto sensorDataUuid_earable = "34c2e3bc-34aa-11eb-adc1-0242ac120002";
auto sensorConfigUuid_earable = "34c2e3bd-34aa-11eb-adc1-0242ac120002";
BLECharacteristic sensorDataCharacteristic_earable(sensorDataUuid_earable, (BLERead | BLENotify), sizeof(SensorDataPacket));
BLECharacteristic sensorConfigCharacteristic_earable(sensorConfigUuid_earable, BLEWrite, sizeof(SensorConfigurationPacket));

// Device information channels
BLEService deviceInfoService_earable("45622510-6468-465a-b141-0b9b0f96b468");
auto deviceIdentifierUuid_earable = "45622511-6468-465a-b141-0b9b0f96b468";
auto deviceGenerationUuid_earable = "45622512-6468-465a-b141-0b9b0f96b468";
BLECharacteristic deviceIdentifierCharacteristic_earable(deviceIdentifierUuid_earable, BLERead, sizeof(deviceIdentifier_earable) + 1);
BLECharacteristic deviceGenerationCharacteristic_earable(deviceGenerationUuid_earable, BLERead, sizeof(deviceGeneration_earable) + 1);

Stream* BLEHandler_Earable::_debug = nullptr;

BLEHandler_Earable::BLEHandler_Earable() {
}

// Sensor channel
void BLEHandler_Earable::receivedSensorConfig(BLEDevice central, BLECharacteristic characteristic) {
    SensorConfigurationPacket data;
    characteristic.readValue(&data, sizeof(data));
    if (_debug) {
        _debug->println("configuration received: ");
        _debug->print("data: ");
        _debug->println(data.sensorId);
        _debug->println(data.sampleRate);
        _debug->println(data.latency);
    }

    sensor_provider_earable.configureSensor(data);
}


bool BLEHandler_Earable::begin() {
    if (!BLE.begin()) {
        if (_debug) _debug->println("BLE already active");

        return false;
    }
    bleActive = true;

    // Code for name
    String address = BLE.address();
    String name;
    int length;

    address.toUpperCase();
    length = address.length();

    name = (String)deviceIdentifier_earable + "-";
    name += address[length - 5];
    name += address[length - 4];
    name += address[length - 2];
    name += address[length - 1];

    BLE.setLocalName(name.c_str());
    BLE.setDeviceName(name.c_str());

    if (_debug) {
        _debug->println("BLEHandler_Earable Begin: ");
        _debug->print("address = ");
        _debug->println(address);
        _debug->print("name = ");
        _debug->println(name);
    }

    // Sensor channel
    BLE.setAdvertisedService(sensorService_earable);
    sensorService_earable.addCharacteristic(sensorConfigCharacteristic_earable);
    sensorService_earable.addCharacteristic(sensorDataCharacteristic_earable);
    BLE.addService(sensorService_earable);
    sensorConfigCharacteristic_earable.setEventHandler(BLEWritten, receivedSensorConfig);

    // Device information
    BLE.setAdvertisedService(deviceInfoService_earable);
    deviceInfoService_earable.addCharacteristic(deviceIdentifierCharacteristic_earable);
    deviceInfoService_earable.addCharacteristic(deviceGenerationCharacteristic_earable);
    BLE.addService(deviceInfoService_earable);
    deviceIdentifierCharacteristic_earable.writeValue(deviceIdentifier_earable);
    deviceGenerationCharacteristic_earable.writeValue(deviceGeneration_earable);
    BLE.advertise();
    return true;
}

void BLEHandler_Earable::end() {
    if (_debug) _debug->println("BLE End");
    bleActive = false;
    BLE.end();
}

void BLEHandler_Earable::update() {
    BLE.poll();
}

void BLEHandler_Earable::send(int ID, int *data) {
    // send list of int data as in int16 2 bytes each
    // first element is length of array
    if (sensorDataCharacteristic_seeed.subscribed()) {
        SensorDataPacket package{};
        int16_t value;
        int length = data[0];
        package.sensorId = ID;
        package.size = 2 + 4 + length * 2;
        package.millis = millis();

        for (int i=0; i<length; i++) {
            value = (int16_t)data[i + 1];
            write_int16_at_pos(value, package.data, i * 2);
        }

        sensorDataCharacteristic_earable.writeValue(&package, sizeof(SensorDataPacket));
    }
}

void BLEHandler_Earable::send(int ID, float *data) {
    // send list of float data floats 4 bytes each
    // first element is length of array (just convert to int)
    if (sensorDataCharacteristic_earable.subscribed()) {
        SensorDataPacket package{};
        int length = (int)data[0];
        package.sensorId = ID;
        package.size = 2 + 4 + length * 4;
        package.millis = millis();

        for (int i=0; i<length; i++) {
            write_float_at_pos(data[i + 1], package.data, i * 4);
        }

        sensorDataCharacteristic_earable.writeValue(&package, sizeof(SensorDataPacket));
    }
}

void BLEHandler_Earable::poll(unsigned long timeout) {
    BLE.poll(timeout);
}

void BLEHandler_Earable::write_int16_at_pos(int16_t value, uint8_t *data, int pos) {
    data[pos] = value & 0x000000ff;
    data[pos+1] = (value & 0x0000ff00) >> 8;
}

void BLEHandler_Earable::write_float_at_pos(float value, uint8_t *data, int pos) {
    int length = sizeof(float);
    for(int i = 0; i < length; i++){
        data[pos+i] = ((uint8_t*)&value)[i];
    }
}

void BLEHandler_Earable::debug(Stream &stream) {
    _debug = &stream;
    //BLE.debug(stream); // Problems with Debug
}

BLEHandler_Earable bleHandler_earable;
