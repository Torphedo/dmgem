#include "cpu.h"
#include "bus.h"

register8 sm83_add8(register8 x, register8 y, cpu_state* cpu) {
    register8 result = x + y + cpu->F.carry;

    cpu->F.zero = ((uint8_t)result == 0);
    cpu->F.subtraction = false;
    cpu->F.half_carry = (((x & 0xF) + (y & 0xF)) > 0xF);
    // We cast to a u16 here so that the value can go higher than 0xFF.
    cpu->F.carry = ((uint16_t)result > 0xFF);
    return result;
}

register8 sm83_sub8(register8 x, register8 y, cpu_state* cpu) {
    cpu->F.zero = ((uint8_t)(x - y) == 0);
    cpu->F.subtraction = true;
    cpu->F.half_carry = (((x & 0xF) - (y & 0xF)) > 0xF);
    // We cast to a u16 here so that the value can underflow higher than 0xFF.
    cpu->F.carry = ((uint16_t)(x - y) > 0xFF);
    return x - y;
}

register16 sm83_add16(register16 x, register16 y, cpu_state* cpu) {
    cpu->F.subtraction = false;
    cpu->F.half_carry = ((uint32_t)((x & 0xFFF) + (y & 0xFFF)) > 0xFFF);
    // We cast to a u32 here so that the value can go higher than 0xFFFF.
    cpu->F.carry = ((uint32_t)(x + y) > 0xFFFF);
    return x + y;
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

// Rotate the input left, using the carry flag as a 9th bit.
register8 sm83_rotate_left(register8 reg, cpu_state* cpu) {
    uint8_t carry = reg & 0b00000001;
    reg = (reg << 1) | (cpu->F.carry);

    cpu->F.carry = carry;
    cpu->F.half_carry = 0;
    cpu->F.subtraction = 0;
    cpu->F.zero = (reg == 0);
    return reg;
}

// Rotate the input left, then copy the last bit to the carry flag.
register8 sm83_rotate_left_copy(register8 reg, cpu_state* cpu) {
    reg = (reg << 1) | (reg >> 7);

    cpu->F.carry = reg & 0b00000001;
    cpu->F.half_carry = 0;
    cpu->F.subtraction = 0;
    cpu->F.zero = (reg == 0);
    return reg;
}

// Rotate the input right, using the carry flag as a 9th bit.
register8 sm83_rotate_right(register8 reg, cpu_state* cpu) {
    cpu->F.subtraction = false;
    cpu->F.half_carry = false;

    uint8_t carry = reg & 0b00000001;
    reg = (cpu->F.carry << 7) | (reg >> 1);
    cpu->F.zero = (reg == 0);
    cpu->F.carry = carry;
    return reg;
}

// Rotate the input right, then copy the last bit to the carry flag.
register8 sm83_rotate_right_copy(register8 reg, cpu_state* cpu) {
    reg = (reg << 7) | (reg >> 1);

    cpu->F.subtraction = false;
    cpu->F.half_carry = false;
    cpu->F.carry = reg & 0b00000001;
    cpu->F.zero = (reg == 0);

    return reg;
}

// Swap the first and last 4 bits of an 8-bit value.
register8 sm83_swap(register8 x, cpu_state* cpu) {
    x = x << 4 | x >> 4;

    cpu->F.subtraction = 0;
    cpu->F.half_carry = 0;
    cpu->F.carry = 0;
    cpu->F.zero = (x == 0);

    return x;
}

