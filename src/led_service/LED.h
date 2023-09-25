#ifndef OPEN_EARABLE_LED_H
#define OPEN_EARABLE_LED_H

#include <Arduino.h>
#include "Earable_Pins.h"

enum Color {
    OFF,
    GREEN,
    BLUE,
    RED,
    CYAN,
    YELLOW,
    MAGENTA,
    WHITE
};

class LED {
public:
    LED(int pin_r, int pin_g, int pin_b);

    void invert();

    void set_color(Color col);

private:
    int _r;
    int _g;
    int _b;

    bool _inverted = false;
};

extern LED earable_led;

#endif //OPEN_EARABLE_LED_H
