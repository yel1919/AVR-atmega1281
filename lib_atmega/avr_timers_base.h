#ifndef AVR_TIMERS_BASE
#define AVR_TIMERS_BASE

#include <avr/interrupt.h>
#include "base_avr.h"

#define COMNA1	0x07
#define COMNA0	0x06
#define COMNB1	0x05
#define COMNB0	0x04
#define COMNC1	0x03
#define COMNC0	0x02
#define WGMN1	0x01
#define WGMN0	0x00

#define FOCNA	0x07
#define FOCNB	0x06
#define ICNCN	FOCNA
#define ICESN	FOCNB
#define WGMN3	0x04
#define WGMN2	0x03
#define CSN2	0x02
#define CSN1	0x01
#define CSN0	0x00

#define ICIEN 	0x05
#define OCIENC	0x03
#define OCIENB	0x02
#define OCIENA	0x01
#define TOIEN	0x00

extern uint8_t get_comna(volatile const uint8_t *const tccrna);
extern uint8_t get_comnb(volatile const uint8_t *const tccrna);
extern uint8_t get_comnc(volatile const uint8_t *const tccrna);
extern uint8_t get_com8(volatile const uint8_t *const tccrna);
extern uint8_t get_com16(volatile const uint8_t *const tccrna);

extern uint8_t get_wgm8(volatile const uint8_t *const tccrnb, volatile const uint8_t *const tccrna);
extern uint8_t get_wgm16(volatile const uint8_t *const tccrnb, volatile const uint8_t *const tccrna);

extern uint8_t get_prescaler(volatile const uint8_t *const tccrnb);

extern void set_comna(volatile uint8_t *const tccrna, uint8_t com_a);
extern void set_comnb(volatile uint8_t *const tccrna, uint8_t com_b);
extern void set_comnc(volatile uint8_t *const tccrna, uint8_t com_c);

extern void set_comn8(volatile uint8_t *const tccrna, uint8_t com_abc);
extern void set_comn16(volatile uint8_t *const tccrna, uint8_t com_abc);

extern void set_wgmn8(volatile uint8_t *const tccrna, volatile uint8_t *const tccrnb, uint8_t wgm);
extern void set_wgmn16(volatile uint8_t *const tccrna, volatile uint8_t *const tccrnb, uint8_t wgm);

extern void set_prescalern(volatile uint8_t *const tccrnb, uint8_t n_presc);

extern void set_timskn8(volatile uint8_t *const timskn, uint8_t timsk);
extern void set_timskn16(volatile uint8_t *const timskn, uint8_t timsk);

extern void set_countern8(volatile uint8_t *const tcntn, uint8_t value);
extern void set_comparen8x(volatile uint8_t *const ocrnx, uint8_t value);

extern void set_countern16(volatile uint8_t *const tcntnh, volatile uint8_t *const tcntnl, uint16_t value);
extern void set_comparen16x(volatile uint8_t *const ocrnaxh, volatile uint8_t *const ocrnaxl, uint16_t value);
extern void set_capturen16(volatile uint8_t *const icrnh, volatile uint8_t *const icrnl, uint16_t value);

#endif //!TIMERS_AVR
