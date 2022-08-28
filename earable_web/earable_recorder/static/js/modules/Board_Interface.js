import * as utils from "./utilities.js";
import * as config from "../config.js";

const sensorServiceUuid = '34c2e3bb-34aa-11eb-adc1-0242ac120002';
const sensorConfigCharacteristicUuid = '34c2e3bd-34aa-11eb-adc1-0242ac120002';
const sensorDataCharacteristicUuid = '34c2e3bc-34aa-11eb-adc1-0242ac120002';

var BLE_filters = [
    {namePrefix: config.deviceName}
]

var TIMESTAMPED = config.timestamped;

var parseScheme;
var sensorTypes;

var path_parseScheme = "config/parse-scheme.json";
var path_sensorTypes = "config/sensor-type-map.json";


fetch(path_parseScheme)
    .then(response => response.json())
    .then(json => {
        console.log(json);
        parseScheme = json;
    });

//fetch("./sensor-type.json")

fetch(path_sensorTypes)
    .then(response => response.json())
    .then(json => {
        console.log(json);
        sensorTypes = json;
    });


export class Board_Interface {
    constructor(button_id) {
        this.bleDevice = undefined;
        this.bleServer = undefined;
        this.sensorService = undefined;
        this.sensorConfigCharacteristic = undefined;
        this.sensorDataCharacteristic = undefined;

        this.sensorMap = new Map();

        this.ready = false;
        this.sender_queue = [];
        this.sending = false;

        this.events = [this.receiveSensorData];
        this.events_parsed = [];
        //this.events_parsed = [this.show_data.bind(this)];

        this.notify_event = [];
        this.connection_events = [];

        this.init_connection_button(button_id);
    }

    init_connection_button(button_id) {
        let button = document.getElementById(button_id);

        button.addEventListener('click', function() {
            if (utils.isWebBluetoothEnabled()) {
                this.connect_to_nicla()
                    .then(_ => {
                        console.log('Connected');
                        this.connection_events.forEach(event => event());
                    })
                    .catch(error => {
                        this.notify("Error");
                        console.log('ERROR: ' + error);
                    });
            }
        }.bind(this));
    }

    send_configure(config) {
        let sensorId = config[0];
        let sampleRate = config[1];
        let latency = config[2];


        let configPacket = new Uint8Array(9);
        configPacket[0] = sensorId;
        configPacket.set(utils.floatToBytes(sampleRate), 1);
        configPacket.set(utils.intToBytes(latency), 5);

        this.sender_queue.push(configPacket);

        if (this.sending) {
            return;
        }
        this.sending = true;
        this.sender();
    }

    sender() {
        let configPacket;

        if (this.sender_queue.length > 0) {
            configPacket = this.sender_queue.shift();
            this.single_send(configPacket).then(_ => {
                console.log('Configuration written');
                this.sender();
            }).catch(_ => {
                console.log('Configuration failed');
                this.sender();
            });
        } else {
            this.sending = false;
            this.notify("Configuration_Written");
        }
    }

    single_send(configPacket) {
        return this.sensorConfigCharacteristic.writeValue(configPacket)
    }

    connect_to_nicla() {
        return this.getDeviceInfo()
            .then(this.connectDevice.bind(this))
            .then(this.getSensorCharacteristics.bind(this))
            .then(this.onConnection.bind(this));
    }

    // Important!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    getDeviceInfo() {
        let options = {
            filters: BLE_filters,
            optionalServices: [sensorServiceUuid]
        };
        console.log('Requesting BLE device info...');

        this.notify("Requesting_BLE");

        return navigator.bluetooth.requestDevice(options).then(device => {
            this.bleDevice = device;
        }).catch(error => {
            console.log('Request device error: ' + error);
            this.notify("Error");
        });
    }

    connectDevice() {
        console.log('Connecting to device');
        this.notify("Connecting_BLE");
        this.bleDevice.addEventListener('gattserverdisconnected', this.onDisconnection.bind(this));
        return this.bleDevice.gatt.connect()
            .then(server => {
                this.bleServer = server;
                this.notify("Getting_Service");
                return this.bleServer.getPrimaryService(sensorServiceUuid);
            })
            .then(service => {
                this.sensorService = service;
            });
    }

    onConnection() {
        this.ready = true;
        this.notify("Connected");
    }

    onDisconnection(event) {
        this.ready = false;
        this.notify("Disconnected", [event.target.name]);
    }

    getSensorCharacteristics() {
        console.log('Getting sensor characteristics');
        this.notify("Getting_Characteristics");
        return this.sensorService.getCharacteristic(sensorConfigCharacteristicUuid)
            .then(characteristic => {
                this.sensorConfigCharacteristic = characteristic;
            })
            .then(_ => {
                return this.sensorService.getCharacteristic(sensorDataCharacteristicUuid);
            })
            .then(characteristic => {
                this.sensorDataCharacteristic = characteristic;
                this.sensorDataCharacteristic.startNotifications();

                this.events.forEach(event => this.add_event(event));
            });
    }

    add_event(event) {
        this.sensorDataCharacteristic.addEventListener('characteristicvaluechanged', event.bind(this));
    }

    generate_string_from_data(data) {
        let result = "";

        data.forEach(point => {
            result += point.type + ": " + point.value + "   ";
        });
        return result;
    }

    receiveSensorData(event) {
        // data is [sensor, parsedData_obj]
        let data = this.parser(event);
        this.events_parsed.forEach(event => event(data));

        // Previous table logic
        // This could be put into events_parsed
    }

    parseData(sensor, data) {
        let millis;
        var type = sensorTypes[sensor].type;
        var sensorName = sensorTypes[sensor].name;
        var scheme = parseScheme["types"][type]["parse-scheme"];

        var data_obj = {
            "name": sensorName,
            "type": type,
            "data":[],
            "time": undefined
        }

        // dataIndex start from 2 because the first bytes of the packet indicate
        // the sensor id and the data size
        var dataIndex = 0 + 2;

        if (TIMESTAMPED) {
            millis = data.getUint16(dataIndex, true) + (data.getUint16(dataIndex+2, true) << 16);
            dataIndex += 4;
            data_obj["time"] = millis;
        }


        scheme.forEach(element => {
            var name = element['name'];
            var valueType = element['type'];
            var scale = element['scale-factor'];
            var value = 0;
            var size = 0;

            if (valueType == "uint8") {
                value = data.getUint8(dataIndex, true) * scale;
                size = 1;
            } else if (valueType == "uint24") {
                value = data.getUint16(dataIndex, true) + (data.getUint8(dataIndex+2, true) << 16);
                value *= scale;
                size = 3;
            } else if (valueType == "uint32") {
                value = data.getUint16(dataIndex, true) + (data.getUint16(dataIndex+2, true) << 16);
                size = 4;
            } else if (valueType == "int16") {
                value = data.getInt16(dataIndex, true) * scale;
                size = 2;
            } else if (valueType == "float") {
                value = data.getFloat32(dataIndex, true) * scale;
                value = value.toFixed(2)
                size = 4;
            } else {
                console.log("Error: unknown type");
            }
            
            var point = {
                "type": element.name,
                "value": value
            }
            
            data_obj["data"].push(point);
            dataIndex += size;
        });
        return data_obj;
    }

    parser(event) {
        let value, sensor, size, parseData_obj;
        value = event.target.value;
        // Get sensor data
        sensor = value.getUint8(0);
        size = value.getUint8(1);
        parseData_obj = this.parseData(sensor, value);
        return [sensor, parseData_obj];
    }

    notify(type, info) {
        /*
            Notifications:
            Requesting_BLE
            Connecting_BLE
            Getting_Service
            Getting_Characteristics
            Connected
            Configuration_Written
            Error
            Disconnected

            (No_Web_BLE)
         */

        this.notify_event.forEach(event => event(type, info));
    }

    show_data(data) {
        let parsedName, parsedValue;
        let [sensor, parsedData_obj] = data;

        parsedValue = this.generate_string_from_data(parsedData_obj["data"]);
        parsedName = parsedData_obj["name"];

        console.log(parsedName, parsedValue);
    }

    // USE
    send_custom_configure(config) {
        if (!this.ready) {
            console.log("Not ready");
            return;
        }

        if (!config) {
            return;
        }
        if (config.length !== 3) {
            if (config.length === 2) {
                config = [config[0], config[1], 0];
            } else {
                return;
            }
        }
        this.send_configure(config);
    }

    // USE
    add_custom_event(func, parsed=true) {
        if (parsed) {
            if (!this.events_parsed.includes(func)) {
                this.events_parsed.push(func);
            }
        } else {
            if (!this.events.includes(func)) {
                this.events.push(func);
            }
        }
    }

    // USE
    remove_custom_event(func) {
        if (!this.events_parsed.includes(func)) {
            this.events_parsed = this.events_parsed.filter(item => item !== func);
        }
        if (!this.events.includes(func)) {
            this.events.push(func);
            this.events = this.events.filter(item => item !== func);
        }
    }

    // USE
    add_custom_connection_event(func) {
        if (!this.connection_events.includes(func)) {
            this.connection_events.push(func);
        }
    }

    // USE
    add_notify_event(func) {
        if (!this.notify_event.includes(func)) {
            this.notify_event.push(func);
        }
    }
}
