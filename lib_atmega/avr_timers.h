#ifndef AVR_TIMERS
#define AVR_TIMERS

#include <stdlib.h>
#include <string.h>
#include "user_timers.h"

#define MAX_TIMER_ATOM 6

enum timer_bit {
    _0bit	= 0x00,
    _8bit	= 0x01,
    _16bit	= 0x02,
};

enum timer_type {
    NoType 	= 0x00,
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

typedef void(*timer_event)(h_timer htmr, uint8_t event);

#pragma pack(push, 1)
struct timer_class {
    timer_event handler;
    word com;
    word wgm;
    word presc;
    word timsk;
    enum timer_type type : 3;
};
#pragma pack(pop)

extern atom register_class(struct timer_class* pTmrClass);
extern boolean unregister_timer_class(enum timer_type type);

extern h_timer create_timer(enum timer_type type, word out_port_a, word out_port_b, word out_port_c);
extern boolean destroy_timer(h_timer htmr);

extern void timer_start(h_timer const htmr);
extern void timer_stop(h_timer const htmr);

extern void set_class_long(h_timer const htmr, uint8_t index, long new_long);
extern void set_timer_dword(h_timer const htmr, uint8_t index, dword new_dword);

#endif //!AVR_TIMERS