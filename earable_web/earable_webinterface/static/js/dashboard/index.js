import * as Interface from '../Interface.js'
import * as Manager from './modules/Manager.js'
import * as Dash from './modules/Dash.js'


var INTER = undefined;
var MAN = undefined;
var DASH = undefined;


/*
Modules:
=== Dash                =>      Contains UI control elements
=== Detect              =>      Handle packet loss
=== GLTFLoader          =>      3D model code
=== Interface           =>      Intermediate Class providing access to Earable_Interface
=== Manager             =>      Manage Connection (start/stop sampling)
=== Earable_Interface   =>      BLE connection logic to Earable_Interface
=== Recorder            =>      Recorder logic
=== utilitis            =>      Various useful functions
*/
/*
Dependencies:
index
--- Interface
    --- Earable_Interface
        --- utilities
--- Manager
    [Injected] Interface
    --- config
    --- Detect
        [Injected] Interface
        --- utilities
--- Dash
    [Injected] Interface
    --- utilities
    --- Recorder
        [Injected] Interface
        --- utilities
*/

function main() {
    INTER = new Interface.Interface("bigButton");
    DASH = new Dash.Dash(INTER);
    MAN = new Manager.Manager(INTER);
}

main();