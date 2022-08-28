import * as config from "../config.js";


export class Manager {
    constructor(inter) {
        this.interface = inter;

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
        config.SENS.forEach(element => {
            this.configure(element);
        });
    }

    stop() {
        config.SENS.forEach(element => {
            element = [element[0], 0];
            this.configure(element);
        });
    }
}