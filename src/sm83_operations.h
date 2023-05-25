#pragma once

#include "cpu.h"

register8 sm83_add8(register8 x, register8 y, cpu_state* cpu);
register16 sm83_add16(register16 x, register16 y, cpu_state* cpu);
register8 sm83_sub8(register8 x, register8 y, cpu_state* cpu);
register8 sm83_and8(register8 x, register8 y, cpu_state* cpu);
register8 sm83_xor8(register8 x, register8 y, cpu_state* cpu);
register8 sm83_or8(register8 x, register8 y, cpu_state* cpu);

// Rotate the input left, using the carry flag as a 9th bit.
register8 sm83_rotate_left(register8 reg, cpu_state* cpu);

// Rotate the input left, then copy the last bit to the carry flag.
register8 sm83_rotate_left_copy(register8 reg, cpu_state* cpu);

// Rotate the input register, using the carry flag as a 9th bit.
register8 sm83_rotate_right(register8 reg, cpu_state* cpu);

// Rotate the input right, then copy the last bit to the carry flag.
register8 sm83_rotate_right_copy(register8 reg, cpu_state* cpu);

// Swap the first and last 4 bits of an 8-bit value.
register8 sm83_swap(register8 x, cpu_state* cpu);

