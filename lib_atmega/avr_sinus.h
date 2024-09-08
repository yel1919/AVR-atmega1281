#ifndef AVR_SINUS
#define AVR_SINUS

#include "avr_timers.h"

#define SIN_ONE_PORT    0x01
#define SIN_TWO_PORT    0x02

#define SIN_OUT_NONE    0x00
#define SIN_OUT_B       0x01
#define SIN_OUT_C       0x02

//#pragma pack(push, 1)
struct sinus_mode {
    uint8_t sin_nport       : 2;
    uint8_t out_ports       : 2;
    uint8_t reserv          : 4;
    float   shiftDegreesB;
    float   shiftDegreesC;
    uint8_t frequency;
} __attribute__((packed, aligned(1)));
//#pragma pack(pop)

extern void     init_sin();

extern void     set_phase_shift_d(float degrees, uint8_t out_port);
extern void     set_phase_shift_r(float radians, uint8_t out_port);
extern h_timer  set_sinus_mode(h_timer const htmr, struct sinus_mode* const sin_mode);

extern uint8_t  shiftPhaseR(float radians);
extern uint8_t  shiftPhaseD(float degrees);

extern void     set_sin_frequency(h_timer const htimer, uint8_t freq);

#endif //!AVR_SINUS
