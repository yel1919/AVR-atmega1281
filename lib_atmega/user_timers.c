#include "user_timers.h"

boolean is_timer_type(uint32_t mode, uint32_t type) {
    return (mode & 0xC00000) == type;
}

uint8_t ucomtocom(uint32_t mode) {
    if(!is_timer_type(mode, TT_16BIT))
        mode &= 0x3C0000;
    return (mode & 0x3F0000) >> 16;
}

uint32_t comtoucom_3(uint8_t com_a, uint8_t com_b, uint8_t com_c) {
    return (uint32_t)(((com_a & 0x03) << 4) | ((com_b & 0x03) << 2) | (com_c & 0x03)) << 16;
}

uint32_t comtoucom_1(uint8_t com_abc) {
     return (uint32_t)(com_abc & 0x3F) << 16;
}

uint8_t xuwgtowgm8(uint8_t mode, uint8_t top) {
    int8_t a = 0, b = 0;

    if(
       !((mode == 0x0a && top == 0x03) ||
       ((mode == 0x05 || mode == 0x0f) && (top == 0x00 || top == 0x003)))
    )
        return 0;

    if(mode == 0x05) a = -4;
    if(mode == 0x0a || mode == 0x0f) a = -12;

    if(top == 0x03) b = 1;

    return mode + top + a + b;
}

uint8_t xuwgtowgm16(uint8_t mode, uint8_t top) {
    int8_t a = 0, b = 0;

    if(
       (mode == 0x00 && (0x00 <= top && top <= 0x04))  ||
       ((mode == 0x0a || mode == 0x14) && (0x00 <= top && top <= 0x02))
    )
        return 0;

    if(mode == 0x05) a = -4;
    if(mode == 0x0a) a = -9;
    if(mode == 0x0f) a = -10;
    if(mode == 0x14) a = -21;

    if(top == 0x03) b = a == -9 ? 0 : 7;
    if(top == 0x04) b = a == -9 ? 7 : 5;

    return mode + top + a + b;
}

uint8_t uwgtowgm(uint32_t mode) {
    uint8_t m = 0, top = 0, wgm = (mode & 0xFF00) >> 8;

    m |= ((wgm >> 0) & 0x01) * 0x00;
    m |= ((wgm >> 0) & 0x01) * 0x05;
    m |= ((wgm >> 1) & 0x01) * 0x0a;
    m |= ((wgm >> 2) & 0x01) * 0x0f;
    m |= ((wgm >> 3) & 0x01) * 0x14;

    top |= ((wgm >> 4) & 0x01) * 0x00;
    top |= ((wgm >> 4) & 0x01) * 0x01;
    top |= ((wgm >> 5) & 0x01) * 0x02;
    top |= ((wgm >> 6) & 0x01) * 0x03;
    top |= ((wgm >> 7) & 0x01) * 0x04;

    if(is_timer_type(mode, TT_8BIT))
        wgm = xuwgtowgm8(m, top);
    else if(is_timer_type(mode, TT_16BIT))
        wgm = xuwgtowgm16(m, top);
    else
        wgm = 0;

    return wgm;
}

uint32_t xwgmtouwg(uint32_t timer_type, uint8_t mode) {
    uint8_t m = 0, top = 0;

    m = mode / 0x05;
    top = (mode - (m * 0x05));
    if(m == 0x04) m = 0x08;
    if(m == 0x03) m = 0x04;
    if(top == 0x04) top = 0x08;
    if(top == 0x03) top = 0x04;

    top = top << 4;
    return timer_type | ((m | top) << 8);
}

uint32_t wgmtouwg8(uint8_t mode) {
    int8_t a = 0, b = 0;

    if(mode > 7 || mode == 4 || mode == 6)
        return 0;

    if(mode == 1 || mode == 5) a = 4;
    if(mode == 2 || mode == 3 || mode == 7) a = 12;

    if(mode == 7 || mode == 2|| mode == 5) b = -1;

    return xwgmtouwg(TT_8BIT, mode + a + b);
}

uint32_t wgmtouwg16(uint8_t mode) {
    int8_t a = 0, b = 0;

    if(mode > 15 || mode == 13) return 0;

    if((1 <= mode && mode <= 3) || mode == 11 || mode == 10) a = 4;
    if(mode == 4 || mode == 12) a = 9;
    if((5 <= mode && mode <= 7) || mode == 15 || mode == 14) a = 10;
    if(mode == 9 || mode == 8) a = 21;

    if(mode == 11 || mode == 12|| mode == 15 || mode == 9) b = -7;
    if(mode == 10 || mode == 14 || mode == 8) b = -5;

    return xwgmtouwg(TT_16BIT, mode + a + b);
}

uint8_t upresctopresc(uint32_t mode) {
    return (uint8_t)((mode & 0xE0) >> 5);
}

uint32_t presctoupresc(uint8_t mode) {
    return (uint32_t)(mode << 5);
}

uint8_t utimsktotimsk(uint32_t mode) {
    uint8_t icie = 0;

    if(is_timer_type(mode, TT_8BIT))
        mode &= 0x07;
    else if(is_timer_type(mode, TT_16BIT)) {
        icie = (mode & ICIE) << 1;
        mode &= 0x0F;
    }
    else
        mode = 0;

    return mode | icie;
}

uint32_t timsktoutimsk8(uint8_t mode) {
    return TT_8BIT | (mode & 0x07);
}

uint32_t timsktoutimsk16(uint8_t mode) {
    mode &= 0x2F;

    uint8_t icie = mode & (ICIE << 1);
    return TT_16BIT | (mode & ~(ICIE << 1)) | (icie >> 1);
}
