#include "avr_timers.h"

struct timer_class atom_table[MAX_TIMER_ATOM];

struct h_timer__ {
    timer_class* class;
};

word get_timer_hash(word key) {
    return ((word)(&(atom_table[0])) + (sizeof(timer_class) * (key - 1)));
}

timer_class* get_class(word hash) {
    return &(atom_table[0]) + (hash - (word)(&(atom_table[0]))) / sizeof(timer_class);
}

atom register_class(timer_class* pTmrClass) {
    atom at = get_timer_hash(pTmrClass->type);
    timer_class* tmrcls = get_class(at);

    if(at != 0x00) {
        if(tmrcls->type == None) {
            memcpy(tmrcls, pTmrClass, sizeof(timer_class));
            return at;
        }
    }

    return 0x00;
}

boolean unregister_timer_class(enum timer_type type) {
    return FALSE;
}

h_timer create_timer(enum timer_type type, word out_port_a, word out_port_b, word out_port_c) {
    timer_class* tmrcls = get_class(get_timer_hash(type));

    if(tmrcls->type != None) {
        h_timer htmr = (h_timer)malloc(sizeof(h_timer__));
        htmr->class = tmrcls;

        //зменить, а то любой порт можно указать
        set_mode(out_port_a, 1);
        set_mode(out_port_b, 1);
        set_mode(out_port_c, 1);

        if(tmrcls->type == Timer0) {
            set_comn8(&TCCR0A, tmrcls->com);
            set_wgmn8(&TCCR0A, &TCCR0B, tmrcls->wgm);
            set_timskn8(&TIMSK0, tmrcls->timsk);

            set_comparen8x(&OCR0A, 0);
            set_comparen8x(&OCR0B, 0);

            sei();
        }
        else if(tmrcls->type == Timer1) {
            set_comn16(&TCCR1A, tmrcls->com);
            set_wgmn16(&TCCR1A, &TCCR1B, tmrcls->wgm);
            set_timskn16(&TIMSK1, tmrcls->timsk);

            set_comparen16x(&OCR1AH, &OCR1AL, 0);
            set_comparen16x(&OCR1BH, &OCR1BL, 0);
            set_comparen16x(&OCR1CH, &OCR1CL, 0);

            sei();
        }
        else if(tmrcls->type == Timer2) {
            set_comn8(&TCCR2A, tmrcls->com);
            set_wgmn8(&TCCR2A, &TCCR2B, tmrcls->wgm);
            set_timskn8(&TIMSK2, tmrcls->timsk);

            set_comparen8x(&OCR2A, 0);
            set_comparen8x(&OCR2B, 0);

            sei();
        }
        else if(tmrcls->type == Timer3) {
            set_comn16(&TCCR3A, tmrcls->com);
            set_wgmn16(&TCCR3A, &TCCR3B, tmrcls->wgm);
            set_timskn16(&TIMSK3, tmrcls->timsk);

            set_comparen16x(&OCR3AH, &OCR3AL, 0);
            set_comparen16x(&OCR3BH, &OCR3BL, 0);
            set_comparen16x(&OCR3CH, &OCR3CL, 0);

            sei();
        }
        else if(tmrcls->type == Timer4) {
            set_comn16(&TCCR4A, tmrcls->com);
            set_wgmn16(&TCCR4A, &TCCR4B, tmrcls->wgm);
            set_timskn16(&TIMSK4, tmrcls->timsk);

            set_comparen16x(&OCR4AH, &OCR4AL, 0);
            set_comparen16x(&OCR4BH, &OCR4BL, 0);
            set_comparen16x(&OCR4CH, &OCR4CL, 0);

            sei();
        }
        else if(tmrcls->type == Timer5) {
            set_comn16(&TCCR5A, tmrcls->com);
            set_wgmn16(&TCCR5A, &TCCR5B, tmrcls->wgm);
            set_timskn16(&TIMSK5, tmrcls->timsk);

            set_comparen16x(&OCR5AH, &OCR5AL, 0);
            set_comparen16x(&OCR5BH, &OCR5BL, 0);
            set_comparen16x(&OCR5CH, &OCR5CL, 0);

            sei();
        }

        return htmr;
    }

    return NULL;
}

void timer_start(h_timer htmr) {
    if(htmr != NULL && htmr->class != NULL) {
        if(htmr->class->type == Timer0) {
            set_prescalern(&TCCR0B, htmr->class->presc);
        }
        else if(htmr->class->type == Timer1) {
            set_prescalern(&TCCR1B, htmr->class->presc);
        }
        else if(htmr->class->type == Timer2) {
            set_prescalern(&TCCR2B, htmr->class->presc);
        }
        else if(htmr->class->type == Timer3) {
            set_prescalern(&TCCR3B, htmr->class->presc);
        }
        else if(htmr->class->type == Timer4) {
            set_prescalern(&TCCR4B, htmr->class->presc);
        }
        else if(htmr->class->type == Timer5) {
            set_prescalern(&TCCR5B, htmr->class->presc);
        }
    }
}

void timer_stop(h_timer htmr) {
    if(htmr != NULL && htmr->class != NULL) {
        if(htmr->class->type == Timer0) {
            set_prescalern(&TCCR0B, 0x00);
            set_countern8(&TCNT0, 0);
        }
        else if(htmr->class->type == Timer1) {
            set_prescalern(&TCCR1B, 0x00);
            set_countern16(&TCNT1H, &TCNT1L, 0);
        }
        else if(htmr->class->type == Timer2) {
            set_prescalern(&TCCR2B, 0x00);
            set_countern8(&TCNT2, 0);
        }
        else if(htmr->class->type == Timer3) {
            set_prescalern(&TCCR3B, 0x00);
            set_countern16(&TCNT3H, &TCNT3L, 0);
        }
        else if(htmr->class->type == Timer4) {
            set_prescalern(&TCCR4B, 0x00);
            set_countern16(&TCNT4H, &TCNT4L, 0);
        }
        else if(htmr->class->type == Timer5) {
            set_prescalern(&TCCR5B, 0x00);
            set_countern16(&TCNT5H, &TCNT5L, 0);
        }
    }
}
