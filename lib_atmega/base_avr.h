#ifndef BASE_AVR
#define BASE_AVR

#include <avr/io.h>
#include "user_avr.h"

#ifndef     NULL
#define     NULL 	0
#endif   //!NULL

#ifndef     FALSE
#define     FALSE 	0
#endif   //!FALSE

#ifndef     TRUE
#define     TRUE 	1
#endif   //!TRUE

#ifndef     F_CPU
#define     F_CPU 	16000000
#endif   //!F_CPU

#define HIBITS(x)        ((((uint8_t)(x)) >> 4) & 0x0F)
#define LOBITS(x)        (((uint8_t)(x)) & 0x0F)
#define MAKEBITS(h, l)   (((((uint8_t)(h)) & 0x0F) << 4) | (((uint8_t)(l)) & 0x0F))

typedef uint8_t boolean;

extern uint8_t  get_bit(uint8_t bit_array, uint8_t index);                                      // возвращает значение указанного бита
extern uint8_t  get_bits(uint8_t bit_array, uint8_t begin, int8_t count);
extern void     get_bit_ref(uint8_t* const bit_array, uint8_t index);
extern void     get_bits_ref(uint8_t* const bit_array, uint8_t begin, int8_t count);

extern uint8_t  set_bit(uint8_t bit_array, uint8_t index, uint8_t value);                       // устанавливает указанный бит
extern uint8_t  set_bits(uint8_t dst, uint8_t dest_begin_idx, uint8_t src, uint8_t src_begin_idx, int8_t count);
extern void     set_bit_ref(uint8_t* const bit_array, uint8_t index, uint8_t value);
extern void     set_bits_ref(uint8_t* const dst_bit_array, uint8_t dest_begin_idx, uint8_t src_bit_array, uint8_t src_begin_idx, int8_t count);

extern boolean  set_mode(uint8_t pin, uint8_t value);                                           // устанавливает пин на вход/выход
extern boolean  set_pin(uint8_t pin, uint8_t value);                                            // открывает/закрывает пин

extern void     setup();
extern void     loop();
//extern int main(void);

#endif //!BASE_AVR
