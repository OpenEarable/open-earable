#include "Button.h"

Button::Button(int pin, bool useInternalPullup) {
    _pin = pin;
    if (useInternalPullup) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }
}

void Button::inverted() {
    _inverted = true;
}

void Button::update() {
    bool reading = digitalRead(_pin);

    if (_inverted) reading = !reading;

    unsigned long now = millis();

    if (!reading) {
        _buttonState = IDLE;
        _lastDebounceTime = now;
        _pressStartTime = now;
        _pressed_flag = 1;
        _held_flag = 1;
        return;
    }

    if (now - _lastDebounceTime < _debounceDelay) return;
    _buttonState = PRESSED;
    if (_pressed_flag == 1) _pressed_flag = 0;

    if (now - _pressStartTime < _holdDelay) return;
    _buttonState = HELD;
    if (_held_flag == 1) _held_flag = 0;
}

ButtonState Button::getState() const {
    return _buttonState;
}

void Button::setDebounceTime(unsigned long debounceTime) {
    _debounceDelay = debounceTime;
}

void Button::setHoldTime(unsigned long holdTime) {
    _holdDelay = holdTime;
}

bool Button::get_pressed() {
    return _buttonState == PRESSED || _buttonState == HELD;
}

bool Button::get_held() {
    return _buttonState == HELD;
}

bool Button::get_pressed_single() {
    if (_pressed_flag) return false;

    _pressed_flag = 2;

    return get_pressed();
}

bool Button::get_held_single() {
    if (_held_flag) return false;

    _held_flag = 2;
    return get_held();
}

Button earable_btn(EPIN_BTN);
