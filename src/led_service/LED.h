#ifndef OPEN_EARABLE_LED_H
#define OPEN_EARABLE_LED_H

#include <Arduino.h>
#include "Earable_Pins.h"
#include <nrfx_pwm.h>

typedef uint8_t RGBColor[3];

class LED {
public:
    LED(int pin_r, int pin_g, int pin_b);

    bool init();

    void set_color(const RGBColor col);

private:
    int _r,_g,_b;

    nrfx_pwm_t m_pwm = NRFX_PWM_INSTANCE(3);
    nrf_pwm_sequence_t seq0;
    nrf_pwm_values_individual_t seq0_values = {0,0,0,0};
};

extern LED earable_led;

#endif //OPEN_EARABLE_LED_H
