import * as Interface from '../Interface.js'
import * as Manager from './modules/Manager.js'
import * as Dash from './modules/Dash.js'


var INTER = undefined;
var MAN = undefined;
var DASH = undefined;


/*
Modules:
=== Dash                =>      Contains UI control elements
=== Interface           =>      Intermediate Class providing access to Board_Interface
=== Manager             =>      Manage Connection (start/stop sampling)
=== Board_Interface     =>     BLE connection logic to Board
=== Recorder            =>      Recorder logic
=== utilitis            =>      Various useful functions
*/

/*
Dependencies:
index
--- Interface
    --- Board_Interface
        --- utilities
--- Manager
    [Injected] Interface
    --- config
--- Dash
    [Injected] Interface
    --- utilities
    --- Recorder
        [Injected] Interface
        --- utilities
*/

function main() {
    INTER = new Interface.Interface("connectButton");
    DASH = new Dash.Dash(INTER);
    MAN = new Manager.Manager(INTER);
}

main();