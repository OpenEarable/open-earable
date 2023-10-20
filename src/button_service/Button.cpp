#include "Button.h"

Button::Button(int pin) {
    _pin = pin;
}

void Button::begin() {
    pinMode(_pin, INPUT);

    // prevent multiattachment of the interrupt
    detachInterrupt(digitalPinToInterrupt(_pin));
    attachInterrupt(digitalPinToInterrupt(_pin), _earable_btn_read_state, CHANGE);
}


void Button::end() {
    detachInterrupt(digitalPinToInterrupt(_pin));
}

void Button::inverted(bool _inverted) {
    this->_inverted = _inverted;
}

void Button::_earable_btn_read_state() {
    earable_btn._read_state();
}

void Button::_read_state() {
    bool reading = digitalRead(_pin);

    //if (_inverted) reading = !reading;
    reading = _inverted ^ reading;

    unsigned long now = millis();

    if (!reading) {
        if (_buttonState != IDLE) {
            button_service.write_state(IDLE);
        }
        _buttonState = IDLE;
        _lastDebounceTime = now;
        _pressStartTime = now;
        return;
    }

    if (now - _lastDebounceTime < _debounceDelay) return;
    _buttonState = PRESSED;
    button_service.write_state(_buttonState);
}

ButtonState Button::getState() const {
    return _buttonState;
}

void Button::setDebounceTime(unsigned long debounceTime) {
    _debounceDelay = debounceTime;
}

Button earable_btn(EPIN_BTN);
