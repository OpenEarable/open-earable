import * as utils from "./utilities.js";


export class Recorder {
    constructor(inter) {
        this.interface = inter;

        this.data = {};
        this.on = false;

        this.rec_button = undefined;
        this.rec_indicator = undefined;

        this.register();
    }

    register() {
        this.interface.event(this.record, this);

        this.rec_button = document.getElementById('rec_Button');

        this.rec_button.addEventListener('click', function() {
            console.log("Rec Toggle");
            this.toggle();
        }.bind(this));

        document.getElementById('downloadButton').addEventListener('click', function() {
            console.log("Download Rec");
            this.download();
        }.bind(this));

        this.rec_indicator = document.getElementById('recButton');
    }

    toggle() {
        if (!this.on) {
            this.clear();
            this.start()
        } else {
            this.stop();
        }
    }

    start() {
        this.on = true

        this.rec_button.innerHTML = "Stop";
        utils.swap_html_class(this.rec_button, 'start_rec_col', 'stop_rec_col');
        utils.swap_html_class(this.rec_indicator, 'notRec', 'Rec');
    }

    stop() {
        this.on = false;

        this.rec_button.innerHTML = "Start";
        utils.swap_html_class(this.rec_indicator, 'Rec', 'notRec');
        utils.swap_html_class(this.rec_button, 'stop_rec_col', 'start_rec_col');
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
