#pragma once

#include "cpu.h"

register8 sm83_add8(register8 x, register8 y, cpu_state* cpu);
register16 sm83_add16(register16 x, register16 y, cpu_state* cpu);
register8 sm83_sub8(register8 x, register8 y, cpu_state* cpu);
register8 sm83_inc8(register8 x, cpu_state* cpu);
register8 sm83_dec8(register8 x, cpu_state* cpu);
register8 sm83_and8(register8 x, register8 y, cpu_state* cpu);
register8 sm83_xor8(register8 x, register8 y, cpu_state* cpu);
register8 sm83_or8(register8 x, register8 y, cpu_state* cpu);

