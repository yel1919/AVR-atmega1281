#ifndef AVR_TIMERS
#define AVR_TIMERS

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "avr_timers_base.h"

#define MAX_TMR_CLSS 2

//------------------
// Timer class name
//
enum tmrclss_name {
    TCN_NONAME  = 0x00,
    TCN_8BIT    = 0x01,
    TCN_16BIT   = 0x02
};
//------------------

enum timer_name {
    TN_NONAME  	= 0x00,
    TN_NULL 	= 0x01,
    TN_FIRST 	= 0x02,
    TN_SECOND 	= 0x03,
    TN_THIRD 	= 0x04,
    TN_FOURTH 	= 0x05,
    TN_FIFTH 	= 0x06
};

//---------------------------
// out ports
//
#define NONE_OUT_PORT   0x00
#define OUT_PORT0       0x01
#define OUT_PORT1       0x02
#define OUT_PORT2       0x04
//---------------------------

//---------------------------
// COM types
//
#define CT_NORMAL               0x00

#define CT_TOGGLE_OCXA          0x01
#define CT_CLEAR_OCXA           0x02
#define CT_SET_OCXA             0x03

#define CT_TOGGLE_OCXB          0x01
#define CT_CLEAR_OCXB           0x02
#define CT_SET_OCXB             0x03

#define CT_TOGGLE_OCXC          0x01
#define CT_CLEAR_OCXC           0x02
#define CT_SET_OCXC             0x03
//---------------------------

//-------------------------------
//WGM Types
//
#define WT_NORMAL               0x00

#define WT8_PHASE_0FF           0x01
#define WT8_CTC_OCRA            0x02
#define WT8_FAST_0FF            0x03
#define WT8_PHASE_OCRA          0x05
#define WT8_FAST_OCRA           0x07

#define WT16_PHASE_0FF          0x01
#define WT16_PHASE_1FF          0x02
#define WT16_PHASE_3FF          0x03
#define WT16_CTC_OCRA           0x04
#define WT16_FAST_0FF           0x05
#define WT16_FAST_1FF           0x06
#define WT16_FAST_3FF           0x07
#define WT16_FREQUENCY_ICR      0x08
#define WT16_FREQUENCY_OCRA     0x09
#define WT16_PHASE_ICR          0x0A
#define WT16_PHASE_OCRA         0x0B
#define WT16_CTC_ICR            0x0C
#define WT16_FAST_ICR           0x0E
#define WT16_FAST_OCRA          0x0F
//-------------------------------

//-------------------------------
// PRESCALER types
//
#define PT_NOCLK                0x00

#define PTN_1CLK                0x01
#define PTN_8CLK                0x02
#define PTN_64CLK               0x03
#define PTN_256CLK              0x04
#define PTN_1024CLK             0x05
#define PTN_TNFALLING           0x06
#define PTN_TNRISING            0x07

#define PT2_1CLK                0x01
#define PT2_8CLK                0x02
#define PT2_32CLK               0x03
#define PT2_64CLK               0x04
#define PT2_128CLK              0x05
#define PT2_256CLK              0x06
#define PT2_1024CLK             0x07
//-------------------------------

//-------------------------------
// TIMSK Types
//
#define TT_NONE                 0x00
#define TT_TOIE                 0x01
#define TT_OCIEA                0x02
#define TT_OCIEB                0x04
#define TT_OCIEC                0x08
#define ICIE                    0x20
//-------------------------------

enum event_type {
    CompareA = 0x01,
    CompareB = 0x02,
    CompareC = 0x03,
    Overflow = 0x04
};

typedef struct h_timer__ h_timer__;
typedef h_timer__* h_timer;

typedef void(*timer_event)(h_timer htmr, enum event_type event);

#pragma pack(push, 1)
struct timer_class {
    timer_event handler;
    uint8_t uiClassName : 8;
};

struct timer_mode {
    uint8_t coma    : 2,
            comb    : 2,
            wgm     : 4;
    uint8_t comc    : 2,
            timsk   : 6;
    uint8_t presc   : 3,
            reserve : 5;
};
#pragma pack(pop)

extern boolean register_class(struct timer_class* pTmrClass);
extern boolean unregister_timer_class(uint8_t tmrClassName);

extern h_timer find_timer(uint8_t tmrClassName, uint8_t tmrName);
extern h_timer create_timer(uint8_t timer_name, uint8_t class_name, struct timer_mode* modes, uint8_t out_ports);
extern boolean destroy_timer(h_timer htmr);
extern void    outports_enable(h_timer const timer, uint8_t out_ports, boolean flag);

extern void timer_start(h_timer const htmr);
extern void timer_stop(h_timer const htmr);

extern void set_frequency(h_timer const htmr, uint16_t freq);
extern void set_pwma(h_timer const htmr, uint8_t percent);
extern void set_pwmb(h_timer const htmr, uint8_t percent);
extern void set_pwmc(h_timer const htmr, uint8_t percent);

#endif //!AVR_TIMERS