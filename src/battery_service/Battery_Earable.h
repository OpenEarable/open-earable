#ifndef EDGE_ML_EARABLE_BATTERY_EARABLE_H
#define EDGE_ML_EARABLE_BATTERY_EARABLE_H

#include "Arduino.h"
#include "Earable_Pins.h"

#include <nrfx_power.h>

enum CharingState {
    BATTERY,
    CHARGING,
    FULLY_CHARGED
};

class Battery_Earable {
public:
    Battery_Earable();

    void begin();

    bool check_battery();
    int get_battery_level() const;
    CharingState get_charging_state() const;

private:
    void _update_battery();

    // Map adc value to uniform format
    int _map_to_uniform(int value) const;

    // Convert from uniform format to battery percentage
    int _map_to_percentage(int value) const;

    unsigned long _last = -1;
    unsigned long _battery_interval = 5000;

    int _battery_level = 0;

    const _AnalogReferenceMode _internal_ref = AR_INTERNAL1V2;
    const int _adc_min = 734;
    const int _adc_max = 917;
    const int _uniform_max = 1000;

    nrfx_power_config_t _usb_config;

    CharingState _charging_state;
};


#endif //EDGE_ML_EARABLE_BATTERY_EARABLE_H
