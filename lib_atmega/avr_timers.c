#include "avr_timers.h"

#pragma pack(push, 1)
struct __tmrclss__ {
    struct timer_class      stc_class;
    struct __timer_base__*  ptmr_head;
    struct __timer_base__*  ptmr_tail;
};

struct __timer_base__ {
    uint8_t                 timer_name;
    uint8_t                 class_name;

    uint8_t                 out_porta;
    uint8_t                 out_portb;

    volatile uint8_t*       tccrna;
    volatile uint8_t*       tccrnb;
    volatile uint8_t*       timskn;

    uint8_t                 presc;
    boolean                 active;

    struct __timer_base__*  next;
    struct __timer_base__*  prev;
};

struct __8bit_rgstr__ {
    volatile uint8_t* lorgstr;
};

struct __16bit_rgstr__ {
    volatile uint8_t* lorgstr;
    volatile uint8_t* hirgstr;
};

struct __8bit_timer__ {
    struct __timer_base__ base;

    struct __8bit_rgstr__ tcntn;
    struct __8bit_rgstr__ ocrna;
    struct __8bit_rgstr__ ocrnb;
};

struct __16bit_timer__ {
    struct __timer_base__   base;
    uint8_t                 out_portc;
    volatile uint8_t*       tccrnc;

    struct __16bit_rgstr__  tcntn;
    struct __16bit_rgstr__  ocrna;
    struct __16bit_rgstr__  ocrnb;
    struct __16bit_rgstr__  ocrnc;
    struct __16bit_rgstr__  icrn;
};
#pragma pack(pop)

struct __tmrclss__ tmrclss_table[MAX_TMR_CLSS];

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

struct __tmrclss__* get_class(uint8_t class_name) {
    if(TCN_NONAME < class_name && class_name <= TCN_16BIT) {
        return &tmrclss_table[class_name - 1];
    }
    return NULL;
}

boolean register_class(struct timer_class* pTmrClass) {
    if(pTmrClass != NULL) {
        struct __tmrclss__* ptmr_clss = get_class(pTmrClass->uiClassName);

        if(ptmr_clss != NULL && ptmr_clss->stc_class.uiClassName == TCN_NONAME) {
            memcpy(ptmr_clss, pTmrClass, sizeof(struct timer_class));
            ptmr_clss->ptmr_head = NULL;
            ptmr_clss->ptmr_tail = NULL;
            return TRUE;
        }
    }
    return FALSE;
}

boolean unregister_timer_class(uint8_t tmrClassName) {
    struct __tmrclss__* ptmr_clss = get_class(tmrClassName);

    if(ptmr_clss != NULL && ptmr_clss->ptmr_head == NULL) {
        memset(ptmr_clss, 0, sizeof(struct __tmrclss__));
        return TRUE;
    }

    return FALSE;
}

h_timer find_timer(uint8_t tmrClassName, uint8_t tmrName) {
    if((TCN_8BIT <= tmrClassName && tmrClassName <= TCN_16BIT) && (TN_NULL <= tmrName && tmrName <= TN_FIFTH)) {
        struct __tmrclss__* ptmr_clss = get_class(tmrClassName);

        if(ptmr_clss != NULL && ptmr_clss->stc_class.uiClassName != TCN_NONAME) {
            struct __timer_base__* p_timer = ptmr_clss->ptmr_head;

            while(p_timer != NULL) {
                if(p_timer->timer_name == tmrName)
                    return (h_timer)p_timer;
                p_timer = p_timer->next;
            }
        }
    }

    return NULL;
}

void set_base_reg(struct __timer_base__* tmr, volatile uint8_t* const tccrna, volatile uint8_t* const tccrnb, volatile uint8_t* const timskn) {
    if(tmr != NULL) {
        tmr->tccrna = tccrna;
        tmr->tccrnb = tccrnb;
        tmr->timskn = timskn;
    }
}

void set_16reg(struct __16bit_rgstr__* p16bit_reg, volatile uint8_t* const hireg, volatile uint8_t* const loreg) {
    if(p16bit_reg != NULL) {
        p16bit_reg->hirgstr = hireg;
        p16bit_reg->lorgstr = loreg;
    }
}

void set_registers(struct __timer_base__* const timer) {
    if(timer != NULL && timer->class_name != TCN_NONAME && (TN_NONAME < timer->timer_name && timer->timer_name <= TN_FIFTH)) {
        set_base_reg(timer, tmrrgstrmap[timer->timer_name - 1][TRM_TCCRA], tmrrgstrmap[timer->timer_name - 1][TRM_TCCRB], tmrrgstrmap[timer->timer_name - 1][TRM_TIMSK]);
        timer->out_porta = tmrpinmap[timer->timer_name - 1][TPM_PORTA];
        timer->out_portb = tmrpinmap[timer->timer_name - 1][TPM_PORTB];

        if(timer->class_name == TCN_8BIT) {
            struct __8bit_timer__* tmr8 = (struct __8bit_timer__*)timer;

            tmr8->tcntn.lorgstr = tmrrgstrmap[timer->timer_name - 1][TRM_TCNTL];
            tmr8->ocrna.lorgstr = tmrrgstrmap[timer->timer_name - 1][TRM_OCRAL];
            tmr8->ocrnb.lorgstr = tmrrgstrmap[timer->timer_name - 1][TRM_OCRBL];
        }
        else if(timer->class_name == TCN_16BIT) {
            struct __16bit_timer__* tmr16 = (struct __16bit_timer__*)timer;

            tmr16->out_portc = tmrpinmap[timer->timer_name - 1][TPM_PORTC];
            tmr16->tccrnc = tmrrgstrmap[timer->timer_name - 1][TRM_TCCRC];
            set_16reg(&tmr16->tcntn, tmrrgstrmap[timer->timer_name - 1][TRM_TCNTH], tmrrgstrmap[timer->timer_name - 1][TRM_TCNTL]);
            set_16reg(&tmr16->ocrna, tmrrgstrmap[timer->timer_name - 1][TRM_OCRAH], tmrrgstrmap[timer->timer_name - 1][TRM_OCRAL]);
            set_16reg(&tmr16->ocrnb, tmrrgstrmap[timer->timer_name - 1][TRM_OCRBH], tmrrgstrmap[timer->timer_name - 1][TRM_OCRBL]);
            set_16reg(&tmr16->ocrnc, tmrrgstrmap[timer->timer_name - 1][TRM_OCRCH], tmrrgstrmap[timer->timer_name - 1][TRM_OCRCL]);
            set_16reg(&tmr16->icrn,  tmrrgstrmap[timer->timer_name - 1][TRM_ICRH],  tmrrgstrmap[timer->timer_name - 1][TRM_ICRL]);
        }
    }
}

void outports_enable(h_timer const timer, uint8_t out_ports, boolean flag) {
    if(timer != NULL) {
        struct __timer_base__* tmr_base = (struct __timer_base__*)timer;
        if(out_ports & OUT_PORT0)
            set_mode(tmr_base->out_porta, flag);
        if(out_ports & OUT_PORT1)
            set_mode(tmr_base->out_portb, flag);
        if((out_ports & OUT_PORT2) && tmr_base->class_name == TCN_16BIT)
            set_mode(((struct __16bit_timer__*)tmr_base)->out_portc, flag);
    }
}

void set_presc(struct __timer_base__* const timer, uint8_t presc) {
    if(timer != NULL) {
        timer->presc = presc;
        if(timer->active)
            set_prescalern(timer->tccrnb, presc);
    }
}

void set_timer_mode(struct __timer_base__* const timer, const struct timer_mode* const modes) {
    if(timer != NULL) {
        set_comna(timer->tccrna, modes->coma);
        set_comnb(timer->tccrna, modes->comb);

        if(timer->class_name == TCN_8BIT) {
            set_wgmn8(timer->tccrna, timer->tccrnb, modes->wgm);
            set_timskn8(timer->timskn, modes->timsk);
        }
        else if(timer->class_name == TCN_16BIT) {
            set_comnc(timer->tccrna, modes->comc);
            set_wgmn16(timer->tccrna, timer->tccrnb, modes->wgm);
            set_timskn16(timer->timskn, modes->timsk);
        }
        set_presc(timer, modes->presc);
    }
}

struct timer_mode get_timer_mode(struct __timer_base__* const timer) {
    struct timer_mode modes;
    if(timer != NULL) {
        modes.coma = get_comna(timer->tccrna);
        modes.comb = get_comnb(timer->tccrna);
        modes.presc = timer->presc;
        if(timer->class_name == TCN_8BIT) {
            modes.wgm   = get_wgmn8(timer->tccrna, timer->tccrnb);
            modes.timsk = get_timskn(timer->timskn);
        }
        else if(timer->class_name == TCN_16BIT) {
            modes.comc  = get_comnc(timer->tccrna);
            modes.wgm   = get_wgmn16(timer->tccrna, timer->tccrnb);
            modes.timsk = get_timskn(timer->timskn);
        }
    }
    return modes;
}

h_timer create_timer(uint8_t timer_name, uint8_t class_name, struct timer_mode* modes, uint8_t out_ports) {
    if(
        (
            (class_name == TCN_8BIT && (timer_name == TN_NULL || timer_name == TN_SECOND)) ||
            (class_name == TCN_16BIT && (timer_name == TN_FIRST || (TN_THIRD <= timer_name && timer_name <= TN_FIFTH)))
        ) &&
        find_timer(class_name, timer_name) == NULL
    ) {
        struct __tmrclss__* ptmr_clss = get_class(class_name);
        if(ptmr_clss != NULL && ptmr_clss->stc_class.uiClassName != TCN_NONAME) {
            struct __timer_base__* tmr = NULL;

            if(class_name == TCN_8BIT)
                tmr = (struct __timer_base__*)malloc(sizeof(struct __8bit_timer__));
            else if(class_name == TCN_16BIT)
                tmr = (struct __timer_base__*)malloc(sizeof(struct __16bit_timer__));

            if(tmr == NULL) return NULL;

            tmr->timer_name = timer_name;
            tmr->class_name = class_name;
            tmr->active = FALSE;
            set_registers(tmr);
            outports_enable((h_timer)tmr, out_ports, TRUE);
            set_timer_mode(tmr, modes);

            if(ptmr_clss->ptmr_head == NULL)
                ptmr_clss->ptmr_head = tmr;
            if(ptmr_clss->ptmr_tail != NULL)
                ptmr_clss->ptmr_tail->next = tmr;

            tmr->prev = ptmr_clss->ptmr_tail;
            ptmr_clss->ptmr_tail = tmr;

            sei();

            return (h_timer)tmr;
        }
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
        if(tmr->class_name == TCN_8BIT) {
            set_comparen8x(((struct __8bit_timer__*)tmr)->ocrna.lorgstr, 0);
            set_comparen8x(((struct __8bit_timer__*)tmr)->ocrnb.lorgstr, 0);
        }
        else if(tmr->class_name == TCN_16BIT) {
            struct __16bit_timer__* tmr16 = (struct __16bit_timer__*)tmr;
            set_comparen16x(tmr16->ocrna.hirgstr, tmr16->ocrna.lorgstr, 0);
            set_comparen16x(tmr16->ocrnb.hirgstr, tmr16->ocrnb.lorgstr, 0);
            set_comparen16x(tmr16->ocrnc.hirgstr, tmr16->ocrnc.lorgstr, 0);
            set_capturen16(tmr16->icrn.hirgstr, tmr16->icrn.lorgstr, 0);
        }

        struct __tmrclss__* ptmr_clss = get_class(tmr->class_name);
        if(tmr->next == NULL)
            ptmr_clss->ptmr_tail = tmr->prev;
        else
            tmr->next->prev = tmr->prev;

        if(tmr->prev == NULL)
            ptmr_clss->ptmr_head = tmr->next;
        else
            tmr->prev->next = tmr->next;

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
        set_prescalern(tmr->tccrnb, tmr->presc);
    }
}

void timer_stop(h_timer const htmr) {
    if(htmr != NULL) {
        struct __timer_base__* tmr = (struct __timer_base__*)htmr;
        tmr->active = FALSE;
        set_prescalern(tmr->tccrnb, 0);

        if(tmr->class_name == TCN_8BIT) {
            set_countern8(((struct __8bit_timer__*)tmr)->tcntn.lorgstr, 0);
        }
        else if(tmr->class_name == TCN_16BIT) {
            struct __16bit_timer__* tmr16 = (struct __16bit_timer__*)tmr;
            set_countern16(tmr16->tcntn.hirgstr, tmr16->tcntn.lorgstr, 0);
        }
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
        uint8_t wgm = 0;
        uint16_t presc = 0;
        uint16_t xcrx = 0;

        if(tmr->class_name == TCN_8BIT) {
            wgm     = get_wgmn8(tmr->tccrna, tmr->tccrnb);
            if(wgm == WT8_FAST_OCRA) {
                presc   = to_prescaler(get_correctpwm(freq, 255));
                xcrx    = get_correctpwm(freq, presc);

                set_comparen8x(((struct __8bit_timer__*)tmr)->ocrna.lorgstr, xcrx);
            }
        }
        else if(tmr->class_name == TCN_16BIT) {
            wgm     = get_wgmn16(tmr->tccrna, tmr->tccrnb);
            if(wgm == WT16_FAST_ICR) {
                presc   = to_prescaler(get_correctpwm(freq, 255));
                xcrx    = get_correctpwm(freq, presc);

                set_presc(tmr, presc);
                set_capturen16(((struct __16bit_timer__*)tmr)->icrn.hirgstr, ((struct __16bit_timer__*)tmr)->icrn.lorgstr, xcrx);
            }
        }


    }
}
