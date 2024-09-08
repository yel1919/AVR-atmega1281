#include "avr_sinus.h"

#define SIN_PERIOD  200
#define SIN_MODE_ID 255

#define ONE_PIN 0
#define TWO_PIN 1

#define COUNT_SIN_PARAMS 4

//#pragma pack(push, 1)
struct __sinus_pin__ {
    uint8_t     shift;
    int16_t    nCurIndex;
} __attribute__((packed, aligned(1)));

struct __sinus_param__ {
    struct __sinus_pin__    pinB;
    struct __sinus_pin__    pinC;

    uint8_t                 pins    : 2;
    uint8_t                 reserv  : 6;

    uint8_t*                sinTable;
} __attribute__((packed, aligned(1)));
//#pragma pack(pop)

uint8_t sin_pulses[2][SIN_PERIOD];
struct __sinus_param__ sin_param[COUNT_SIN_PARAMS] = {
    { { 0, 0 }, { 0, 0 }, 0, 0, NULL },
    { { 0, 0 }, { 0, 0 }, 0, 0, NULL },
    { { 0, 0 }, { 0, 0 }, 0, 0, NULL },
    { { 0, 0 }, { 0, 0 }, 0, 0, NULL }
};

float degrees2radians(float degrees) {
    return degrees * M_PI / (float)180;
}

uint8_t to_persentuf(float value, float max_value) {
    return abs(round(value * (100 / max_value)));
}

float sin_1pin(uint8_t x) {
    return (sin(2 * M_PI * x / (float)SIN_PERIOD) + 1) * ((0xff + 1) / (float)2);
}

float sin_2pin(uint8_t x) {
    return (sin(2 * M_PI * x / (float)SIN_PERIOD)) * ((0xff + 1) / (float)2);
}

uint32_t frequency_sin2mk(uint8_t sin_freq) {
    return sin_freq * (uint32_t)SIN_PERIOD;
}

uint8_t frequency_mk2sin(uint32_t mk_freq) {
    return (uint8_t)round(mk_freq / (float)SIN_PERIOD);
}

void init_sin() {
    uint8_t index = 0;
    for(;index < SIN_PERIOD; index++) {
            sin_pulses[ONE_PIN][index] = to_persentuf(sin_1pin(index), 0x100);
            sin_pulses[TWO_PIN][index] = to_persentuf(sin_2pin(index), 0x080);
    }
}

uint8_t shiftPhaseR(float radians) {
    if(0.f <= radians && radians < (2 * M_PI))
        return round((SIN_PERIOD * radians) / (float)(2 * M_PI));
    return 0;
}

uint8_t shiftPhaseD(float degrees) {
    return shiftPhaseR(degrees2radians(degrees));
}

uint8_t get_percent(uint8_t* const sin, int16_t* index, uint8_t shift) {
    if((shift + (*index)) > (SIN_PERIOD - 1))
        *index = -shift;
    if((shift + (*index)) < 0)
        *index = SIN_PERIOD - shift;
    return sin[((*index)++) + shift];
}

lresult sinus_cmp_proc(h_timer htmr, uint8_t msg, uint8_t subId, void* subRefData) {
    struct __sinus_param__* sin_param = (struct __sinus_param__*)subRefData;

    switch(msg) {
        case TM_OVF: {
            if(sin_param->sinTable == sin_pulses[ONE_PIN]) {
                if(sin_param->pins & SIN_OUT_B)
                    set_portb_percent(htmr, get_percent(sin_param->sinTable, &(sin_param->pinB.nCurIndex), sin_param->pinB.shift));
                if(sin_param->pins & SIN_OUT_C)
                    set_portc_percent(htmr, get_percent(sin_param->sinTable, &(sin_param->pinC.nCurIndex), sin_param->pinC.shift));
            }
            if(sin_param->sinTable == sin_pulses[TWO_PIN]) {
                if((sin_param->pinB.nCurIndex + sin_param->pinB.shift) < SIN_PERIOD / 2) {
                    set_portb_percent(htmr, get_percent(sin_param->sinTable, &(sin_param->pinB.nCurIndex), sin_param->pinB.shift));
                    set_portc_percent(htmr, 0);
                }
                else {
                    set_portb_percent(htmr, 0);
                    set_portc_percent(htmr, get_percent(sin_param->sinTable, &(sin_param->pinB.nCurIndex), sin_param->pinB.shift));
                }
            }
            break;
        }
        default: break;
    }

    return def_subtimer_proc(htmr, msg);
}

void init_sinus_mode(h_timer const htimer, struct sinus_mode* const sin_mode, uint8_t sinus_idx) {
    struct timer_mode tmr_modes;
    uint8_t out_ports = 0;

    // timer mode
    tmr_modes.coma = CT_NORMAL;
    if(sin_mode->sin_nport == SIN_TWO_PORT || (sin_mode->out_ports & SIN_OUT_B) != 0) {
        tmr_modes.comb = CT_CLEAR_OCXB;
        out_ports |= OUT_PORTB;
    }
    if(sin_mode->sin_nport == SIN_TWO_PORT || (sin_mode->out_ports & SIN_OUT_C) != 0) {
        tmr_modes.comc = CT_CLEAR_OCXC;
        out_ports |= OUT_PORTC;
    }

    tmr_modes.wgm = WT16_FREQUENCY_OCRA;
    tmr_modes.freq = frequency_sin2mk(sin_mode->frequency);
    tmr_modes.timsk = TT_TOIE;

    // sinus params
    if(sin_mode->sin_nport == SIN_TWO_PORT || (sin_mode->out_ports & SIN_OUT_B) != 0)
        sin_param[sinus_idx].pinB.shift = shiftPhaseD(sin_mode->shiftDegreesB);
    if(sin_mode->sin_nport == SIN_ONE_PORT && (sin_mode->out_ports & SIN_OUT_C) != 0)
        sin_param[sinus_idx].pinC.shift = shiftPhaseD(sin_mode->shiftDegreesC);

    if(sin_mode->sin_nport == SIN_ONE_PORT)
        sin_param[sinus_idx].sinTable = sin_pulses[ONE_PIN];
    if(sin_mode->sin_nport == SIN_TWO_PORT)
        sin_param[sinus_idx].sinTable = sin_pulses[TWO_PIN];

    sin_param[sinus_idx].pins = sin_mode->out_ports;
    sin_param[sinus_idx].pinB.nCurIndex = 0;
    sin_param[sinus_idx].pinC.nCurIndex = 0;

    outports_enable(htimer, OUT_PORTA, FALSE);
    outports_enable(htimer, out_ports, TRUE);
    set_timer_mode(htimer, &tmr_modes);
    set_subtimer(htimer, &sinus_cmp_proc, SIN_MODE_ID, (void*)&sin_param[sinus_idx]);
}

h_timer set_sinus_mode(h_timer const htimer, struct sinus_mode* const sin_mode) {
    if(htimer != NULL && sin_mode != NULL) {
        uint8_t tmr_class = get_timer_ptr(htimer, GTP_CLASS);
        if(tmr_class == TCN_16BIT) {
            uint8_t sin_idx = get_timer_ptr(htimer, GTP_NAME);
            sin_idx -= sin_idx == TN_FIRST ? 2 : 3;

            init_sinus_mode(htimer, sin_mode, sin_idx);
            return htimer;
        }
    }
    return NULL;
}

void set_sin_frequency(h_timer const htimer, uint8_t freq) {
    set_frequency(htimer, frequency_sin2mk(freq));
}
