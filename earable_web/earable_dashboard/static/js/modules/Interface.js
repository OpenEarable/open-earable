import * as Earable_Interface from './Earable_Interface.js'



export class Interface {
    constructor(button_id) {
        this.interface = new Earable_Interface.Earable_Interface(button_id);
    }

    configure(config) {
        this.interface.send_custom_configure(config);
    }

    event(func, obj) {
        this.interface.add_custom_event(func.bind(obj));
    }

    remove_event(func) {
        this.interface.remove_custom_event(func);
    }

    connection_event(func, obj) {
        this.interface.add_custom_connection_event(func.bind(obj));
    }

    notify(func, obj) {
        this.interface.add_notify_event(func.bind(obj));
    }
}