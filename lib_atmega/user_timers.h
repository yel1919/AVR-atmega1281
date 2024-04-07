#ifndef USER_TIMERS
#define USER_TIMERS

#define CL_COM          0x01
#define CL_WGM          0x02
#define CL_PRSCLR       0x03
#define CL_TIMSK        0x04
#define CL_TMRPRC       0x05

#define TD_TCNT         0x01
#define TD_OCRA         0x02
#define TD_OCRB         0x03
#define TD_OCRC         0x04
#define TD_ICR          0x05

//COM | WGM | PRSCLR | TIMSK - 0bccwwwwww 0bwwpppttt

//COM Types
#define CT_NORMAL       0x0000
#define CT_TOGGLEOCX	0x4000
#define CT_CLEAROCX     0x8000
#define CT_SETOCX       0xc000
//

//WGM Types
#define WT_NORMAL       0x0000	//----------------
#define WT_CTC          0x0040	// 0b000000xx
#define WT_PWM          0x0080	//----------------
#define WT_FAST         0x0000	//----------------
#define WT_PHASE        0x0100	// 0b0000xx00
#define WT_FREQUENCY	0x0200	//----------------
#define WT_8BIT         0x0000	//----------------
#define WT_9BIT         0x0400	// 0b00xx0000
#define WT_10BIT        0x0800	//----------------
#define WT_TOP_FF       0x0000	//----------------
#define WT_TOP_OCR      0x1000	// 0bxx000000
#define WT_TOP_ICR      0x2000	//----------------
//

//PRESCALER Types
#define PT_NOCLK        0x0000
#define PT_1CLK         0x0008
#define PT_8CLK         0x0010
#define PT_32CLK        0x0018
#define PT_64CLK        0x0020
#define PT_128CLK       0x0028
#define PT_256CLK       0x0030
#define PT_1024CLK      0x0038
//

//TIMSK Types
#define TT_NONE         0x0000
#define TT_TOIE         0x0001
#define TT_OCIEA        0x0002
#define TT_OCIEB        0x0003
#define TT_OCIEC        0x0004
#define ICIE1           0x0005
//

#endif //!USER_TIMERS
