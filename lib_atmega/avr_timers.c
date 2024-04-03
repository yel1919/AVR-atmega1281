#include "avr_timers.h"

struct timer_class atom_table[MAX_TIMER_ATOM];

struct h_timer__ {
    struct timer_class* class;
};

struct tmr_rgstrs {
    volatile uint8_t* tccrna;
    volatile uint8_t* tccrnb;
    volatile uint8_t* timskn;
    volatile uint8_t* tcntn;
    volatile uint8_t* ocrna;
    volatile uint8_t* ocrnb;

    volatile uint8_t* tcntnh;
    volatile uint8_t* ocrnah;
    volatile uint8_t* ocrnbh;
    volatile uint8_t* ocrnch;

    volatile uint8_t* tcntnl;
    volatile uint8_t* ocrnal;
    volatile uint8_t* ocrnbl;
    volatile uint8_t* ocrncl;

    enum timer_bit tmrbit;
};

word get_timer_hash(word key) {
    return ((word)(&(atom_table[0])) + (sizeof(struct timer_class) * (key - 1)));
}

struct timer_class* get_class(word hash) {
    return &(atom_table[0]) + (hash - (word)(&(atom_table[0]))) / sizeof(struct timer_class);
}

struct tmr_rgstrs get_timer_registers(enum timer_type type) {
    struct tmr_rgstrs registers;
    memset(&registers, 0, sizeof(struct tmr_rgstrs));

    switch(type) {
        case Timer0:
            registers.tmrbit 	= _8bit;
            registers.tccrna 	= &TCCR0A;
            registers.tccrnb 	= &TCCR0B;
            registers.timskn 	= &TIMSK0;

            registers.tcntn		= &TCNT0;
            registers.ocrna 	= &OCR0A;
            registers.ocrnb 	= &OCR0B;
            break;
        case Timer1:
            registers.tmrbit 	= _16bit;
            registers.tccrna 	= &TCCR1A;
            registers.tccrnb 	= &TCCR1B;
            registers.timskn 	= &TIMSK1;

            registers.tcntnh	= &TCNT1H;
            registers.ocrnah 	= &OCR1AH;
            registers.ocrnbh 	= &OCR1BH;
            registers.ocrnch 	= &OCR1CH;

            registers.tcntnl	= &TCNT1L;
            registers.ocrnal 	= &OCR1AL;
            registers.ocrnbl 	= &OCR1BL;
            registers.ocrncl 	= &OCR1CL;
            break;
        case Timer2:
            registers.tmrbit 	= _8bit;
            registers.tccrna 	= &TCCR2A;
            registers.tccrnb 	= &TCCR2B;
            registers.timskn 	= &TIMSK2;

            registers.tcntn		= &TCNT2;
            registers.ocrna 	= &OCR2A;
            registers.ocrnb 	= &OCR2B;
            break;
        case Timer3:
            registers.tmrbit 	= _16bit;
            registers.tccrna 	= &TCCR3A;
            registers.tccrnb 	= &TCCR3B;
            registers.timskn 	= &TIMSK3;

            registers.tcntnh	= &TCNT3H;
            registers.ocrnah 	= &OCR3AH;
            registers.ocrnbh 	= &OCR3BH;
            registers.ocrnch 	= &OCR3CH;

            registers.tcntnl	= &TCNT3L;
            registers.ocrnal 	= &OCR3AL;
            registers.ocrnbl 	= &OCR3BL;
            registers.ocrncl 	= &OCR3CL;
            break;
        case Timer4:
            registers.tmrbit 	= _16bit;
            registers.tccrna 	= &TCCR4A;
            registers.tccrnb 	= &TCCR4B;
            registers.timskn 	= &TIMSK4;

            registers.tcntnh	= &TCNT1H;
            registers.ocrnah 	= &OCR4AH;
            registers.ocrnbh 	= &OCR4BH;
            registers.ocrnch 	= &OCR4CH;

            registers.tcntnl	= &TCNT4L;
            registers.ocrnal 	= &OCR4AL;
            registers.ocrnbl 	= &OCR4BL;
            registers.ocrncl 	= &OCR4CL;
            break;
        case Timer5:
            registers.tmrbit 	= _16bit;
            registers.tccrna 	= &TCCR5A;
            registers.tccrnb 	= &TCCR5B;
            registers.timskn 	= &TIMSK5;

            registers.tcntnh	= &TCNT5H;
            registers.ocrnah 	= &OCR5AH;
            registers.ocrnbh 	= &OCR5BH;
            registers.ocrnch 	= &OCR5CH;

            registers.tcntnl	= &TCNT5L;
            registers.ocrnal 	= &OCR5AL;
            registers.ocrnbl 	= &OCR5BL;
            registers.ocrncl 	= &OCR5CL;
            break;
        case NoType:
        default:
            break;
    }

    return registers;
}

atom register_class(struct timer_class* pTmrClass) {
    atom at = get_timer_hash(pTmrClass->type);
    struct timer_class* tmrcls = get_class(at);

    if(tmrcls != NULL && tmrcls->type == NoType) {
        memcpy(tmrcls, pTmrClass, sizeof(struct timer_class));
        return at;
    }

    return 0x00;
}

boolean unregister_timer_class(enum timer_type type) {
    struct timer_class* tmrcls = get_class(get_timer_hash(type));

    if(tmrcls != NULL) {
        memset(tmrcls, 0, sizeof(struct timer_class));
        return TRUE;
    }

    return FALSE;
}

h_timer create_timer(enum timer_type type, word out_port_a, word out_port_b, word out_port_c) {
    struct timer_class* tmrcls = get_class(get_timer_hash(type));

    if(tmrcls != NULL && tmrcls->type != NoType) {
        struct tmr_rgstrs tmr_regs = get_timer_registers(tmrcls->type);
        h_timer htmr = (h_timer)malloc(sizeof(h_timer__));
        htmr->class = tmrcls;

        //зменить, а то любой порт можно указать
        set_mode(out_port_a, 1);
        set_mode(out_port_b, 1);
        set_mode(out_port_c, 1);

        if(tmr_regs.tmrbit == _8bit) {
            set_comn8(tmr_regs.tccrna, tmrcls->com);
            set_wgmn8(tmr_regs.tccrna, tmr_regs.tccrnb, tmrcls->wgm);
            set_timskn8(tmr_regs.timskn, tmrcls->timsk);

            set_comparen8x(tmr_regs.ocrna, 0);
            set_comparen8x(tmr_regs.ocrnb, 0);

            sei();
        }
        else if(tmr_regs.tmrbit == _16bit) {
            set_comn16(tmr_regs.tccrna, tmrcls->com);
            set_wgmn16(tmr_regs.tccrna, tmr_regs.tccrnb, tmrcls->wgm);
            set_timskn16(tmr_regs.timskn, tmrcls->timsk);

            set_comparen16x(tmr_regs.ocrnah, tmr_regs.ocrnal, 0);
            set_comparen16x(tmr_regs.ocrnbh, tmr_regs.ocrnbl, 0);
            set_comparen16x(tmr_regs.ocrnch, tmr_regs.ocrncl, 0);

            sei();
        }
        else {
            htmr->class = NULL;
            free(htmr);
            htmr = NULL;
        }

        return htmr;
    }

    return NULL;
}

boolean destroy_timer(h_timer htmr) {
    if(htmr != NULL) {
        timer_stop(htmr);
        free(htmr);
        return TRUE;
    }
    return FALSE;
}

void timer_start(h_timer const htmr) {
    if(htmr != NULL && htmr->class != NULL) {
        struct tmr_rgstrs tmr_regs = get_timer_registers(htmr->class->type);
        set_prescalern(tmr_regs.tccrnb, htmr->class->presc);
    }
}

void timer_stop(h_timer const htmr) {
    if(htmr != NULL && htmr->class != NULL) {
        struct tmr_rgstrs tmr_regs = get_timer_registers(htmr->class->type);
        set_prescalern(tmr_regs.tccrnb, 0x00);
        if(tmr_regs.tmrbit == _8bit)
            set_countern8(tmr_regs.tcntn, 0);
        else if(tmr_regs.tmrbit == _16bit)
            set_countern16(tmr_regs.tcntnh, tmr_regs.tcntnl, 0);
    }
}
