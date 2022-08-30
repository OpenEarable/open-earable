//
// Created by Dylan Ray Roodt on 30.08.2022.
//

#ifndef EDGE_ML_EARABLE_PWM2_H
#define EDGE_ML_EARABLE_PWM2_H

int pwm_pin = D13;
float dutyCycle = 50.0f;
float freq  = 18000.0f;
//float freq  = 1000.0f;
mbed::PwmOut* pwm   = nullptr;

void begin_pwm2();
void start_pwm2();
void stop_pwm2();

void begin_pwm2() {
    pinMode(pwm_pin, OUTPUT);
    digitalWrite(pwm_pin, LOW);
}

void start_pwm2() {
    setPWM(pwm, pwm_pin, freq, dutyCycle);
}

void stop_pwm2() {
    stopPWM(pwm, pwm_pin);
}

#endif //EDGE_ML_EARABLE_PWM2_H
