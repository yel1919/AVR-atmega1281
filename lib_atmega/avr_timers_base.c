#include "avr_timers_base.h"

// register TCCR[0,2]A:
//   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0
//--------|--------|--------|--------|--------|--------|--------|--------
// COM0A1 | COM0A0 | COM0B1 | COM0B0 |   -    |   -    | WGM01  | WGM00

// register TCCR[0,2]B:
//   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0
//-------|-------|-------|-------|-------|-------|-------|-------
// FOC0A | FOC0B |   -   |   -   | WGM02 | CS02  | CS01  | CS00


// register TCCR[1,3:5]A:
//   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0
//--------|--------|--------|--------|--------|--------|--------|--------
// COM1A1 | COM1A0 | COM1B1 | COM1B0 | COM1C1 | COM1C0 | WGM11  | WGM10

// register TCCR[1,3:5]B:
//   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0
//-------|-------|-------|-------|-------|-------|-------|-------
// ICNC1 | ICES1 |   -   | WGM13 | WGM12 | CS12  | CS11  | CS10


uint8_t get_comna(volatile const uint8_t *const tccrna) {
    return get_bits((*tccrna), COMNA0, 2);
}

uint8_t get_comnb(volatile const uint8_t *const tccrna) {
    return get_bits((*tccrna), COMNB0, 2);
}

uint8_t get_comnc(volatile const uint8_t *const tccrna) {
    return get_bits((*tccrna), COMNC0, 2);
}

uint8_t get_com8(volatile const uint8_t *const tccrna) {
    return get_bits((*tccrna), COMNB0, 4);
}

uint8_t get_com16(volatile const uint8_t *const tccrna) {
    return get_bits((*tccrna), COMNC0, 6);
}

uint8_t get_wgmn8(volatile const uint8_t *const tccrna, volatile const uint8_t *const tccrnb) {
    return  (get_bits((*tccrnb), WGMN2, 1) >> 1) | get_bits((*tccrna), WGMN0, 2);
}

uint8_t get_wgmn16(volatile const uint8_t *const tccrna, volatile const uint8_t *const tccrnb) {
    return  (get_bits((*tccrnb), WGMN2, 2) >> 1) | get_bits((*tccrna), WGMN0, 2);
}

uint8_t get_prescaler(volatile const uint8_t *const tccrnb) {
    return get_bits((*tccrnb), CSN0, 3);
}

void set_comna(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_bits((*tccrna), COMNA0, com, 0, 2);
}

void set_comnb(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_bits((*tccrna), COMNB0, com, 0, 2);
}

void set_comnc(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_bits((*tccrna), COMNC0, com, 0, 2);
}

void set_comn8(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_bits((*tccrna), COMNB0, com, 2, 4);
}

void set_comn16(volatile uint8_t *const tccrna, uint8_t com) {
    (*tccrna) = set_bits((*tccrna), COMNC0, com, 0, 6);
}

void set_wgmn8(volatile uint8_t *const tccrna, volatile uint8_t *const tccrnb, uint8_t wgm) {
    (*tccrna) = set_bits((*tccrna), WGMN0, wgm, 0, 2);
    (*tccrnb) = set_bits((*tccrnb), WGMN2, wgm, 2, 1);
}

void set_wgmn16(volatile uint8_t *const tccrna, volatile uint8_t *const tccrnb, uint8_t wgm) {
    (*tccrna) = set_bits((*tccrna), WGMN0, wgm, 0, 2);
    (*tccrnb) = set_bits((*tccrnb), WGMN2, wgm, 2, 2);
}

void set_prescalern(volatile uint8_t *const tccrnb, uint8_t n_presc) {
    (*tccrnb) = set_bits((*tccrnb), CSN0, n_presc, 0, 3);
}

// register TIMSK[0,2]:
//    7    |   6    |   5    |   4    |   3    |   2    |   1    |   0
// --------|--------|--------|--------|--------|--------|--------|--------
//    -    |   -    |   -    |   -    |   -    | OCIE0B | OCIE0A | TOIE0

// register TIMSK[1,3:5]:
//    7    |   6    |   5    |   4    |   3    |   2    |   1    |   0
// --------|--------|--------|--------|--------|--------|--------|--------
//    -    |   -    | ICIE1  |   -    | OCIE1C | OCIE1B | OCIE1A | TOIE1
uint8_t get_timskn(volatile const uint8_t* const timskn) {
    return (*timskn);
}

void set_timskn8(volatile uint8_t *const timskn, uint8_t timsk) {
    (*timskn) = set_bits((*timskn), TOIEN, timsk, 0, 3);
}

void set_timskn16(volatile uint8_t *const timskn, uint8_t timsk) {
    (*timskn) = set_bit(((*timskn) & 0xf0), ICIEN, get_bit(timsk, 5)) |
                set_bits(((*timskn) & 0xdf), TOIEN, timsk, 0, 4);
}

uint16_t get_registerx(volatile uint8_t *const rgstrh, volatile uint8_t *const rgstrl) {
    uint16_t value = 0;
    if(rgstrh != NULL)
        value |= (*rgstrh) << 8;
    if(rgstrl != NULL)
        value |= (*rgstrl);
    return value;;
}

void set_registerx(volatile uint8_t *const rgstrh, volatile uint8_t *const rgstrl, uint16_t value) {
    if(rgstrh != NULL)
        (*rgstrh) = (uint8_t)(value >> 8);
    if(rgstrl != NULL)
        (*rgstrl) = (uint8_t)(value);
}
