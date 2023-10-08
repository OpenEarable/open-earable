#include "LED.h"

#define TOP 255

const uint8_t black[3] = {0,0,0};

LED::LED(int pin_r, int pin_g, int pin_b) {
    _r = pin_r;
    _g = pin_g;
    _b = pin_b;
}

bool LED::init() {
    nrfx_pwm_config_t const config0 = { 
      .output_pins =
      {
          digitalPinToPinName(_r) | NRFX_PWM_PIN_INVERTED, // channel 0
          digitalPinToPinName(_g) | NRFX_PWM_PIN_INVERTED, // channel 1
          digitalPinToPinName(_b) | NRFX_PWM_PIN_INVERTED, // channel 2
          NRFX_PWM_PIN_NOT_USED  // channel 3
      },
      .irq_priority = APP_IRQ_PRIORITY_LOWEST,
      .base_clock   = NRF_PWM_CLK_500kHz,
      .count_mode   = NRF_PWM_MODE_UP,
      .top_value    = TOP,
      .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
      .step_mode    = NRF_PWM_STEP_AUTO
    };

   
    if (nrfx_pwm_init(&m_pwm, &config0, NULL) != NRF_SUCCESS)  {
        return false;
    }
    // Search for available pwm
    /*for (int i = 0; i < 4; i++) {
        m_pwm = NRFX_PWM_INSTANCE(i);
        if (nrfx_pwm_init(&m_pwm, &config0, NULL) == NRF_SUCCESS)  {
            break;
            //return false;
        }
    }*/

    seq0.values.p_individual = &seq0_values;
    seq0.length          = NRF_PWM_VALUES_LENGTH(seq0_values);
    seq0.repeats         = 0;
    seq0.end_delay       = 0;
    
    (void)nrfx_pwm_simple_playback(&m_pwm, &seq0, 1, NRFX_PWM_FLAG_LOOP);
    return true;
}

void LED::set_color(const uint8_t col[3]) {
    seq0_values = {col[0],col[1],col[2],0};
}

LED earable_led(EPIN_RGB_R, EPIN_RGB_G, EPIN_RGB_B);


