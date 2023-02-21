#include "Battery_Earable.h"

Battery_Earable::Battery_Earable() {
}

void Battery_Earable::begin() {
    pinMode(_battery_pin, INPUT);
    analogReference(_internal_ref);
}

int Battery_Earable::get_battery_level() const {
    return _battery_level;
}

bool Battery_Earable::check_battery() {
    unsigned long now = millis();
    if (now - _last > _battery_interval) {
        _update_battery();
        _last = now;
        return true;
    }
    return false;
}

void Battery_Earable::_update_battery() {
    int adc_value = analogRead(_battery_pin);
    int uniform = _map_to_uniform(adc_value);
    _battery_level = _map_to_percentage(uniform);
}

int Battery_Earable::_map_to_uniform(int value) const {
    if (value < _adc_min) return 0;
    if (value > _adc_max) return _uniform_max;
    return map(value, _adc_min, _adc_max, 0, _uniform_max);
}

int Battery_Earable::_map_to_percentage(int value) const {
    const int a1 = int(0.82 * _uniform_max);
    const int p1 = 96;

    const int a2 = int(0.30 * _uniform_max);
    const int p2 = 46;

    const int a3 = int(0.14 * _uniform_max);
    const int p3 = 15;

    if (value > _uniform_max) return 100;
    else if (value > a1) return map(value, a1, _uniform_max, p1, 100);
    else if (value > a2) return map(value, a2, a1, p2, p1);
    else if (value > a3) return map(value, a3, a2, p3, p2);
    else if (value > 0) return map(value, 0, a3, 0, p3);

    return 0;
}
