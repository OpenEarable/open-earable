import * as Recorder from './Recorder.js'
import * as utils from "./utilities.js";
import "./OBJLoader.js";
import "./MTLLoader.js";

var SensorMap = {
    accelerometer:
        {
            data: {'Ax':[],'Ay':[],'Az':[]},
        },
    gyroscope:
        {
            data: {'Gx':[],'Gy':[],'Gz':[]},
        },
    temperature:{
        data: {temperature:[]}
    },

    pressure:{
        data: {pressure:[]}
    }
}

var IDMap = {
    0:"accelerometer",
    1:"gyroscope",
    2:"pressure",
    3:"temperature"
}

const ROTATION_OFFSET = [
    - Math.PI/2,
    0,
    0
]

const CONVERSION_FACTORS = {
    0: 1,           // Accelerometer Factor
    1: 1           // Gyroscope Factor
}

const colors = ["#FF355E","#FD5B20","#FF6037","#FF9966","#FF9933","#FFCC33","#FFFF66","#FFFF66","#CCFF00","#66FF66","#AAF0D1","#50BFE6","#FF6EFF","#EE34D2","#FF00CC","#FF00CC"];
Array.prototype.latest = function(){return this[this.length - 1];};


export class Dash {
    constructor(inter) {
        this.interface = inter;
        this.recorder = new Recorder.Recorder(inter);

        this.dashboard = new Dashboard();

        this.register();
    }

    register() {
        this.interface.event(this.data_receive, this);
        this.interface.connection_event(this.connection, this);
        this.interface.notify(this.notify, this);
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
        this.maxRecords = 64;
        this.minimal_update = 500;
        this.arduinoModel = undefined;
        this.connected = false;

        this.model_name = 'models/model.obj';
        this.model_textures = 'models/textures.mtl';

        // UI elements
        this.bigButton = document.getElementById('bigButton');
        this.BLEstatus = document.getElementById('bluetooth');

        this.sensors = Object.keys(SensorMap);

        // Graphing
        this.colorIndex = 0;

        // 3d model
        this.renderer = new THREE.WebGLRenderer();
        this.scene = new THREE.Scene();
        this.camera = new THREE.PerspectiveCamera(75, 1, 1, 10000);

        this.skip_frame = false;

        this.check_web_ble();
        this.init_sensors();
        this.init3D();

        requestAnimationFrame(this.draw.bind(this));
    }

    check_web_ble() {
        if (!("bluetooth" in navigator)) {
            this.notify("No_Web_BLE")
        }
    }

    init_sensors() {
        for (const [key, value] of Object.entries(SensorMap)) {
            value["time"] = [];
            value["timeout"] = new utils.Timer(this.minimal_update);
        }

        this.init_Graph('accelerometer');
        this.init_Graph('gyroscope');
        this.init_Graph('temperature');
        this.init_Graph('pressure');
    }

    init_Graph(sensor){
        let title = sensor;
        let series = Object.keys(SensorMap[sensor].data);
        let format = [];
        series.forEach(function(item){
            this.colorIndex++;
            if (this.colorIndex>colors.length) {this.colorIndex=0;}
            format.push({
                x: [],
                y: [],
                name: item,
                mode: 'lines',
                width: 1,
                line: {width: 1,color: colors[this.colorIndex]}
            });
        }.bind(this));

        Plotly.plot(title,format,{
                plot_bgcolor: '#111111',
                paper_bgcolor: '#111111',
                margin: {l:8,r:8,b:18,t:18},
                showlegend: false,
                yaxis: {'showticklabels':false},
                xaxis: {
                    //'range': [0, this.maxRecords],
                    'showticklabels':false,
                    //'autorange': false,
                    'showgrid': true,
                    'zeroline': true,
                    tickfont: {size: 8}
                }
            }
        );
    }

    init3D() {
        let container = document.getElementById( '3d' );
        var mtlLoader = new THREE.MTLLoader();
        mtlLoader.load(this.model_textures, function(materials) {
            materials.preload();
            let loader = new THREE.OBJLoader();
            loader.setMaterials(materials);
            this.scene.background = new THREE.Color( 0x111111 );
            this.renderer.setSize( 384, 384 );
            this.renderer.setPixelRatio( 4 );
            container.appendChild( this.renderer.domElement );
            loader.load( this.model_name,
                // called when the Arduinio model is loaded
                function ( object ) {
                    // Model
                    let mesh = object;
                    let box = new THREE.Box3().setFromObject( mesh );
                    box.getCenter( mesh.position );
                    mesh.position.multiplyScalar( - 1 );
                    let pivot = new THREE.Group();
                    pivot.rotation.set(...ROTATION_OFFSET);
                    this.scene.add(pivot);
                    pivot.add( mesh );
                    this.arduinoModel = pivot;

                    // The X axis is red. The Y axis is blue. The Z axis is green.
                    //let axesHelper = new THREE.AxesHelper( 100 );
                    //this.scene.add( axesHelper );


                    // Light
                    const color = 0xFFFFFF;
                    const intensity = 2;
                    const light = new THREE.DirectionalLight(color, intensity);
                    light.position.set(-20, 100, 0);
                    light.target.position.set(0, 40, 0);
                    this.scene.add(light);
                    this.scene.add(light.target);
                    var hemiLight = new THREE.HemisphereLight(0xffffff, 0xffffff, 1000);
                    this.scene.add(hemiLight);

                    // Camera
                    this.camera.position.set(0,200,0);
                    this.camera.rotation.y = 0; // 180
                    this.camera.lookAt(new THREE.Vector3(0,0,0));
                    this.renderer.render(this.scene, this.camera);
                }.bind(this)
            );
        }.bind(this));
    }

    msg(m){
        this.BLEstatus.innerHTML = m;
    }

    notify(type, info) {
        switch (type) {
            case "Requesting_BLE":
                this.bigButton.style.backgroundColor="grey";
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
                this.bigButton.style.backgroundColor = 'green';
                this.msg('connected.');
                break;
            case "Error":
                this.msg('Error!');
                break;
            case "Disconnected":
                this.connected = false;
                // clear read polling
                this.sensors.forEach(sensor => {
                    if(typeof SensorMap[sensor].polling !== 'undefined'){
                        clearInterval(SensorMap[sensor].polling);
                    }
                });
                this.msg('Device ' + info[0] + ' is disconnected.');
                break;
            case "No_Web_BLE":
                this.msg("browser not supported"); bigButton.style.backgroundColor = "red";
                alert("Error: This browser doesn't support Web Bluetooth. Try using Chrome.");
                break;
        }
    }

    graph(sensor){
        let labels = Object.keys(SensorMap[sensor].data);
        let values = [];
        let time = [];

        labels.forEach(function(label){
            values.push(SensorMap[sensor].data[label]);
            time.push(SensorMap[sensor].time);
        });
        Plotly.restyle(sensor, {x: time,y: values});
    }

    draw(){
        function updateViz(sensor,fns) {
            if (this.connected) {
                this.auto_adder(sensor);
            }

            if (SensorMap[sensor].rendered === false) { // only render if new values are received
                fns.forEach(function(fn){
                    fn(sensor);
                });
                SensorMap[sensor].rendered = true;
            }
        }

        updateViz = updateViz.bind(this);

        if (this.skip_frame === false){ // TODO update with fuction to iterate object with viz function as a property
            updateViz('gyroscope', [this.graph]);
            updateViz('accelerometer', [this.graph,this.update3d.bind(this)]);

            updateViz('temperature', [this.graph,this.digit]);
            updateViz('pressure', [this.graph,this.digit]);

            this.skip_frame = true; // render alternate frames = 30fps
        } else {this.skip_frame=false;}
        requestAnimationFrame(this.draw.bind(this));
    }

    digit(sensor){
        const value = SensorMap[sensor].data[sensor].latest();
        const div = document.getElementById(sensor+"-value");
        if (!Number.isNaN(value)) {div.innerHTML = Math.round(value*10)/10;};
    }

    update3d(sensor){
        let Ax = SensorMap['accelerometer'].data.Ax.latest() * 0.0174533;
        let Ay = SensorMap['accelerometer'].data.Ay.latest() * 0.0174533;
        let Az = SensorMap['accelerometer'].data.Az.latest() * 0.0174533;
        Ay, Az = Az, Ay;
        let pitch = Math.atan2((-Ax) , Math.sqrt(Ay * Ay + Az * Az));
        let roll = Math.atan2(Ay , Az);

        this.arduinoModel.rotation.set(...ROTATION_OFFSET);

        // Model axis not board axis
        this.arduinoModel.rotation.x -= roll + Math.PI/2;
        this.arduinoModel.rotation.y += 0;
        this.arduinoModel.rotation.z += pitch;

        this.renderer.render(this.scene, this.camera);
    }

    auto_adder(sensor_name) {
        let sensor = SensorMap[sensor_name];

        if (sensor.timeout.check()) {
            if (sensor.time.length === 0) {
                return;
            }
            this.add_value(sensor.time[sensor.time.length-1]+this.minimal_update, sensor.time);

            for (const [key, value] of Object.entries(sensor.data)) {
                this.add_value(value[value.length-1], value);
            }
            SensorMap[sensor_name].rendered = false;
        }
    }

    add_value(value, array) {
        // Push sensor reading onto data array
        array.push(value);
        // Keep array at buffer size
        if (array.length> this.maxRecords) {array.shift();}
    }

    data_receive(data) {
        let sensor, columns, s_value;
        let [id, parsedData_obj] = data;

        sensor = SensorMap[IDMap[id]];
        columns = Object.keys(sensor.data);

        if (sensor["skip_first"] === undefined) {
            sensor["skip_first"] = false;
            return;
        }

        sensor.timeout.set();
        this.add_value(parsedData_obj.time, sensor.time)
        parsedData_obj.data.forEach(function (value, i) {

            s_value = value.value
            if (id in CONVERSION_FACTORS) {
                s_value *= CONVERSION_FACTORS[id];
            }

            this.add_value(s_value, sensor.data[columns[i]])
        }.bind(this));

        sensor.rendered = false; // flag - vizualization needs to be updated
    }
}


