#include "avr_timers.h"

typedef uint8_t  (*v2p)(uint16_t value);
typedef uint16_t (*p2v)(uint8_t presc);
typedef uint16_t (*calc_top_presc)(uint32_t freq, uint16_t top);

uint8_t  value2presc(uint16_t value);
uint8_t  value2presc2(uint16_t value);
uint16_t presc2value(uint8_t presc);
uint16_t presc2value2(uint8_t presc);

uint16_t calc_prescaler_ctc(uint32_t freq, uint16_t top);
uint16_t calc_prescaler_fastpwm(uint32_t freq, uint16_t top);
uint16_t calc_prescaler_correctpwm(uint32_t freq, uint16_t top);
uint16_t calc_top_ctc(uint32_t freq, uint16_t prescaler);
uint16_t calc_top_fastpwm(uint32_t freq, uint16_t prescaler);
uint16_t calc_top_correctpwm(uint32_t freq, uint16_t prescaler);

#pragma pack(push, 1)
struct __wg_mode__ {
    uint16_t                max_top;
    uint8_t                 reg_xcrx;

    v2p                     to_prescaler;
    p2v                     to_value;
    calc_top_presc          calc_presc;
    calc_top_presc          calc_period;
};

struct __sub_timer__ {
    uint8_t                 id;
    void                    *refData;
    sub_tmr_proc            subproc;

    struct __sub_timer__    *next;
    struct __sub_timer__    *prev;
};

struct __timer_base__ {
    uint8_t                 timer_name;
    uint8_t                 class_name;

    uint8_t                 presc;
    boolean                 active;

    tmr_cmp_proc            handler;

    struct __wg_mode__      mode;

    struct __sub_timer__    *subtimer_current;
    struct __sub_timer__    *subtimer_head;
    struct __sub_timer__    *subtimer_tail;
};
#pragma pack(pop)

#define MAX_TOP_0FF     255
#define MAX_TOP_1FF     511
#define MAX_TOP_3FF     1023
#define MAX_TOP_XCRX    65535

#define TRM_TCCRA       0
#define TRM_TCCRB       1
#define TRM_TCCRC       2
#define TRM_TIMSK       3
#define TRM_TCNTH       4
#define TRM_TCNTL       5
#define TRM_OCRAH       6
#define TRM_OCRAL       7
#define TRM_OCRBH       8
#define TRM_OCRBL       9
#define TRM_OCRCH       10
#define TRM_OCRCL       11
#define TRM_ICRH        12
#define TRM_ICRL        13

#define TPM_PORTA       0
#define TPM_PORTB       1
#define TPM_PORTC       2

uint8_t tmrpinmap [6][3] = {
    { PNB7, PNG5, PN00 },
    { PNB5, PNB6, PNB7 },
    { PNB4, PN00, PN00 },
    { PNE3, PNE4, PNE5 },
    { PN00, PN00, PN00 },
    { PN00, PN00, PN00 }
};

volatile uint8_t* tmrrgstrmap[6][14] = {
    { &TCCR0A, &TCCR0B, NULL,    &TIMSK0, NULL,    &TCNT0,  NULL,    &OCR0A,  NULL,    &OCR0B,  NULL,    NULL,    NULL,   NULL   },
    { &TCCR1A, &TCCR1B, &TCCR1C, &TIMSK1, &TCNT1H, &TCNT1L, &OCR1AH, &OCR1AL, &OCR1BH, &OCR1BL, &OCR1CH, &OCR1CL, &ICR1H, &ICR1L },
    { &TCCR2A, &TCCR2B, NULL,    &TIMSK2, NULL,    &TCNT2,  NULL,    &OCR2A,  NULL,    &OCR2B,  NULL,    NULL,    NULL,   NULL   },
    { &TCCR3A, &TCCR3B, &TCCR3C, &TIMSK3, &TCNT3H, &TCNT3L, &OCR3AH, &OCR3AL, &OCR3BH, &OCR3BL, &OCR3CH, &OCR3CL, &ICR3H, &ICR3L },
    { &TCCR4A, &TCCR4B, &TCCR4C, &TIMSK4, &TCNT4H, &TCNT4L, &OCR4AH, &OCR4AL, &OCR4BH, &OCR4BL, &OCR4CH, &OCR4CL, &ICR4H, &ICR4L },
    { &TCCR5A, &TCCR5B, &TCCR5C, &TIMSK5, &TCNT5H, &TCNT5L, &OCR5AH, &OCR5AL, &OCR5BH, &OCR5BL, &OCR5CH, &OCR5CL, &ICR5H, &ICR5L }
};

calc_top_presc calcprescmap8[8] = {
    NULL, &calc_prescaler_correctpwm, &calc_prescaler_ctc, &calc_prescaler_fastpwm, NULL, &calc_prescaler_correctpwm, NULL, &calc_prescaler_fastpwm
};

calc_top_presc calctopmap8[8] = {
    NULL, &calc_top_correctpwm, &calc_top_ctc, &calc_top_fastpwm, NULL, &calc_top_correctpwm, NULL, &calc_top_fastpwm
};

calc_top_presc calcprescmap16[16] = {
    NULL,
    &calc_prescaler_correctpwm,
    &calc_prescaler_correctpwm,
    &calc_prescaler_correctpwm,
    &calc_prescaler_ctc,
    &calc_prescaler_fastpwm,
    &calc_prescaler_fastpwm,
    &calc_prescaler_fastpwm,
    &calc_prescaler_correctpwm,
    &calc_prescaler_correctpwm,
    &calc_prescaler_correctpwm,
    &calc_prescaler_correctpwm,
    &calc_prescaler_ctc,
    NULL,
    &calc_prescaler_fastpwm,
    &calc_prescaler_fastpwm
};

calc_top_presc calctopmap16[16] = {
    NULL,
    &calc_top_correctpwm,
    &calc_top_correctpwm,
    &calc_top_correctpwm,
    &calc_top_ctc,
    &calc_top_fastpwm,
    &calc_top_fastpwm,
    &calc_top_fastpwm,
    &calc_top_correctpwm,
    &calc_top_correctpwm,
    &calc_top_correctpwm,
    &calc_top_correctpwm,
    &calc_top_ctc,
    NULL,
    &calc_top_fastpwm,
    &calc_top_fastpwm
};

v2p toprescmap[2] = {
    &value2presc, &value2presc2
};

p2v tovaluemap[2] = {
    &presc2value, &presc2value2
};

void* wgmfoomap[6] = { &toprescmap, &tovaluemap, &calcprescmap8, &calcprescmap16, &calctopmap8, &calctopmap16 };

h_timer tmr_table[MAX_TMR];

void outports_enable(h_timer const timer, uint8_t out_ports, boolean flag) {
    if(timer != NULL) {
        struct __timer_base__* tmr_base = (struct __timer_base__*)timer;
        if(out_ports & OUT_PORTA)
            set_mode(tmrpinmap[tmr_base->timer_name - 1][TPM_PORTA], flag);
        if(out_ports & OUT_PORTB)
            set_mode(tmrpinmap[tmr_base->timer_name - 1][TPM_PORTB], flag);
        if((out_ports & OUT_PORTC) && tmr_base->class_name == TCN_16BIT)
            set_mode(tmrpinmap[tmr_base->timer_name - 1][TPM_PORTC], flag);
    }
}

void set_presc(struct __timer_base__* const timer, uint8_t presc) {
    if(timer != NULL) {
        timer->presc = presc;
        if(timer->active)
            set_prescalern(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRB], presc);
    }
}

void set_wgmode(struct __timer_base__* timer, uint8_t wgm)
{
    if(timer->timer_name == TN_SECOND) {
        timer->mode.to_prescaler   = ((v2p*)wgmfoomap[0])[1];
        timer->mode.to_value       = ((p2v*)wgmfoomap[1])[1];
    }
    else {
        timer->mode.to_prescaler   = ((v2p*)wgmfoomap[0])[0];
        timer->mode.to_value       = ((p2v*)wgmfoomap[1])[0];
    }

    if(timer->class_name == TCN_8BIT) {
        set_wgmn8(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], tmrrgstrmap[timer->timer_name - 1][TRM_TCCRB], wgm);

        timer->mode.max_top      = MAX_TOP_0FF;
        if(wgm == WT8_CTC_OCRA || wgm == WT8_FAST_OCRA || wgm == WT8_PHASE_OCRA)
            timer->mode.reg_xcrx = MAKEREG(TRM_OCRAH, TRM_OCRAL);
        else
            timer->mode.reg_xcrx = 0;

        timer->mode.calc_presc      = ((calc_top_presc*)wgmfoomap[2])[wgm];
        timer->mode.calc_period     = ((calc_top_presc*)wgmfoomap[4])[wgm];
    }
    else if(timer->class_name == TCN_16BIT) {
        set_wgmn16(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], tmrrgstrmap[timer->timer_name - 1][TRM_TCCRB], wgm);

        if(wgm == WT16_PHASE_0FF || wgm == WT16_FAST_0FF)
            timer->mode.max_top = MAX_TOP_0FF;
        else if(wgm == WT16_PHASE_1FF || wgm == WT16_FAST_1FF)
            timer->mode.max_top = MAX_TOP_1FF;
        else if(wgm == WT16_PHASE_3FF || wgm == WT16_FAST_3FF)
            timer->mode.max_top = MAX_TOP_3FF;
        else
            timer->mode.max_top = MAX_TOP_XCRX;

        if(wgm == WT16_CTC_OCRA || wgm == WT16_FAST_OCRA || wgm == WT16_PHASE_OCRA || wgm == WT16_FREQUENCY_OCRA)
            timer->mode.reg_xcrx = MAKEREG(TRM_OCRAH, TRM_OCRAL);
        else if(wgm == WT16_CTC_ICR || wgm == WT16_FAST_ICR || wgm == WT16_PHASE_ICR || wgm == WT16_FREQUENCY_ICR)
            timer->mode.reg_xcrx = MAKEREG(TRM_ICRH, TRM_ICRL);
        else
            timer->mode.reg_xcrx = 0;

        timer->mode.calc_presc     = ((calc_top_presc*)wgmfoomap[3])[wgm];
        timer->mode.calc_period    = ((calc_top_presc*)wgmfoomap[5])[wgm];
    }
}

void set_timer_mode(h_timer const timer, const struct timer_mode* const modes) {
    if(timer != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)timer;

        set_comna(tmrrgstrmap[tmr->timer_name - 1][TRM_TCCRA], modes->coma);
        set_comnb(tmrrgstrmap[tmr->timer_name - 1][TRM_TCCRA], modes->comb);
        set_wgmode(tmr, modes->wgm);

        if(tmr->class_name == TCN_8BIT) {
            set_timskn8(tmrrgstrmap[tmr->timer_name - 1][TRM_TIMSK], modes->timsk);
        }
        else if(tmr->class_name == TCN_16BIT) {
            set_comnc(tmrrgstrmap[tmr->timer_name - 1][TRM_TCCRA], modes->comc);
            set_timskn16(tmrrgstrmap[tmr->timer_name - 1][TRM_TIMSK], modes->timsk);
        }

        set_frequency(timer, modes->freq);
    }
}

struct timer_mode get_timer_mode(struct __timer_base__* const timer) {
    struct timer_mode modes;
    if(timer != NULL) {
        modes.coma = get_comna(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA]);
        modes.comb = get_comnb(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA]);
        modes.timsk = get_timskn(tmrrgstrmap[timer->timer_name - 1][TRM_TIMSK]);
        //modes.presc = timer->presc;
        if(timer->class_name == TCN_8BIT) {
            modes.wgm   = get_wgmn8(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], tmrrgstrmap[timer->timer_name - 1][TRM_TCCRB]);
        }
        else if(timer->class_name == TCN_16BIT) {
            modes.comc  = get_comnc(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA]);
            modes.wgm   = get_wgmn16(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], tmrrgstrmap[timer->timer_name - 1][TRM_TCCRB]);
        }
    }
    return modes;
}

h_timer create_timer(uint8_t timer_name, uint8_t class_name, struct timer_mode* modes, uint8_t out_ports, tmr_cmp_proc handler) {
    if(
        (
            (class_name == TCN_8BIT && (timer_name == TN_NULL || timer_name == TN_SECOND)) ||
            (class_name == TCN_16BIT && (timer_name == TN_FIRST || (TN_THIRD <= timer_name && timer_name <= TN_FIFTH)))
        ) &&
        tmr_table[class_name - 1] == NULL
    ) {
        struct __timer_base__* tmr = NULL;

        tmr = (struct __timer_base__*)malloc(sizeof(struct __timer_base__));
        if(tmr == NULL) return NULL;

        memset((void*)tmr, 0, sizeof(struct __timer_base__));
        tmr->timer_name = timer_name;
        tmr->class_name = class_name;
        tmr->active = FALSE;
        tmr->subtimer_head = NULL;
        tmr->subtimer_tail = NULL;
        tmr->subtimer_current = NULL;
        tmr->handler = handler;

        tmr_table[tmr->class_name - 1] = (h_timer)tmr;

        outports_enable((h_timer)tmr, out_ports, TRUE);
        set_timer_mode((h_timer)tmr, modes);

        //sei();

        return tmr_table[tmr->class_name - 1];
    }
    return NULL;
}

void clear_subtimer(struct __timer_base__ * const timer) {
    if(timer != NULL) {
        struct __sub_timer__ *subtimer = timer->subtimer_head;

        timer->subtimer_tail = NULL;
        timer->subtimer_current = NULL;

        while(subtimer != NULL) {
            if(subtimer->next != NULL)
                subtimer->next->prev = NULL;

            timer->subtimer_head = subtimer->next;
            subtimer->next = NULL;
            free(subtimer);
            subtimer = timer->subtimer_head;
        }
    }
}

struct __sub_timer__* find_subtimer(struct __timer_base__* const tmr, uint8_t subId) {
    if(tmr != NULL) {
        struct __sub_timer__ *subtimer = tmr->subtimer_head;

        while(subtimer != NULL) {
            if(subtimer->id == subId)
                return subtimer;
            subtimer = subtimer->next;
        }
    }

    return NULL;
}

boolean set_subtimer(h_timer const htimer, sub_tmr_proc subProc, uint8_t subId, void* subRefData) {
    if(htimer != NULL && subProc != NULL) {
        struct __timer_base__ *timer = (struct __timer_base__*)htimer;
        struct __sub_timer__ *subtimer = NULL;

        subtimer = find_subtimer(timer, subId);
        if(subtimer == NULL) {
            subtimer = (struct __sub_timer__*)malloc(sizeof(struct __sub_timer__));
            if(subtimer == NULL) return FALSE;

            memset((void*)subtimer, 0, sizeof(struct __sub_timer__));
            if(timer->subtimer_head == NULL) {
                timer->subtimer_head = subtimer;
                timer->subtimer_current = subtimer;
            }
            if(timer->subtimer_tail != NULL)
                timer->subtimer_tail->next = subtimer;

            subtimer->prev = timer->subtimer_tail;
            timer->subtimer_tail = subtimer;

            subtimer->id = subId;
        }

        subtimer->refData = subRefData;
        subtimer->subproc = subProc;

        return TRUE;
    }
    return FALSE;
}

boolean remove_subtimer(h_timer const htimer, uint8_t subId) {
    if(htimer != NULL) {
        struct __timer_base__ *timer = (struct __timer_base__*)htimer;
        struct __sub_timer__ *subtimer = NULL;

        subtimer = find_subtimer(timer, subId);

        if(subtimer != NULL){
            if(timer->subtimer_current == subtimer)
                timer->subtimer_current = subtimer->next;
            if(timer->subtimer_head == subtimer)
                timer->subtimer_head = subtimer->next;
            if(timer->subtimer_tail == subtimer)
                timer->subtimer_tail = subtimer->prev;

            if(subtimer->next != NULL)
                subtimer->next->prev = subtimer->prev;
            if(subtimer->prev != NULL)
                subtimer->prev->next = subtimer->next;

            subtimer->next = NULL;
            subtimer->prev = NULL;

            free(subtimer);

            return TRUE;
        }
    }

    return FALSE;
}

lresult base_timer_proc(h_timer const htimer, uint8_t msg) {
    if(htimer != NULL) {
        struct __timer_base__ *timer = (struct __timer_base__*)htimer;

        if(timer->subtimer_head != NULL) {
            if(timer->subtimer_current != NULL)
                return timer->subtimer_current->subproc(htimer, msg, timer->subtimer_current->id, timer->subtimer_current->refData);
            else
                timer->subtimer_current = timer->subtimer_head;
        }

        return timer->handler(htimer, msg);
    }

    return 0;
}

lresult cmp_timer_proc(uint8_t timer_name, uint8_t msg) {
    return base_timer_proc(tmr_table[timer_name - 1], msg);
}

lresult def_subtimer_proc(h_timer htimer, uint8_t msg) {
    if(htimer != NULL) {
        struct __timer_base__ *timer = (struct __timer_base__*)htimer;

        if(timer->subtimer_current != NULL) {
            timer->subtimer_current = timer->subtimer_current->next;
        }

        return base_timer_proc(htimer, msg);
    }

    return 0;
}

boolean destroy_timer(h_timer htmr) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;

        timer_stop(htmr);
        struct timer_mode modes;
        memset(&modes, 0, sizeof(struct timer_mode));
        set_timer_mode((h_timer)tmr, &modes);

        set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_OCRAH], tmrrgstrmap[tmr->timer_name - 1][TRM_OCRAL], 0);
        set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_OCRBH], tmrrgstrmap[tmr->timer_name - 1][TRM_OCRBL], 0);

        if(tmr->class_name == TCN_16BIT) {
            set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_OCRCH], tmrrgstrmap[tmr->timer_name - 1][TRM_OCRCL], 0);
            set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_ICRH],  tmrrgstrmap[tmr->timer_name - 1][TRM_ICRL],  0);
        }

        clear_subtimer(tmr);

        tmr_table[tmr->class_name - 1] = NULL;
        free(tmr);
        htmr = NULL;

        return TRUE;
    }
    return FALSE;
}

void timer_start(h_timer const htmr) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;
        tmr->active = TRUE;
        set_prescalern(tmrrgstrmap[tmr->timer_name - 1][TRM_TCCRB], tmr->presc);
    }
}

void timer_pause(h_timer const htmr) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;
        tmr->active = FALSE;
        set_prescalern(tmrrgstrmap[tmr->timer_name - 1][TRM_TCCRB], 0);
    }
}

void set_tcnt(struct __timer_base__* timer, uint16_t value) {
    if(timer != NULL) {
        set_registerx(tmrrgstrmap[timer->timer_name - 1][TRM_TCNTH], tmrrgstrmap[timer->timer_name - 1][TRM_TCNTL], value);
    }
}

void timer_stop(h_timer const htmr) {
    if(htmr != NULL) {
        timer_pause(htmr);
        set_tcnt((struct __timer_base__*)htmr, 0);
    }
}

void timer_restart(h_timer const htmr) {
    if(htmr != NULL) {
        set_tcnt((struct __timer_base__*)htmr, 0);
    }
}

uint8_t value2presc(uint16_t value) {
    uint8_t presc = 0;

    if(value > PTV_256CLK) 		presc = PTN_1024CLK;
    else if(value > PTV_64CLK) 	presc = PTN_256CLK;
    else if(value > PTV_8CLK) 	presc = PTN_64CLK;
    else if(value > PTV_1CLK) 	presc = PTN_8CLK;
    else if(value > PTV_NOCLK) 	presc = PTN_1CLK;
    else                        presc = PT_NOCLK;

    return presc;
}

uint8_t value2presc2(uint16_t value) {
    uint8_t presc = 0;

    if(value > PTV_256CLK) 		presc = PT2_1024CLK;
    else if(value > PTV_128CLK) presc = PT2_256CLK;
    else if(value > PTV_64CLK) 	presc = PT2_128CLK;
    else if(value > PTV_32CLK) 	presc = PT2_64CLK;
    else if(value > PTV_8CLK) 	presc = PT2_32CLK;
    else if(value > PTV_1CLK) 	presc = PT2_8CLK;
    else if(value > PTV_NOCLK) 	presc = PT2_1CLK;
    else                        presc = PT_NOCLK;

    return presc;
}

uint16_t presc2value(uint8_t presc) {
    uint16_t value = 0;

    if(presc == PTN_1024CLK)     value = PTV_1024CLK;
    else if(presc == PTN_256CLK) value = PTV_256CLK;
    else if(presc == PTN_64CLK)  value = PTV_64CLK;
    else if(presc == PTN_8CLK) 	 value = PTV_8CLK;
    else if(presc == PTN_1CLK) 	 value = PTV_1CLK;
    else                     	 value = PTV_NOCLK;

    return value;
}

uint16_t presc2value2(uint8_t presc) {
    uint16_t value = 0;

    if(presc == PT2_1024CLK)     value = PTV_1024CLK;
    else if(presc == PT2_256CLK) value = PTV_256CLK;
    else if(value == PT2_128CLK) value = PTV_128CLK;
    else if(presc == PT2_64CLK)  value = PTV_64CLK;
    else if(value == PT2_32CLK)  value = PTV_32CLK;
    else if(presc == PT2_8CLK) 	 value = PTV_8CLK;
    else if(presc == PT2_1CLK) 	 value = PTV_1CLK;
    else                     	 value = PTV_NOCLK;

    return value;
}

uint32_t calc_frequency_ctc(uint16_t top, uint16_t prescaler) {
    return round(F_CPU / (double)(2 * top * prescaler));
}

uint32_t calc_frequency_fastpwm(uint16_t top, uint16_t prescaler) {
    return round(F_CPU / (double)(prescaler * (1 + top)));
}

uint32_t calc_frequency_correctpwm(uint16_t top, uint16_t prescaler) {
    return round(F_CPU / (double)(2 * top * prescaler));
}

uint16_t calc_prescaler_ctc(uint32_t freq, uint16_t top) {
    return ceil(F_CPU / (double)(2 * top * freq));
}

uint16_t calc_prescaler_fastpwm(uint32_t freq, uint16_t top) {
    return ceil(F_CPU / (double)(freq * (1 + top)));
}

uint16_t calc_prescaler_correctpwm(uint32_t freq, uint16_t top) {
    return ceil(F_CPU / (double)(2 * top * freq));
}

uint16_t calc_top_ctc(uint32_t freq, uint16_t prescaler) {
    return round(F_CPU / (double)(2 * prescaler * freq));
}

uint16_t calc_top_fastpwm(uint32_t freq, uint16_t prescaler) {
    return round(F_CPU / (double)(freq * prescaler)) - 1;
}

uint16_t calc_top_correctpwm(uint32_t freq, uint16_t prescaler) {
    return round(F_CPU / (double)(2 * prescaler * freq));
}

void set_frequency(h_timer const htmr, uint16_t freq) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;

        uint16_t presc = 0;
        uint16_t xcrx = 0;

        if(tmr->mode.calc_presc != NULL) {
            presc = tmr->mode.to_prescaler(tmr->mode.calc_presc(freq, tmr->mode.max_top));
            if(tmr->mode.calc_period != NULL)
                xcrx = tmr->mode.calc_period(freq, tmr->mode.to_value(presc));

            set_presc(tmr, presc);
            if(tmr->mode.reg_xcrx != 0)
                set_registerx(tmrrgstrmap[tmr->timer_name - 1][HIREG(tmr->mode.reg_xcrx)], tmrrgstrmap[tmr->timer_name - 1][LOREG(tmr->mode.reg_xcrx)], xcrx);
        }
        else {
            set_presc(tmr, 0);
        }
    }
}

void set_porta_percent(h_timer const htmr, uint8_t percent) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;
        uint16_t valuea = 0;

        if(tmr->mode.reg_xcrx != MAKEREG(TRM_OCRAH, TRM_OCRAL)) {       //проверка, не повлияет ли подстановка на TOP (TOP OCRA) таймера
            if(tmr->mode.reg_xcrx != MAKEREG(TRM_ICRH, TRM_ICRL))
                valuea = (uint16_t)round((tmr->mode.max_top / (float)(100)) * percent);
            else {
                uint16_t value_icr = 0;
                value_icr = get_registerx(tmrrgstrmap[tmr->timer_name - 1][HIREG(tmr->mode.reg_xcrx)], tmrrgstrmap[tmr->timer_name - 1][LOREG(tmr->mode.reg_xcrx)]);
                valuea = (uint16_t)round((value_icr / (float)(100)) * percent);
            }
            set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_OCRAH], tmrrgstrmap[tmr->timer_name][TRM_OCRAL], valuea);
        }
    }
}

void set_port_percent(struct __timer_base__* const tmr, uint8_t percent, uint8_t ocrxh, uint8_t ocrxl) {
    uint16_t top = 0;
    uint16_t port_value = 0;

    if(tmr->mode.reg_xcrx != 0)
        top = get_registerx(tmrrgstrmap[tmr->timer_name - 1][HIREG(tmr->mode.reg_xcrx)], tmrrgstrmap[tmr->timer_name - 1][LOREG(tmr->mode.reg_xcrx)]);
    else
        top = tmr->mode.max_top;

    port_value = (uint16_t)round((top / (float)(100)) * percent);
    set_registerx(tmrrgstrmap[tmr->timer_name - 1][ocrxh], tmrrgstrmap[tmr->timer_name - 1][ocrxl], port_value);
}

void set_portb_percent(h_timer const htmr, uint8_t percent) {
    if(htmr != NULL) {
        set_port_percent((struct __timer_base__*)htmr, percent, TRM_OCRBH, TRM_OCRBL);
    }
}

void set_portc_percent(h_timer const htmr, uint8_t percent) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;
        if(tmr->class_name == TCN_16BIT)
            set_port_percent(tmr, percent, TRM_OCRCH, TRM_OCRCL);
    }
}

uint16_t get_timer_ptr(h_timer const htimer, uint8_t nIndex) {
    if(nIndex < sizeof(struct __timer_base__)) {
        switch(nIndex) {
            case GTP_NAME:
            case GTP_CLASS:
            case GTP_PRESC:
                return *(((uint8_t*)htimer) + nIndex);
            case GTP_CMPPROC:
            default:
                return *((uint16_t*)(((uint8_t*)htimer) + nIndex));
        }
    }
    return 0;
}

uint16_t set_timer_ptr(h_timer const htimer, uint8_t nIndex, uint16_t newValue) {
    uint16_t oldValue = 0;
    if(nIndex < sizeof(struct __timer_base__)) {
        switch(nIndex) {
            case GTP_NAME:
            case GTP_CLASS:
            case GTP_PRESC:
                break;
            case GTP_CMPPROC:
            default: {
                oldValue = get_timer_ptr(htimer, nIndex);
                *((uint16_t*)(((uint8_t*)htimer) + nIndex)) = newValue;
                break;
            }
        }
    }
    return oldValue;
}

ISR(TIMER0_OVF_vect) {
    cli();
    cmp_timer_proc(TN_NULL, TM_OVF);
    sei();
}

ISR(TIMER0_COMPA_vect) {
    cli();
    cmp_timer_proc(TN_NULL, TM_COMPA);
    sei();
}

ISR(TIMER0_COMPB_vect) {
    cli();
    cmp_timer_proc(TN_NULL, TM_COMPB);
    sei();
}

ISR(TIMER1_CAPT_vect) {
    cli();
    cmp_timer_proc(TN_FIRST, TM_CAPT);
    sei();
}

ISR(TIMER1_OVF_vect) {
    cli();
    cmp_timer_proc(TN_FIRST, TM_OVF);
    sei();
}

ISR(TIMER1_COMPA_vect) {
    cli();
    cmp_timer_proc(TN_FIRST, TM_COMPA);
    sei();
}

ISR(TIMER1_COMPB_vect) {
    cli();
    cmp_timer_proc(TN_FIRST, TM_COMPB);
    sei();
}

ISR(TIMER1_COMPC_vect) {
    cli();
    cmp_timer_proc(TN_FIRST, TM_COMPC);
    sei();
}

ISR(TIMER2_OVF_vect) {
    cli();
    cmp_timer_proc(TN_SECOND, TM_OVF);
    sei();
}

ISR(TIMER2_COMPA_vect) {
    cli();
    cmp_timer_proc(TN_SECOND, TM_COMPA);
    sei();
}

ISR(TIMER2_COMPB_vect) {
    cli();
    cmp_timer_proc(TN_SECOND, TM_COMPB);
    sei();
}

ISR(TIMER3_CAPT_vect) {
    cli();
    cmp_timer_proc(TN_THIRD, TM_CAPT);
    sei();
}

ISR(TIMER3_OVF_vect) {
    cli();
    cmp_timer_proc(TN_THIRD, TM_OVF);
    sei();
}

ISR(TIMER3_COMPA_vect) {
    cli();
    cmp_timer_proc(TN_THIRD, TM_COMPA);
    sei();
}

ISR(TIMER3_COMPB_vect) {
    cli();
    cmp_timer_proc(TN_THIRD, TM_COMPB);
    sei();
}

ISR(TIMER3_COMPC_vect) {
    cli();
    cmp_timer_proc(TN_THIRD, TM_COMPC);
    sei();
}

ISR(TIMER4_OVF_vect) {
    cli();
    cmp_timer_proc(TN_FOURTH, TM_OVF);
    sei();
}

ISR(TIMER4_COMPA_vect) {
    cli();
    cmp_timer_proc(TN_FOURTH, TM_COMPA);
    sei();
}

ISR(TIMER4_COMPB_vect) {
    cli();
    cmp_timer_proc(TN_FOURTH, TM_COMPB);
    sei();
}

ISR(TIMER4_COMPC_vect) {
    cli();
    cmp_timer_proc(TN_FOURTH, TM_COMPC);
    sei();
}

ISR(TIMER5_OVF_vect) {
    cli();
    cmp_timer_proc(TN_FIFTH, TM_OVF);
    sei();
}

ISR(TIMER5_COMPA_vect) {
    cli();
    cmp_timer_proc(TN_FIFTH, TM_COMPA);
    sei();
}

ISR(TIMER5_COMPB_vect) {
    cli();
    cmp_timer_proc(TN_FIFTH, TM_COMPB);
    sei();
}

ISR(TIMER5_COMPC_vect) {
    cli();
    cmp_timer_proc(TN_FIFTH, TM_COMPC);
    sei();
}