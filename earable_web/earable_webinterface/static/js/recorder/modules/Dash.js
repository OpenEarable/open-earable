import * as Recorder from './Recorder.js'

export class Dash {
    constructor(inter) {
        this.interface = inter;
        this.recorder = new Recorder.Recorder(inter);


        this.dashboard = new Dashboard(inter);

        this.register();
        this.set_default();

        this.init_configure_button();
    }

    register() {
        this.interface.event(this.data_receive, this);
        this.interface.connection_event(this.connection, this);
        this.interface.notify(this.notify, this);
    }

    set_default() {
        document.getElementById('sensorId').value = '0';
        document.getElementById('rate').value = '0';
        document.getElementById('latency').value = '0';
    }

    init_configure_button() {
        document.querySelector('#configureButton').addEventListener('click',
            _=> {
                let config = this.configure_form_button()
                this.configure(config);
            }
        );
    }

    configure_form_button() {
        let sensorId = parseInt(document.getElementById('sensorId').value);
        let sampleRate = parseFloat(document.getElementById('rate').value);
        let latency = parseInt(document.getElementById('latency').value);

        return [sensorId, sampleRate, latency]
    }

    configure(config) {
        this.interface.configure(config);
    }

    connection() {
        console.log("Sensor connected")
    }

    data_receive(data) {
        this.dashboard.data_receive(data);
    }

    notify(type, info) {
        this.dashboard.notify(type, info);
    }
}

class Dashboard {
    constructor() {
        this.connected = false;

        // UI elements
        this.BLEstatus = document.getElementById('connectionStatus');

        this.sensorMap = new Map();

        this.check_web_ble();
    }

    check_web_ble() {
        if (!("bluetooth" in navigator)) {
            this.notify("No_Web_BLE")
        }
    }

    msg(m){
        this.BLEstatus.innerHTML = m;
    }

    notify(type, info) {
        switch (type) {
            case "Requesting_BLE":
                this.msg('requesting device ...');
                break;
            case "Connecting_BLE":
                this.msg('connecting to device ...');
                break;
            case "Getting_Service":
                this.msg('getting primary service ...');
                break;
            case "Getting_Characteristics":
                this.msg('getting characteristics ...');
                break;
            case "Connected":
                this.connected = true;
                this.msg('connected.');
                this.BLEstatus.className = "badge rounded-pill bg-success";
                break;
            case "Error":
                this.msg('Error!');
                break;
            case "Disconnected":
                this.connected = false;
                this.msg('Device ' + info[0] + ' is disconnected.');
                this.BLEstatus.className = "badge rounded-pill bg-danger";
                break;
            case "No_Web_BLE":
                this.msg("browser not supported");
                alert("Error: This browser doesn't support Web Bluetooth. Try using Chrome.");
                break;
        }
    }

    data_receive(data) {
        let sensor, packet, parsedName, parsedValue;

        [sensor, packet] = data;
        parsedName = packet["name"];

        parsedValue = "";
        packet["data"].forEach(elem => {
            parsedValue += elem["type"] + ": " + elem["value"] + " ";
        });
        parsedValue += " Time: " + packet["time"];

        var table = document.getElementById("dataTable");
        // If sensor is already in the table -> update its value
        if (this.sensorMap.has(sensor)) {
            var rowIndex = this.sensorMap.get(sensor);
            var row = table.rows[rowIndex];
            var cell = row.cells[0];
            cell.innerHTML = sensor;
            cell = row.cells[1];
            cell.innerHTML = parsedName;
            cell = row.cells[2];
            cell.innerHTML = parsedValue;

            // If sensor is NOT in the table -> insert a new row filled with sensor data
        } else {
            var tableLength = table.rows.length;
            this.sensorMap.set(sensor, tableLength);
            var row = table.insertRow(tableLength);
            var cell = row.insertCell(0);
            cell.innerHTML = sensor;
            cell = row.insertCell(1);
            cell.innerHTML = parsedName;
            cell = row.insertCell(2);
            cell.innerHTML = parsedValue;
        }
    }
}


