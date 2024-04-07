#include "avr_timers.h"

#pragma pack(push, 1)
struct private_h_timer__ {
    struct private_timer_class* class;
    struct private_h_timer__* next;
};

struct private_timer_class {
    struct timer_class t_class;
    struct private_h_timer__* p_timer_head;
};
#pragma pack(pop)

struct private_timer_class atom_table[MAX_TIMER_ATOM];

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
    return ((word)(&(atom_table[0])) + (sizeof(struct private_timer_class) * (key - 1)));
}

struct private_timer_class* get_class(word hash) {
    return &(atom_table[0]) + (hash - (word)(&(atom_table[0]))) / sizeof(struct private_timer_class);
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

            registers.tcntn     = &TCNT0;
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
    struct private_timer_class* pr_tmr_cls = get_class(at);

    if(pr_tmr_cls != NULL && pr_tmr_cls->t_class.type == NoType) {
        memcpy(pr_tmr_cls, pTmrClass, sizeof(struct timer_class));
        pr_tmr_cls->p_timer_head = NULL;
        return at;
    }

    return 0x00;
}

boolean unregister_timer_class(enum timer_type type) {
    struct private_timer_class* pr_tmr_cls = get_class(get_timer_hash(type));

    if(pr_tmr_cls != NULL && pr_tmr_cls->p_timer_head == NULL) {
        memset(pr_tmr_cls, 0, sizeof(struct private_timer_class));
        return TRUE;
    }

    return FALSE;
}

h_timer create_timer(enum timer_type type, word out_port_a, word out_port_b, word out_port_c) {
    struct private_timer_class* pr_tmr_cls = get_class(get_timer_hash(type));

    //NoType свидетельствует о том, что класс таймера не зарегистрирован
    //p_timer_head == NULL, может существовать только единственный экземпляр таймера
    if(pr_tmr_cls != NULL && pr_tmr_cls->t_class.type != NoType && pr_tmr_cls->p_timer_head == NULL) {
        struct tmr_rgstrs tmr_regs = get_timer_registers(pr_tmr_cls->t_class.type);
        struct private_h_timer__* htmr = (struct private_h_timer__*)malloc(sizeof(struct private_h_timer__));
        htmr->class = pr_tmr_cls;
        pr_tmr_cls->p_timer_head = htmr;

        //зменить, а то любой порт можно указать
        set_mode(out_port_a, 1);
        set_mode(out_port_b, 1);
        set_mode(out_port_c, 1);

            if(tmr_regs.tmrbit == _8bit) {
            set_comn8(tmr_regs.tccrna, pr_tmr_cls->t_class.com);
            set_wgmn8(tmr_regs.tccrna, tmr_regs.tccrnb, pr_tmr_cls->t_class.wgm);
            set_timskn8(tmr_regs.timskn, pr_tmr_cls->t_class.timsk);

            set_comparen8x(tmr_regs.ocrna, 0);
            set_comparen8x(tmr_regs.ocrnb, 0);

            sei();
        }
        else if(tmr_regs.tmrbit == _16bit) {
            set_comn16(tmr_regs.tccrna, pr_tmr_cls->t_class.com);
            set_wgmn16(tmr_regs.tccrna, tmr_regs.tccrnb, pr_tmr_cls->t_class.wgm);
            set_timskn16(tmr_regs.timskn, pr_tmr_cls->t_class.timsk);

            set_comparen16x(tmr_regs.ocrnah, tmr_regs.ocrnal, 0);
            set_comparen16x(tmr_regs.ocrnbh, tmr_regs.ocrnbl, 0);
            set_comparen16x(tmr_regs.ocrnch, tmr_regs.ocrncl, 0);

            sei();
        }
        else {
            htmr->class = NULL;
            pr_tmr_cls->p_timer_head = NULL;
            free(htmr);
            htmr = NULL;
        }

        return (h_timer)htmr;
    }

    return NULL;
}

boolean destroy_timer(h_timer htmr) {
    if(htmr != NULL) {
        timer_stop(htmr);
        struct private_h_timer__* pr_htmr = (struct private_h_timer__*)htmr;
        if(pr_htmr->class != NULL)
            pr_htmr->class->p_timer_head = NULL;

        free(htmr);
        return TRUE;
    }
    return FALSE;
}

void timer_start(h_timer const htmr) {
    struct private_h_timer__ *const pr_htmr = (struct private_h_timer__ *const)htmr;

    if(pr_htmr != NULL && pr_htmr->class != NULL) {
        struct tmr_rgstrs tmr_regs = get_timer_registers(pr_htmr->class->t_class.type);
        set_prescalern(tmr_regs.tccrnb, pr_htmr->class->t_class.presc);
    }
}

void timer_stop(h_timer const htmr) {
    struct private_h_timer__ *const pr_htmr = (struct private_h_timer__ *const)htmr;

    if(pr_htmr != NULL && pr_htmr->class != NULL) {
        struct tmr_rgstrs tmr_regs = get_timer_registers(pr_htmr->class->t_class.type);
        set_prescalern(tmr_regs.tccrnb, 0x00);
        if(tmr_regs.tmrbit == _8bit)
            set_countern8(tmr_regs.tcntn, 0);
        else if(tmr_regs.tmrbit == _16bit)
            set_countern16(tmr_regs.tcntnh, tmr_regs.tcntnl, 0);
    }
}
