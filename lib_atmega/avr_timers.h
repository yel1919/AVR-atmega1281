#ifndef AVR_TIMERS
#define AVR_TIMERS

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "avr_timers_base.h"

#define MAX_TMR 6

#define HIREG(x)        ((((uint8_t)(x)) >> 4) & 0x0F)
#define LOREG(x)        (((uint8_t)(x)) & 0x0F)
#define MAKEREG(h, l)   (((((uint8_t)(h)) & 0x0F) << 4) | (((uint8_t)(l)) & 0x0F))

//-----------------------------------
// Timer class name
//
#define TCN_NONAME              0x000
#define TCN_8BIT                0x001
#define TCN_16BIT               0x002
//-----------------------------------

//-----------------------------------
// Timer name
//
#define TN_NONAME               0x000
#define TN_NULL                 0x001
#define TN_FIRST                0x002
#define TN_SECOND               0x003
#define TN_THIRD                0x004
#define TN_FOURTH               0x005
#define TN_FIFTH                0x006
//-----------------------------------

//-----------------------------------
// out ports
//
#define NONE_OUT_PORT           0x000
#define OUT_PORTA               0x001
#define OUT_PORTB               0x002
#define OUT_PORTC               0x004
//-----------------------------------

//-----------------------------------
// COM types
//
#define CT_NORMAL               0x000

#define CT_TOGGLE_OCXA          0x001
#define CT_CLEAR_OCXA           0x002
#define CT_SET_OCXA             0x003

#define CT_TOGGLE_OCXB          0x001
#define CT_CLEAR_OCXB           0x002
#define CT_SET_OCXB             0x003

#define CT_TOGGLE_OCXC          0x001
#define CT_CLEAR_OCXC           0x002
#define CT_SET_OCXC             0x003
//-----------------------------------

//-----------------------------------
//WGM Types
//
#define WT_NORMAL               0x000

#define WT8_PHASE_0FF           0x001
#define WT8_CTC_OCRA            0x002
#define WT8_FAST_0FF            0x003
#define WT8_PHASE_OCRA          0x005
#define WT8_FAST_OCRA           0x007

#define WT16_PHASE_0FF          0x001
#define WT16_PHASE_1FF          0x002
#define WT16_PHASE_3FF          0x003
#define WT16_CTC_OCRA           0x004
#define WT16_FAST_0FF           0x005
#define WT16_FAST_1FF           0x006
#define WT16_FAST_3FF           0x007
#define WT16_FREQUENCY_ICR      0x008
#define WT16_FREQUENCY_OCRA     0x009
#define WT16_PHASE_ICR          0x00A
#define WT16_PHASE_OCRA         0x00B
#define WT16_CTC_ICR            0x00C
#define WT16_FAST_ICR           0x00E
#define WT16_FAST_OCRA          0x00F
//-----------------------------------

//-----------------------------------
// PRESCALER types
//
#define PT_NOCLK                0x000

#define PTN_1CLK                0x001
#define PTN_8CLK                0x002
#define PTN_64CLK               0x003
#define PTN_256CLK              0x004
#define PTN_1024CLK             0x005
#define PTN_TNFALLING           0x006
#define PTN_TNRISING            0x007

#define PT2_1CLK                0x001
#define PT2_8CLK                0x002
#define PT2_32CLK               0x003
#define PT2_64CLK               0x004
#define PT2_128CLK              0x005
#define PT2_256CLK              0x006
#define PT2_1024CLK             0x007

// Value
#define PTV_NOCLK               0x000
#define PTV_1CLK                0x001
#define PTV_8CLK                0x008
#define PTV_32CLK               0x020
#define PTV_64CLK               0x040
#define PTV_128CLK              0x080
#define PTV_256CLK              0x100
#define PTV_1024CLK             0x400
//-----------------------------------

//-----------------------------------
// TIMSK Types
//
#define TT_NONE                 0x000
#define TT_TOIE                 0x001
#define TT_OCIEA                0x002
#define TT_OCIEB                0x004
#define TT_OCIEC                0x008
#define ICIE                    0x020
//-----------------------------------

//-----------------------------------
// Get Timer Ptr
//
#define GTP_NAME                0x000
#define GTP_CLASS               0x001
#define GTP_PRESC               0x002
#define GTP_CMPPROC             0x004
//-----------------------------------

//-----------------------------------
// Timer Message
//
#define TM_CAPT                 0x001
#define TM_COMPA                0x002
#define TM_COMPB                0x003
#define TM_COMPC                0x004
#define TM_OVF                  0x005
//-----------------------------------

typedef struct h_timer__ h_timer__;
typedef h_timer__* h_timer;
typedef uint8_t lresult;

typedef lresult(*tmr_cmp_proc)(h_timer htmr, uint8_t msg);
typedef lresult(*sub_tmr_proc)(h_timer htmr, uint8_t msg, uint8_t subId, void *refData);

//#pragma pack(push, 1)
struct timer_mode {
    uint8_t  coma    : 2,
             comb    : 2,
             wgm     : 4;
    uint8_t  comc    : 2,
             timsk   : 6;
    uint32_t freq;
} __attribute__((packed, aligned(1)));
//#pragma pack(pop)

extern h_timer  create_timer(uint8_t timer_name, uint8_t class_name, struct timer_mode* modes, uint8_t out_ports, tmr_cmp_proc handler);
extern boolean  destroy_timer(h_timer htmr);
extern void     outports_enable(h_timer const timer, uint8_t out_ports, boolean flag);

extern void     timer_start(h_timer const htmr);
extern void     timer_pause(h_timer const htmr);
extern void     timer_stop(h_timer const htmr);
extern void     timer_restart(h_timer const htmr);

extern void     set_timer_mode(h_timer const timer, const struct timer_mode* const modes);

extern void     set_frequency(h_timer const htmr, uint16_t freq);
extern void     set_porta_percent(h_timer const htmr, uint8_t percent);
extern void     set_portb_percent(h_timer const htmr, uint8_t percent);
extern void     set_portc_percent(h_timer const htmr, uint8_t percent);

extern uint32_t calc_frequency_ctc(uint16_t top, uint16_t prescaler);
extern uint32_t calc_frequency_fastpwm(uint16_t top, uint16_t prescaler);
extern uint32_t calc_frequency_correctpwm(uint16_t top, uint16_t prescaler);

extern uint16_t get_timer_ptr(h_timer const htimer, uint8_t nIndex);
extern uint16_t set_timer_ptr(h_timer const htimer, uint8_t nIndex, uint16_t newValue);

extern boolean  set_subtimer(h_timer const htimer, sub_tmr_proc sub_proc, uint8_t subId, void* subRefData);
extern boolean  remove_subtimer(h_timer const htimer, uint8_t subId);
extern lresult  def_subtimer_proc(h_timer htmr, uint8_t msg);

#endif //!AVR_TIMERS