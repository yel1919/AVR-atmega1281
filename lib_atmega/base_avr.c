#include "base_avr.h"

uint8_t mapbit[8] = {
    0b00000001,   //1,
    0b00000011,   //3,
    0b00000111,   //7,
    0b00001111,   //15,
    0b00011111,   //31,
    0b00111111,   //63,
    0b01111111,   //127,
    0b11111111,   //255
};

uint8_t get_bit(uint8_t bit_array, uint8_t index) {
    if(index < 8) return (bit_array >> index) & 0x01;
    return 0;
}

uint8_t get_bits(uint8_t bit_array, uint8_t begin, int8_t count) {
    --count;
    if((begin + count) < 8) return (bit_array >> begin) & mapbit[count];
    return 0;
}

void get_bit_ref(uint8_t* const bit_array, uint8_t index) {
    if(index < 8) (*bit_array) = ((*bit_array) >> index) & 0x01;
}

void get_bits_ref(uint8_t* const bit_array, uint8_t begin, int8_t count) {
    --count;
    if((begin + count) < 8) (*bit_array) = ((*bit_array) >> begin) & mapbit[count];
}

uint8_t set_bit(uint8_t bit_array, uint8_t index, uint8_t value) {
    if(index < 8) return (bit_array & ~(0x01 << index)) | ((value & 0x01) << index);
    return bit_array;
}

uint8_t set_bits(uint8_t dst_bit_array, uint8_t dest_begin_idx, uint8_t src_bit_array, uint8_t src_begin_idx, int8_t count) {
    --count;

    if((dest_begin_idx + count) < 8 && (src_begin_idx + count) < 8 && count > -1)
        dst_bit_array = (dst_bit_array & ~(mapbit[count] << dest_begin_idx)) | (((src_bit_array >> src_begin_idx) & mapbit[count]) << dest_begin_idx);

    return dst_bit_array;
}

void set_bit_ref(uint8_t* const bit_array, uint8_t index, uint8_t value) {
    if(index < 8) (*bit_array) = ((*bit_array) & ~(0x01 << index)) | ((value & 0x01) << index);
}

void set_bits_ref(uint8_t* const dst_bit_array, uint8_t dest_begin_idx, uint8_t src_bit_array, uint8_t src_begin_idx, int8_t count) {
    --count;

    if((dest_begin_idx + count) < 8 && (src_begin_idx + count) < 8 && count > -1)
        (*dst_bit_array) = ((*dst_bit_array) & ~(mapbit[count] << dest_begin_idx)) | (((src_bit_array >> src_begin_idx) & mapbit[count]) << dest_begin_idx);
}

boolean set_mode(uint8_t pin, uint8_t value) {
    value = value <= 1 ? value : 1;

    switch(HIBITS(pin)) {
        case PNA: DDRA = set_bit(DDRA, LOBITS(pin), value); break;
        case PNB: DDRB = set_bit(DDRB, LOBITS(pin), value); break;
        case PNC: DDRC = set_bit(DDRC, LOBITS(pin), value); break;
        case PND: DDRD = set_bit(DDRD, LOBITS(pin), value); break;
        case PNE: DDRE = set_bit(DDRE, LOBITS(pin), value); break;
        case PNF: DDRF = set_bit(DDRF, LOBITS(pin), value); break;
        case PNG: DDRG = set_bit(DDRG, LOBITS(pin), value); break;
        default: return FALSE;
    }

    return TRUE;
}

boolean set_pin(uint8_t pin, uint8_t value) {
    value = value <= 1 ? value : 1;

    switch(HIBITS(pin)) {
        case PNA: PORTA = set_bit(PORTA, LOBITS(pin), value); break;
        case PNB: PORTB = set_bit(PORTB, LOBITS(pin), value); break;
        case PNC: PORTC = set_bit(PORTC, LOBITS(pin), value); break;
        case PND: PORTD = set_bit(PORTD, LOBITS(pin), value); break;
        case PNE: PORTE = set_bit(PORTE, LOBITS(pin), value); break;
        case PNF: PORTF = set_bit(PORTF, LOBITS(pin), value); break;
        case PNG: PORTG = set_bit(PORTG, LOBITS(pin), value); break;
        default: return FALSE;
    }

    return TRUE;
}

int main(void) {
    setup();

    while(1) {
        loop();
    }
    return 0;
}