#ifndef EDGE_ML_EARABLE_EDGEML_EARABLE_H
#define EDGE_ML_EARABLE_EDGEML_EARABLE_H

#include "EdgeML.h"
#include <custom_sensor/SensorManager_Earable.h>
#include <battery_service/Battery_Service.h>

class EdgeML_Earable {
public:
    EdgeML_Earable() = default;

    void begin() {
        _interface = new SensorManager_Earable();
        _battery = new Battery_Service();

        _battery->begin();

        edge_ml_generic.set_ble_config("Earable", "2.0.0");
        edge_ml_generic.set_custom(_interface);
        edge_ml_generic.begin();
    };

    void update() {
        _battery->update();
        edge_ml_generic.update();
    };

    void debug(Stream &stream) {
        _battery->debug(stream);
        edge_ml_generic.debug(stream);
    };
private:
    SensorManagerInterface * _interface{};
    Battery_Service * _battery{};
};

EdgeML_Earable edge_ml_earable;

#endif //EDGE_ML_EARABLE_EDGEML_EARABLE_H
