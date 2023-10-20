#ifndef OPEN_EARABLE_BUTTON_H
#define OPEN_EARABLE_BUTTON_H

#include <Arduino.h>
#include "Earable_Pins.h"
#include "Button_Service.h"

enum ButtonState {
    IDLE,
    PRESSED,
    //HELD
};

class Button {
public:
    Button(int pin);

    void begin();
    void end();

    void inverted(bool _inverted = true);

    ButtonState getState() const;

   /*bool get_pressed();
    bool get_held();

    bool get_pressed_once();
    bool get_held_once();*/

    void setDebounceTime(unsigned long debounceTime);
    //void setHoldTime(unsigned long holdTime);

private:
    int _pin;
    bool _inverted = false;
    unsigned long _lastDebounceTime;
    unsigned long _pressStartTime;
    unsigned long _debounceDelay = 25;
    //unsigned long _holdDelay = 1000;

    //int _pressed_flag = 1;
    //int _held_flag = 1;

    ButtonState _buttonState = IDLE;

    void _read_state();
    static void _earable_btn_read_state();
};

extern Button earable_btn;

#endif //OPEN_EARABLE_BUTTON_H
