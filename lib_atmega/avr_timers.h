#ifndef AVR_TIMERS
#define AVR_TIMERS

#include "avr_timers_base.h"
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#define MAX_TIMER_ATOM 6

enum timer_type {
    None 	= 0x00,
    Timer0 	= 0x01,
    Timer1 	= 0x02,
    Timer2 	= 0x03,
    Timer3 	= 0x04,
    Timer4 	= 0x05,
    Timer5 	= 0x06
};

enum event_type {
    CompareA = 0x01,
    CompareB = 0x02,
    CompareC = 0x03,
    Overflow = 0x04
};

typedef struct h_timer__ h_timer__;
typedef h_timer__* h_timer;

typedef void(*timer_event)(h_timer sender, uint8_t event);

#pragma pack(push, 1)
typedef struct timer_class {
    timer_event handler;
    word com;
    word wgm;
    word presc;
    word timsk;
    enum timer_type type : 3;
} timer_class;
#pragma pack(pop)

extern atom register_class(timer_class* pTmrClass);
extern boolean unregister_timer_class(enum timer_type type);

extern h_timer create_timer(enum timer_type type, word out_port_a, word out_port_b, word out_port_c);
extern void destroy_timer(h_timer htmr);

extern void timer_start(h_timer htmr);
extern void timer_stop(h_timer htmr);

#endif //!AVR_TIMERS