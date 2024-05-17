#ifndef USER_TIMERS
#define USER_TIMERS

#include "avr_timers_base.h"

//out ports
#define NONE_OUT_PORT   0x00
#define OUT_PORT0       0x01
#define OUT_PORT1       0x02
#define OUT_PORT2       0x04
//

#define CL_COM          0x00
#define CL_WGM          0x01
#define CL_PRSCLR       0x02
#define CL_TIMSK        0x03
#define CL_TMRPRC       0x05

#define TD_TCNT         0x01
#define TD_OCRA         0x02
#define TD_OCRB         0x03
#define TD_OCRC         0x04
#define TD_ICR          0x05

//TIMER types (0xC00000) 0bxx000000 0b00000000 0b00000000
#define TT_0BIT         0x000000
#define TT_8BIT         0x400000
#define TT_16BIT        0x800000
//

//COM types (0x3F0000) 0b00xxxxxx 0b00000000 0b00000000
#define CT_NORMAL       0x000000

#define CT_TOGGLE_OCXA  0x100000
#define CT_CLEAR_OCXA   0x200000
#define CT_SET_OCXA     0x300000

#define CT_TOGGLE_OCXB  0x040000
#define CT_CLEAR_OCXB   0x080000
#define CT_SET_OCXB     0x0C0000

#define CT_TOGGLE_OCXC  0x010000
#define CT_CLEAR_OCXC   0x020000
#define CT_SET_OCXC     0x030000
//

//WGM Types (0x000f00) 0b00000000 0bxxxxxxxx 0b00000000
#define WT_NORMAL       0x000000
#define WT_PHASE        0x000100
#define WT_CTC          0x000200
#define WT_FAST         0x000400
#define WT_FREQUENCY    0x000800
//WGM Types (0x00f000)
#define WT_TOP_0FF      0x000000
#define WT_TOP_1FF      0x001000
#define WT_TOP_3FF      0x002000
#define WT_TOP_OCR      0x004000
#define WT_TOP_ICR      0x008000
//

//PRESCALER types 0b00000000 0b00000000 0bxxx00000
#define PT_NOCLK        0x000000

#define PTN_1CLK        0x000020
#define PTN_8CLK        0x000040
#define PTN_64CLK       0x000060
#define PTN_256CLK      0x000080
#define PTN_1024CLK     0x0000A0
#define PTN_TNFALLING   0x0000C0
#define PTN_TNRISING    0x0000E0

#define PT2_1CLK        0x000020
#define PT2_8CLK        0x000040
#define PT2_32CLK       0x000060
#define PT2_64CLK       0x000080
#define PT2_128CLK      0x0000A0
#define PT2_256CLK      0x0000C0
#define PT2_1024CLK     0x0000E0
//

//TIMSK Types 0b00000000 0b00000000 0b000xxxxx
#define TT_NONE         0x000000
#define TT_TOIE         0x000001
#define TT_OCIEA        0x000002
#define TT_OCIEB        0x000004
#define TT_OCIEC        0x000008
#define ICIE            0x000010
//

extern uint8_t  ucomtocom(uint32_t mode);
extern uint32_t comtoucom_3(uint8_t com_a, uint8_t com_b, uint8_t com_c);
extern uint32_t comtoucom_1(uint8_t com_abc);

extern uint8_t  uwgtowgm(uint32_t mode);
extern uint32_t wgmtouwg8(uint8_t mode);
extern uint32_t wgmtouwg16(uint8_t mode);

extern uint8_t upresctopresc(uint32_t mode);
extern uint32_t presctoupresc(uint8_t mode);

extern uint8_t utimsktotimsk(uint32_t mode);
extern uint32_t timsktoutimsk8(uint8_t mode);
extern uint32_t timsktoutimsk16(uint8_t mode);

#endif //!USER_TIMERS