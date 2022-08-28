import * as utils from "./utilities.js";

export class Detect {
    constructor(inter,config, exclude) {
        this.interface = inter;
        this.config = config;
        this.exclude = exclude;
        this.uncertainty = 0.5;

        this.sensors = {};
        this.released = false;

        this.alert_element = document.getElementById('alert');
        this.alert_element_c = document.getElementById('alert_c');
        this.alert_animation = "fade-out";
        this.animation_on = false;

        this.register();
    }

    register() {
        this.interface.event(this.data_receive, this);

        this.config.forEach(element => {
            let [id, rate] = element;
            if (this.exclude.includes(id)) {
                return;
            }
            this.sensors[id] = new Sensor(id, rate, this.uncertainty);
        });

        this.alert_element.addEventListener('animationend', () => {
            this.alert_callback();
        });

        this.alert_element_c.addEventListener('click', () => {
            this.alert_callback();
        });

        this.interface.notify(this.notify, this);
    }

    notify(type, info) {
        switch (type) {
            case "Connected":
                this.reset_all();
                break;
            case "Configuration_Written":
                this.release();
                break;
        }
    }

    data_receive(data) {
        let sensor;

        let [id, values] = data;

        if (this.exclude.includes(id)) {
            return;
        }

        sensor = this.sensors[id];

        if (sensor === undefined) {
            return;
        }

        sensor.tick(values["time"]);

        if (!this.released) {
            return;
        }

        if (sensor.check()) {
            this.alert();
        }
    }

    alert() {
        if (this.animation_on) {
            this.animation_on = false;

            this.alert_element.style.animation = 'none';
            this.alert_element.offsetHeight; /* trigger reflow */
            this.alert_element.style.animation = null;
            return;
        }

        this.alert_element.style.visibility = "visible" ;
        this.alert_element.style.opacity = "1";
        utils.add_html_class(this.alert_element, this.alert_animation);

        this.animation_on = true;
    }

    alert_callback() {
        this.alert_element.style.visibility = "hidden" ;
        utils.remove_html_class(this.alert_element, this.alert_animation);
        this.animation_on = false;
    }

    reset_all() {
        for (const [key, value] of Object.entries(this.sensors)) {
            value.reset();
        }
    }

    release() {
        this.released = true;
    }
}

class Sensor {
    constructor(sensor, rate, uncertainty) {
        this.sensor = sensor;
        this.rate = rate;
        this.uncertainty = uncertainty;

        this.buffer_size = 3;
        this.timebuffer = [];
        this.deltas = [];

        this.fresh = true;

        this.delay = (1000 / this.rate) * (1 + this.uncertainty);
    }

    reset() {
        this.timebuffer = [];
        this.deltas = [];
        this.fresh = true;
    }

    tick(time) {
        this.timebuffer.fixedAdd(time, this.buffer_size);
    }

    compute_delta() {
        let previous;
        this.deltas = [];
        previous = undefined;
        this.timebuffer.forEach((item, index) => {
            if (previous === undefined) {
                previous = item;
                return;
            }
            this.deltas.push(item - previous);
            previous = item;
        });
    }

    check() {
        let compare;
        if (this.fresh) {
            this.fresh = false;
            return false;
        }

        this.compute_delta();

        compare = this.deltas[this.deltas.length - 1];
        if (compare === undefined) {
            return false;
        }
        return compare > this.delay;
    }
}

