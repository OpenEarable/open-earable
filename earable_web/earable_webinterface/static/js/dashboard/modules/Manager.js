import * as config from "../../config.js";
import * as Detect from "./Detect.js";

export class Manager {
    constructor(inter) {
        this.interface = inter;
        this.detect = new Detect.Detect(inter, config.SENS_Dashboard, config.EXCLUDE_DETECT);

        this.register();
    }

    register() {
        this.interface.notify(this.notify, this);
    }

    configure(config) {
        this.interface.configure(config);
    }

    notify(type, info) {
        switch (type) {
            case "Connected":
                this.start();
                break;
            case "Disconnected":
                this.stop();
                break;
        }
    }

    start() {
        config.SENS_Dashboard.forEach(element => {
            this.configure(element);
        });
    }

    stop() {
        config.SENS_Dashboard.forEach(element => {
            element = [element[0], 0];
            this.configure(element);
        });
    }
}