#include "LED.h"

LED::LED(int pin_r, int pin_g, int pin_b) {
    _r = pin_r;
    _g = pin_g;
    _b = pin_b;

    pinMode(_r, OUTPUT);
    pinMode(_g, OUTPUT);
    pinMode(_b, OUTPUT);
}

void LED::set_color(Color color) {
    bool on = _inverted;
    bool off = !_inverted;
    switch (color) {
        case OFF:
            digitalWrite(_r, off);
            digitalWrite(_g, off);
            digitalWrite(_b, off);
            break;
        case RED:
            digitalWrite(_r, on);
            digitalWrite(_g, off);
            digitalWrite(_b, off);
            break;
        case GREEN:
            digitalWrite(_r, off);
            digitalWrite(_g, on);
            digitalWrite(_b, off);
            break;
        case BLUE:
            digitalWrite(_r, off);
            digitalWrite(_g, off);
            digitalWrite(_b, on);
            break;
        case YELLOW:
            digitalWrite(_r, on);
            digitalWrite(_g, on);
            digitalWrite(_b, off);
            break;
        case CYAN:
            digitalWrite(_r, off);
            digitalWrite(_g, on);
            digitalWrite(_b, on);
            break;
        case MAGENTA:
            digitalWrite(_r, on);
            digitalWrite(_g, off);
            digitalWrite(_b, on);
            break;
        case WHITE:
            digitalWrite(_r, on);
            digitalWrite(_g, on);
            digitalWrite(_b, on);
            break;
    }
}

void LED::invert() {
    _inverted = !_inverted;
}

LED earable_led(EPIN_RGB_R, EPIN_RGB_G, EPIN_RGB_B);


