#include "lib_atmega/avr_sinus.h"

void setup() {
    h_timer tmr1 = create_timer(TN_FIRST, TCN_16BIT, NULL, NONE_OUT_PORT, NULL);
    h_timer tmr3 = create_timer(TN_THIRD, TCN_16BIT, NULL, NONE_OUT_PORT, NULL);
    h_timer tmr4 = create_timer(TN_FOURTH,TCN_16BIT, NULL, NONE_OUT_PORT, NULL);

    struct sinus_mode sin_mode;
    sin_mode.sin_nport = SIN_TWO_PORT;
    sin_mode.out_ports = SIN_OUT_NONE;
    sin_mode.shiftDegreesB = 0;
    sin_mode.shiftDegreesC = 0;
    sin_mode.frequency = 50;

    set_sinus_mode(tmr1, &sin_mode);

    sin_mode.shiftDegreesB = 120;
    set_sinus_mode(tmr3, &sin_mode);

    sin_mode.shiftDegreesB = 240;
    set_sinus_mode(tmr4, &sin_mode);

    timer_start(tmr1);
    timer_start(tmr3);
    timer_start(tmr4);
}

void loop() {

}