//
// Created by Dylan Ray Roodt on 29.08.2022.
//

#include "BLESender.h"


const char* deviceIdentifier = "Earable_Audio";

const auto serviceUuid = "0001";
const auto dataUuid = "0002";
const auto infoUuid = "0003";

BLESender::BLESender() {
    service = new BLEService(serviceUuid);
    dataCharacteristic = new BLECharacteristic(dataUuid,  (BLERead | BLENotify), sizeof(DataPacket));
    packageInfoCharacteristic = new BLECharacteristic(infoUuid,  (BLERead | BLENotify), sizeof(package_count) * 2);
}

bool BLESender::begin() {
    if (!BLE.begin()) {
        return false;
    }
    String address = BLE.address();
    String name;
    int length;

    address.toUpperCase();
    length = (int)address.length();
    name = (String)deviceIdentifier + "-" + address.substring(length - 5,length - 3) + address.substring(length - 2,length);

    BLE.setLocalName(name.c_str());
    BLE.setDeviceName(name.c_str());
    BLE.setAdvertisedService(*service);
    service->addCharacteristic(*dataCharacteristic);
    service->addCharacteristic(*packageInfoCharacteristic);
    BLE.addService(*service);

    // Start Advertising
    BLE.advertise();
    return true;
}

void BLESender::send_buffer(short *value, int size) {
    package_size = size;
    package_array = value;
    package_total_count = package_size / chunk_bytes + 1;

    time_wait = (unsigned long)(chunk_points * (1000. / frequency)) + 5;

    sending_active = true;
    package_offset = 0;
    package_count = 0;
    completed = false;

    send_info_flag = true;
    send_info();
    send_next();
    last = millis();
}

void BLESender::send_info() {
    if (!packageInfoCharacteristic->subscribed()) return;

    InfoPacket data{};

    data.packages = (short)package_total_count;
    data.state = (short)completed;

    packageInfoCharacteristic->writeValue(&data, sizeof(data));
    send_info_flag = false;
}

void BLESender::send_next() {
    if (!dataCharacteristic->subscribed()) return;

    if (package_count >= package_total_count) {
        finalize();
        return;
    }

    DataPacket data{};
    
    int range;
    if (package_offset > package_size - chunk_bytes) {
        range = (package_size - package_offset);
        data.size = (short)(range);
        range /= 2;
        for (int i = 0; i < range; i++) {
            data.data[i] = package_array[i + package_count * chunk_points];
        }
    } else {
        data.size = (short) chunk_bytes;
        for (int i = 0; i < chunk_points; i++) {
            data.data[i] = package_array[i + package_count * chunk_points];
        }

        package_offset += chunk_bytes;
    }

    dataCharacteristic->writeValue(&data, sizeof(data));
    package_count++;
}

void BLESender::check_activity() {
    if (send_info_flag) {
        send_info();
    }

    if (sending_active) {
        unsigned long time = millis();
        if (time - last >= time_wait) {
            send_next();
            last = time;
        }
    }
}

void BLESender::finalize() {
    completed = true;
    send_info_flag = true;
    sending_active = false;
}

bool BLESender::check_sending() {
    return sending_active;
}

void BLESender::update() {
    check_activity();
    BLE.poll();
}

void BLESender::end() {
    BLE.end();
}

bool BLESender::connected() {
    return dataCharacteristic->subscribed();
}

bool BLESender::check_sending_complete() {
    return completed;
}
