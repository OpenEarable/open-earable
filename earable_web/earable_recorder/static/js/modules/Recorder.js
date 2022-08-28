import * as utils from "./utilities.js";


export class Recorder {
    constructor(inter) {
        this.interface = inter;

        this.data = {};
        this.on = false;

        this.register();
    }

    register() {
        this.interface.event(this.record, this);

        this.start_button = document.getElementById('startButton');
        this.start_button.addEventListener('click', function() {
            console.log("Start Rec");
            this.clear();
            this.start();
        }.bind(this));



        this.stop_button = document.getElementById('stopButton');

        this.stop_button.addEventListener('click', function() {
            console.log("Stop Rec");
            this.stop();
        }.bind(this));

        document.getElementById('downloadButton').addEventListener('click', function() {
            console.log("Download Rec");
            this.download();
        }.bind(this));
    }

    start() {
        this.on = true
    }

    stop() {
        this.on = false;
    }

    record(rec_data) {
        let sensor, parseData_obj, values, value_names;

        if (!this.on) {
            return;
        }

        [sensor, parseData_obj] = rec_data;

        if (this.data[sensor] === undefined) {
            value_names = [];
            values = {};

            parseData_obj["data"].forEach(element => {
                value_names.push(element.type);

                values[element.type] = [element.value];
            });

            if (parseData_obj["time"] !== undefined) {
                value_names.push("time");
                values["time"] = [parseData_obj["time"]];
            }

            this.data[sensor] = {
                name: parseData_obj["name"],
                values: values,
                value_names: value_names,
                count: 1
            };
        } else {
            parseData_obj["data"].forEach(element => {
                this.data[sensor]["values"][element.type].push(element.value);
            })
            if (parseData_obj["time"] !== undefined) {
                this.data[sensor]["values"]["time"].push(parseData_obj["time"]);
            }
            this.data[sensor]["count"] += 1;
        }
    }

    clear() {
        this.data = {};
    }

    download() {
        let csv_files, csv_data, name, line;

        csv_files = {};
        for (const [key, value] of Object.entries(this.data)) {
            csv_data = [];
            name = value["name"] + ".csv";

            csv_data.push(value["value_names"]);

            for (let i = 0; i < value["count"]; i++) {
                line = [];
                value["value_names"].forEach(element => {
                    line.push(value["values"][element][i]);
                })
                csv_data.push(line);
            }
            csv_files[name] = csv_data;
        }

        for (const [key, value] of Object.entries(csv_files)) {
            utils.exportToCsv(key, value);
        }
    }
}
