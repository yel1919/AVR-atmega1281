#include "avr_timers_base.h"

// Регистр TCCR[0,2]A:
//   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0
//--------|--------|--------|--------|--------|--------|--------|--------
// COM0A1 | COM0A0 | COM0B1 | COM0B0 |   -    |   -    | WGM01  | WGM00

// Регистр TCCR[0,2]B:
//   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0
//-------|-------|-------|-------|-------|-------|-------|-------
// FOC0A | FOC0B |   -   |   -   | WGM02 | CS02  | CS01  | CS00


// Регистр TCCR[1,3:5]A:
//   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0
//--------|--------|--------|--------|--------|--------|--------|--------
// COM1A1 | COM1A0 | COM1B1 | COM1B0 | COM1C1 | COM1C0 | WGM11  | WGM10

// Регистр TCCR[1,3:5]B:
//   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0
//-------|-------|-------|-------|-------|-------|-------|-------
// ICNC1 | ICES1 |   -   | WGM13 | WGM12 | CS12  | CS11  | CS10


uint8_t get_comna(volatile const uint8_t *const tccrna) {
    return ((*tccrna) & 0xC0) >> 6; 					//0b11000000
}

uint8_t get_comnb(volatile const uint8_t *const tccrna) {
    return ((*tccrna) & 0x30) >> 4; 					//0b00110000
}

uint8_t get_comnc(volatile const uint8_t *const tccrna) {
    return ((*tccrna) & 0x0C) >> 2; 					//0b00001100
}

uint8_t get_com8(volatile const uint8_t *const tccrna) {
    return ((*tccrna) & 0xF0) >> 2;
}

uint8_t get_com16(volatile const uint8_t *const tccrna) {
    return get_com8(tccrna) | (((*tccrna) & 0x0C) >> 2);
}

uint8_t get_wgmn8(volatile const uint8_t *const tccrna, volatile const uint8_t *const tccrnb) {
    return  (((*tccrnb) & 0x08) >> 1) | ((*tccrna) & 0x03);			//0b00001000 | 0b00000011
}

uint8_t get_wgmn16(volatile const uint8_t *const tccrna, volatile const uint8_t *const tccrnb) {
    return  (((*tccrnb) & 0x10) >> 1) | get_wgmn8(tccrna, tccrnb); 	//0b00010000 | 0b00001011
}

uint8_t get_prescaler(volatile const uint8_t *const tccrnb) {
    return (*tccrnb) & 0x07; 						//0b00000111
}

void set_comna(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_bit(((*tccrna) & 0x3f), COMNA1, get_bit(com, 1)) |
                set_bit(((*tccrna) & 0x3f), COMNA0, get_bit(com, 0));
}

void set_comnb(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_bit(((*tccrna) & 0xcf), COMNB1, get_bit(com, 1)) |
                set_bit(((*tccrna) & 0xcf), COMNB0, get_bit(com, 0));
}

void set_comnc(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_bit(((*tccrna) & 0xf3), COMNC1, get_bit(com, 1)) |
                set_bit(((*tccrna) & 0xf3), COMNC0, get_bit(com, 0));
}

uint8_t set_com8_base(uint8_t tccrna, uint8_t com) {
    return  set_bit((tccrna & 0x0f), COMNA1, get_bit(com, 5)) |
            set_bit((tccrna & 0x0f), COMNA0, get_bit(com, 4)) |
            set_bit((tccrna & 0x0f), COMNB1, get_bit(com, 3)) |
            set_bit((tccrna & 0x0f), COMNB0, get_bit(com, 2));
}

void set_comn8(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_com8_base((*tccrna), com);
}

void set_comn16(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_com8_base(((*tccrna) & 0x03), com) |
                set_bit(((*tccrna) & 0x03), COMNC1, get_bit(com, 1)) |
                set_bit(((*tccrna) & 0x03), COMNC0, get_bit(com, 0));
}

uint8_t set_wgma_base(uint8_t tccrna, uint8_t wgm) {
    return  set_bit((tccrna & 0xfc), WGMN1, get_bit(wgm, 1)) |
            set_bit((tccrna & 0xfc), WGMN0, get_bit(wgm, 0));
}

uint8_t set_wgmb_base(uint8_t tccrnb, uint8_t wgm) {
    return  set_bit((tccrnb & 0xf7), WGMN2, get_bit(wgm, 2));
}

void set_wgmn8(volatile uint8_t *const tccrna, volatile uint8_t *const tccrnb, uint8_t wgm) {
    (*tccrna) = set_wgma_base((*tccrna), wgm);

    (*tccrnb) = set_wgmb_base((*tccrnb), wgm);
}

void set_wgmn16(volatile uint8_t *const tccrna, volatile uint8_t *const tccrnb, uint8_t wgm) {
    (*tccrna) = set_wgma_base((*tccrna), wgm);

    (*tccrnb) = set_wgmb_base(((*tccrnb) & 0xe7), wgm) |
                set_bit(((*tccrnb) & 0xe7), WGMN3, get_bit(wgm, 3));
}

void set_prescalern(volatile uint8_t *const tccrnb, uint8_t n_presc) {
    (*tccrnb) = set_bit(((*tccrnb) & 0xf8), CSN2, get_bit(n_presc, 2)) |
                set_bit(((*tccrnb) & 0xf8), CSN1, get_bit(n_presc, 1)) |
                set_bit(((*tccrnb) & 0xf8), CSN0, get_bit(n_presc, 0));
}

// Регистр TIMSK0:
//    7    |   6    |   5    |   4    |   3    |   2    |   1    |   0
// --------|--------|--------|--------|--------|--------|--------|--------
//    -    |   -    |   -    |   -    |   -    | OCIE0B | OCIE0A | TOIE0

// Регистр TIMSK1:
//    7    |   6    |   5    |   4    |   3    |   2    |   1    |   0
// --------|--------|--------|--------|--------|--------|--------|--------
//    -    |   -    | ICIE1  |   -    | OCIE1C | OCIE1B | OCIE1A | TOIE1
uint8_t get_timskn(volatile const uint8_t* const timskn) {
    return (*timskn);
}

uint8_t set_timsk_base(uint8_t timskn, uint8_t timsk) {
    return  set_bit((timskn & 0xf8), OCIENB, get_bit(timsk, 2)) |
            set_bit((timskn & 0xf8), OCIENA, get_bit(timsk, 1)) |
            set_bit((timskn & 0xf8), TOIEN,  get_bit(timsk, 0));
}

void set_timskn8(volatile uint8_t *const timskn, uint8_t timsk) {
    (*timskn) = set_timsk_base((*timskn), timsk);
}

void set_timskn16(volatile uint8_t *const timskn, uint8_t timsk) {
    (*timskn) = set_bit(((*timskn) & 0xd0), ICIEN,   get_bit(timsk, 5)) |
                set_bit(((*timskn) & 0xd0), OCIENC,  get_bit(timsk, 3)) |
                set_timsk_base(((*timskn) & 0xd0), timsk);
}

void set_registerx(volatile uint8_t *const rgstrh, volatile uint8_t *const rgstrl, uint16_t value) {
    if(rgstrh != NULL)
        (*rgstrh) = (uint8_t)(value>>8);
    if(rgstrl != NULL)
        (*rgstrl) = (uint8_t)(value);
}
