#include "cpu.h"
#include "bus.h"

register8 sm83_add8(register8 x, register8 y, cpu_state* cpu) {
    cpu->F.zero = ((uint8_t)(x + y) == 0);
    cpu->F.subtraction = false;
    cpu->F.half_carry = (((x & 0xF) + (y & 0xF)) > 0xF);
    cpu->F.carry = ((uint8_t)(x + y) > 0xFF);
    return x + y;
}

register8 sm83_sub8(register8 x, register8 y, cpu_state* cpu) {
    cpu->F.zero = ((uint8_t)(x - y) == 0);
    cpu->F.subtraction = true;
    cpu->F.half_carry = (((x & 0xF) - (y & 0xF)) > 0xF);
    cpu->F.carry = ((uint8_t)(x - y) > 0xFF);
    return x - y;
}

register16 sm83_add16(register16 x, register16 y, cpu_state* cpu) {
    cpu->F.subtraction = false;
    cpu->F.half_carry = ((uint8_t)((x & 0xFFF) + (y & 0xFFF)) > 0xFFF);
    cpu->F.carry = ((uint8_t)(x + y) > 0xFFFF);
    return x + y;
}

register8 sm83_inc8(register8 x, cpu_state* cpu) {
    cpu->F.zero = ((uint8_t)(x + 1) == 0);
    cpu->F.subtraction = false;
    cpu->F.half_carry = ((uint8_t)((x & 0xF) + (1 & 0xF)) > 0xF);
    return x + 1;
}

register8 sm83_dec8(register8 x, cpu_state* cpu) {
    cpu->F.zero = ((uint8_t)(x - 1) == 0);
    cpu->F.subtraction = true;
    cpu->F.half_carry = ((uint8_t)((x & 0xF) - (1 & 0xF)) > 0xF);
    return x - 1;
}

register8 sm83_and8(register8 x, register8 y, cpu_state* cpu) {
    cpu->F.zero        = ((uint8_t)(x & y) == 0);
    cpu->F.half_carry  = 1;
    cpu->F.carry       = 0;
    cpu->F.subtraction = 0;
    return x & y;
}

register8 sm83_xor8(register8 x, register8 y, cpu_state* cpu) {
    cpu->F.zero        = ((uint8_t)(x ^ y) == 0);
    cpu->F.half_carry  = 0;
    cpu->F.carry       = 0;
    cpu->F.subtraction = 0;
    return x ^ y;
}

register8 sm83_or8(register8 x, register8 y, cpu_state* cpu) {
    cpu->F.zero        = ((uint8_t)(x | y) == 0);
    cpu->F.half_carry  = 0;
    cpu->F.carry       = 0;
    cpu->F.subtraction = 0;
    return x | y;
}

