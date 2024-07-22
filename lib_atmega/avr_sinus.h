#ifndef AVR_SINUS
#define AVR_SINUS

#include "avr_timers.h"

#define SIN_PERIOD 200

extern void    init_sin();
extern uint8_t shiftPhaseR(float radians);
extern uint8_t shiftPhaseD(float degrees);

#endif //!AVR_SINUS
