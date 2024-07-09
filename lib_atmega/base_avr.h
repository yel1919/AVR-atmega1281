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

#ifndef F_CPU
#define F_CPU 	16000000
#endif //!F_CPU

typedef uint8_t		boolean;

extern uint8_t  get_bit(uint8_t bit_array, uint8_t index);                   // возвращает значение указанного бита
extern uint8_t  set_bit(uint8_t bit_array, uint8_t index, uint8_t value);	// устанавливает указанный бит
extern void     set_bit_ref(uint8_t* bit_array, uint8_t index, uint8_t value);
extern boolean  set_mode(uint8_t pin, uint8_t value);                        // устанавливает пин на вход/выход
extern boolean  set_pin(uint8_t pin, uint8_t value);                         // открывает/закрывает пин

extern void     setup();
extern void     loop();
//extern int main(void);

#endif //!BASE_AVR
