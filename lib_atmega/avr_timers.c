#include "avr_timers.h"

typedef uint8_t (*to_presc)(uint16_t value);
typedef uint16_t (*calc_top_presc)(uint32_t freq, uint16_t top);

uint8_t to_prescaler(uint16_t value);
uint8_t to_prescaler2(uint16_t value);

#pragma pack(push, 1)
struct __wg_mode__ {
    uint16_t        max_top;
    uint8_t         reg_xcrx;

    to_presc        to_prescaler;
    calc_top_presc  calc_presc;
    calc_top_presc  calc_period;
};

struct __timer_base__ {
    uint8_t                 timer_name;
    uint8_t                 class_name;

    uint8_t                 presc;
    boolean                 active;

    timer_event             handler;

    struct __wg_mode__      mode;
};
#pragma pack(pop)

#define TRM_TCCRA 0
#define TRM_TCCRB 1
#define TRM_TCCRC 2
#define TRM_TIMSK 3
#define TRM_TCNTH 4
#define TRM_TCNTL 5
#define TRM_OCRAH 6
#define TRM_OCRAL 7
#define TRM_OCRBH 8
#define TRM_OCRBL 9
#define TRM_OCRCH 10
#define TRM_OCRCL 11
#define TRM_ICRH  12
#define TRM_ICRL  13

#define TPM_PORTA 0
#define TPM_PORTB 1
#define TPM_PORTC 2

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
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

calc_top_presc calcperiodmap8[8] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

calc_top_presc calcprescmap16[16] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

calc_top_presc calcperiodmap16[16] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

to_presc toprescmap[2] = {
    &to_prescaler, &to_prescaler2
};

void* wgmfoomap[5] = { toprescmap, calcprescmap8, calcprescmap16, calcperiodmap8, calcperiodmap16 };

h_timer tmr_table[MAX_TMR];

void outports_enable(h_timer const timer, uint8_t out_ports, boolean flag) {
    if(timer != NULL) {
        struct __timer_base__* tmr_base = (struct __timer_base__*)timer;
        if(out_ports & OUT_PORT0)
            set_mode(tmrpinmap[tmr_base->timer_name - 1][TPM_PORTA], flag);
        if(out_ports & OUT_PORT1)
            set_mode(tmrpinmap[tmr_base->timer_name - 1][TPM_PORTB], flag);
        if((out_ports & OUT_PORT2) && tmr_base->class_name == TCN_16BIT)
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
    if(timer->timer_name == TN_SECOND)
        timer->mode.to_prescaler   = *(to_presc*)&wgmfoomap[0][1];
    else
        timer->mode.to_prescaler   = *(to_presc*)&wgmfoomap[0][0];

    if(timer->class_name == TCN_8BIT) {
        set_wgmn8(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], tmrrgstrmap[timer->timer_name - 1][TRM_TCCRB], wgm);

        timer->mode.max_top      = 255;
        if(wgm == WT8_CTC_OCRA || wgm == WT8_FAST_OCRA || wgm == WT8_PHASE_OCRA)
            timer->mode.reg_xcrx = MAKEREG(TRM_OCRAH, TRM_OCRAL);
        else
            timer->mode.reg_xcrx = 0;

        timer->mode.calc_presc      = *(calc_top_presc*)&wgmfoomap[1][wgm];
        timer->mode.calc_period     = *(calc_top_presc*)&wgmfoomap[3][wgm];
    }
    else if(timer->class_name == TCN_16BIT) {
        set_wgmn16(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], tmrrgstrmap[timer->timer_name - 1][TRM_TCCRB], wgm);

        if(wgm == WT16_PHASE_0FF || wgm == WT16_FAST_0FF)
            timer->mode.max_top = 255;
        else if(wgm == WT16_PHASE_1FF || wgm == WT16_FAST_1FF)
            timer->mode.max_top = 511;
        else if(wgm == WT16_PHASE_3FF || wgm == WT16_FAST_3FF)
            timer->mode.max_top = 1023;
        else
            timer->mode.max_top = 65535;

        if(wgm == WT16_CTC_OCRA || wgm == WT16_FAST_OCRA || wgm == WT16_PHASE_OCRA || wgm == WT16_FREQUENCY_OCRA)
            timer->mode.reg_xcrx = MAKEREG(TRM_OCRAH, TRM_OCRAL);
        else if(wgm == WT16_CTC_ICR || wgm == WT16_FAST_ICR || wgm == WT16_PHASE_ICR || wgm == WT16_FREQUENCY_ICR)
            timer->mode.reg_xcrx = MAKEREG(TRM_ICRH, TRM_ICRL);
        else
            timer->mode.reg_xcrx = 0;

        timer->mode.calc_presc     = *(calc_top_presc*)&wgmfoomap[2][wgm];
        timer->mode.calc_period    = *(calc_top_presc*)&wgmfoomap[4][wgm];
    }
}

void set_timer_mode(struct __timer_base__* const timer, const struct timer_mode* const modes) {
    if(timer != NULL) {
        set_comna(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], modes->coma);
        set_comnb(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], modes->comb);
        set_wgmode(timer, modes->wgm);

        if(timer->class_name == TCN_8BIT) {
            set_timskn8(tmrrgstrmap[timer->timer_name - 1][TRM_TIMSK], modes->timsk);
        }
        else if(timer->class_name == TCN_16BIT) {
            set_comnc(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], modes->comc);
            set_timskn16(tmrrgstrmap[timer->timer_name - 1][TRM_TIMSK], modes->timsk);
        }

        set_presc(timer, modes->presc);
    }
}

struct timer_mode get_timer_mode(struct __timer_base__* const timer) {
    struct timer_mode modes;
    if(timer != NULL) {
        modes.coma = get_comna(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA]);
        modes.comb = get_comnb(tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA]);
        modes.timsk = get_timskn(tmrrgstrmap[timer->timer_name - 1][TRM_TIMSK]);
        modes.presc = timer->presc;
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

h_timer create_timer(uint8_t timer_name, uint8_t class_name, struct timer_mode* modes, uint8_t out_ports, timer_event handler) {
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

        tmr->timer_name = timer_name;
        tmr->class_name = class_name;
        tmr->active = FALSE;
        tmr_table[tmr->class_name - 1] = (h_timer)tmr;
        tmr->handler = handler;

        outports_enable((h_timer)tmr, out_ports, TRUE);
        set_timer_mode(tmr, modes);

        sei();

        return tmr_table[tmr->class_name - 1];
    }
    return NULL;
}

boolean destroy_timer(h_timer htmr) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;

        timer_stop(htmr);
        struct timer_mode modes;
        memset(&modes, 0, sizeof(struct timer_mode));
        set_timer_mode(tmr, &modes);

        set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_OCRAH], tmrrgstrmap[tmr->timer_name - 1][TRM_OCRAL], 0);
        set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_OCRBH], tmrrgstrmap[tmr->timer_name - 1][TRM_OCRBL], 0);

        if(tmr->class_name == TCN_16BIT) {
            set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_OCRCH], tmrrgstrmap[tmr->timer_name - 1][TRM_OCRCL], 0);
            set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_ICRH],  tmrrgstrmap[tmr->timer_name - 1][TRM_ICRL],  0);
        }

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

void timer_stop(h_timer const htmr) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;
        tmr->active = FALSE;
        set_prescalern(tmrrgstrmap[tmr->timer_name - 1][TRM_TCCRB], 0);
        set_registerx(tmrrgstrmap[tmr->timer_name - 1][TRM_TCNTH], tmrrgstrmap[tmr->timer_name - 1][TRM_TCNTL], 0);
    }
}

uint32_t get_fastpwm(uint32_t freq, uint16_t value) {
    return round(F_CPU / (double)(freq * (1 + value)));
}

uint32_t get_correctpwm(uint32_t freq, uint16_t value) {
    return round(F_CPU / (double)(2 * freq * value));
}

uint8_t to_prescaler(uint16_t value) {
    uint8_t presc = 0;

    if(value > 0x100) 		presc = PTN_1024CLK;
    else if(value > 0x040) 	presc = PTN_256CLK;
    else if(value > 0x008) 	presc = PTN_64CLK;
    else if(value > 0x001) 	presc = PTN_8CLK;
    else if(value > 0x000) 	presc = PTN_1CLK;
    else 					presc = PT_NOCLK;

    return presc;
}

uint8_t to_prescaler2(uint16_t value) {
    uint8_t presc = 0;

    if(value > 0x100) 		presc = PT2_1024CLK;
    else if(value > 0x080) 	presc = PT2_256CLK;
    else if(value > 0x040) 	presc = PT2_128CLK;
    else if(value > 0x020) 	presc = PT2_64CLK;
    else if(value > 0x008) 	presc = PT2_32CLK;
    else if(value > 0x001) 	presc = PT2_8CLK;
    else if(value > 0x000) 	presc = PT2_1CLK;
    else 					presc = PT_NOCLK;

    return presc;
}

void set_frequency(h_timer const htmr, uint16_t freq) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;

        uint16_t presc = 0;
        uint16_t xcrx = 0;

        if(tmr->mode.calc_presc != NULL) {
            presc = tmr->mode.to_prescaler(tmr->mode.calc_presc(freq, tmr->mode.max_top));
            if(tmr->mode.calc_period != NULL)
                xcrx = tmr->mode.calc_period(freq, presc);

            set_presc(tmr, presc);
            if(tmr->mode.reg_xcrx != 0)
                set_registerx(tmrrgstrmap[tmr->timer_name - 1][HIREG(tmr->mode.reg_xcrx)], tmrrgstrmap[tmr->timer_name][LOREG(tmr->mode.reg_xcrx)], xcrx);
        }
        else {
            set_presc(tmr, 0);
        }
    }
}
