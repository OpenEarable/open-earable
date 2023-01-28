#include "Edge_ML_Earable.h"

Edge_ML_Earable::Edge_ML_Earable() {
    // Init
}

bool Edge_ML_Earable::begin() {
    if (_debug) _debug->println("Begin Ard_Sense");

    // Begin sensors
    bleHandler_earable.begin();
    sensor_provider_earable.begin();

    if (_debug) _debug->println("End Begin \n");
    return true;
}

void Edge_ML_Earable::update() {
    // Update logic
    sensor_provider_earable.update();
    bleHandler_earable.update();
}

void Edge_ML_Earable::debug(Stream &stream)
{
    _debug = &stream;
    bleHandler_earable.debug(stream);
    sensor_provider_earable.debug(stream);

    _debug->println("Debugger Active");
}

Edge_ML_Earable edge_ml_earable;
