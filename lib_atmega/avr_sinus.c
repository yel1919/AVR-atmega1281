#include "avr_sinus.h"

uint8_t sin_pulses_1pin[SIN_PERIOD];
uint8_t sin_pulses_2pin[SIN_PERIOD];

float degrees2radians(float degrees) {
    return degrees * M_PI / (float)180;
}

uint8_t to_persentuf(float value, float max_value) {
    return abs(round(value * (100 / max_value)));
}

float sin_1pin(uint8_t x) {
    return (sin(2 * M_PI * x / (float)SIN_PERIOD) + 1) * ((0xff + 1) / (float)2);
}

float sin_2pin(uint8_t x) {
    return (sin(2 * M_PI * x / (float)SIN_PERIOD)) * ((0xff + 1) / (float)2);
}

void init_sin() {
    uint8_t index = 0;
    for(;index < SIN_PERIOD; index++) {
            sin_pulses_1pin[index] = to_persentuf(sin_1pin(index), 0x100);
            sin_pulses_2pin[index] = to_persentuf(sin_2pin(index), 0x080);
    }
}

uint8_t shiftPhaseR(float radians) {
    return round((SIN_PERIOD * radians) / (float)(2 * M_PI));
}

uint8_t shiftPhaseD(float degrees) {
    return shiftPhaseR(degrees2radians(degrees));
}
