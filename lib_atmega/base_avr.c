#include "base_avr.h"

uint8_t get_bit(uint8_t bit_array, uint8_t index) {
    if(index < 0 || index >= 8) return 0;
    return (bit_array >> index) & 0x01;
}

uint8_t set_bit(uint8_t bit_array, uint8_t index, uint8_t value) {
    if(index < 0 || index >= 8) return bit_array;
    return (bit_array & ~(0x01<<index)) | ((value & 0x01)<<index);
}

void set_bit_ref(uint8_t* bit_array, uint8_t index, uint8_t value) {
    if(index < 0 || index >= 8) return;
    (*bit_array) = ((*bit_array) & ~(0x01<<index)) | ((value & 0x01)<<index);
}

boolean set_mode(uint8_t pin, uint8_t value) {
    value = value <= 1 ? value : 1;

    if(pin >= PNA0 && pin <= PNA7) {
        DDRA = (DDRA & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNB0 && pin <= PNB7) {
        DDRB = (DDRB & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNC0 && pin <= PNC7) {
        DDRC = (DDRC & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PND0 && pin <= PND7) {
        DDRD = (DDRD & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNE0 && pin <= PNE7) {
        DDRE = (DDRE & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNF0 && pin <= PNF7) {
        DDRF = (DDRF & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNG0 && pin <= PNG5) {
        DDRG = (DDRG & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else return FALSE;

    return TRUE;
}

boolean set_pin(uint8_t pin, uint8_t value) {
    value = value <= 1 ? value : 1;

    if(pin >= PNA0 && pin <= PNA7) {
        PORTA = (PORTA & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNB0 && pin <= PNB7) {
        PORTB = (PORTB & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNC0 && pin <= PNC7) {
        PORTC = (PORTC & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PND0 && pin <= PND7) {
        PORTD = (PORTD & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNE0 && pin <= PNE7) {
        PORTE = (PORTE & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNF0 && pin <= PNF7) {
        PORTF = (PORTF & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else if(pin >= PNG0 && pin <= PNG5) {
        PORTG = (PORTG & ~(1<<(pin & 0xf))) | (value<<(pin & 0xf));
    }
    else return FALSE;

    return TRUE;
}

int main(void) {
    setup();

    while(1) {
        loop();
    }
    return 0;
}