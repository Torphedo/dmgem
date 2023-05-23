#include <stdlib.h>
#include <stdbool.h>

#include <logging.h>

#include "cpu.h"
#include "bus.h"
#include "machine.h"
#include "sm83_operations.h"

typedef struct {
    uint8_t unused: 4;
    bool joypad: 1;
    bool serial: 1;
    bool timer: 1;
    bool lcd_stat: 1;
    bool vblank: 1;
}interrupt_flags;

static uint8_t get_execution_time(const machine_state* machine, const cpu_state* cpu) {
    uint16_t opcode = *bus_read(cpu->PC, machine);
    switch (opcode) {
        // These instructions have variable execution times depending on the
        // state of the CPU. If they don't need to take the slower path, the
        // value can just be fetched from the table.
        case JP_NZ_U16:
        case JP_Z_U16:
        case CALL_Z_U16:
        case JP_NC_U16:
        case CALL_NC_U16:
        case JP_C_U16:
        case CALL_C_U16:
            log_error(WARNING, "Instruction encountered at $%04x needs special logic to determine its execution time. Defaulting to best-case execution time...\n", cpu->PC);
        case CALL_NZ_U16:
            if (!cpu->F.zero) { return 6; }
        case JR_Z_i8:
            if (cpu->F.zero) { return 3; }
        case JR_NZ_i8:
            if (!cpu->F.zero) { return 3; }
        case JR_C_i8:
            if (cpu->F.carry) { return 3; }
        case JR_NC_i8:
            if (!cpu->F.carry) { return 3; }

        case RET_C:
            if (cpu->F.carry) { return 5; }
        case RET_NC:
            if (!cpu->F.carry) { return 5; }
        case RET_Z:
            if (cpu->F.zero) { return 5; }
        case RET_NZ:
            if (!cpu->F.zero) { return 5; }
        default:
            return opcode_cycles[opcode];
    }
}

// Used to handle opcodes prefixed with 0xCB.
static bool execute_prefix(cpu_state* cpu, const machine_state* machine) {
    uint8_t opcode = *bus_read(cpu->PC, machine);
    switch (opcode) {
        case RR_B:
            cpu->B = sm83_rotate_register(cpu->B, cpu);
            break;
        case RR_C:
            cpu->C = sm83_rotate_register(cpu->C, cpu);
            break;
        case RR_D:
            cpu->D = sm83_rotate_register(cpu->D, cpu);
            break;
        case RR_E:
            cpu->E = sm83_rotate_register(cpu->E, cpu);
            break;
        case RR_H:
            cpu->H = sm83_rotate_register(cpu->H, cpu);
            break;
        case RR_L:
            cpu->L = sm83_rotate_register(cpu->L, cpu);
            break;
        case SWAP_B:
            cpu->B = sm83_swap(cpu->B, cpu);
            break;
        case SWAP_C:
            cpu->C = sm83_swap(cpu->C, cpu);
            break;
        case SWAP_D:
            cpu->D = sm83_swap(cpu->D, cpu);
            break;
        case SWAP_E:
            cpu->E = sm83_swap(cpu->E, cpu);
            break;
        case SWAP_H:
            cpu->H = sm83_swap(cpu->H, cpu);
            break;
        case SWAP_L:
            cpu->L = sm83_swap(cpu->L, cpu);
            break;
        case SWAP_HL:
            {
                register8 value = *bus_read(cpu->HL, machine);
                value = sm83_swap(value, cpu);
                bus_write_8_bit(cpu->HL, value, machine);
            }
            break;
        case SWAP_A:
            cpu->A = sm83_swap(cpu->A, cpu);
            break;
        case SRL_B:
            cpu->B >>= 1;
            break;
        case SRL_C:
            cpu->C >>= 1;
            break;
        case SRL_D:
            cpu->D >>= 1;
            break;
        case SRL_E:
            cpu->E >>= 1;
            break;
        case SRL_H:
            cpu->H >>= 1;
            break;
        case SRL_L:
            cpu->L >>= 1;
            break;
        case SRL_HL:
            {
                register8 value = *bus_read(cpu->HL, machine);
                value >>= 1;
                bus_write_8_bit(cpu->HL, value, machine);
            }
            break;
        case SRL_A:
            cpu->A >>= 1;
            break;
        default:
            // Enable logging if it's off
            enable_logging();
            log_error(CRITICAL, "execute_prefix(): Unknown opcode 0xCB%02x at $%04x\n", opcode, cpu->PC - 1);
            return false;
    }
    // Callee handles incrementing program counter.
    return true;
}

static bool execute_switch(cpu_state* cpu, const machine_state* machine) {
    cpu->executing = false;
    uint8_t opcode = *bus_read(cpu->PC, machine);
    // Program counter before execution so the right address is printed at the end
    uint16_t opcode_pc = cpu->PC;
    cpu->PC++; // Increment PC to save a line of code on all single-byte opcodes
    switch (opcode) {
        case NOP:
            break;
        case LD_BC_U16:
            cpu->BC = *(uint16_t*) bus_read(cpu->PC, machine);
            cpu->PC += 2;
            break;
        case INC_BC:
            cpu->BC++;
            break;
        case INC_B:
            cpu->B = sm83_add8(cpu->B, 1, cpu);
            break;
        case DEC_B:
            cpu->B = sm83_sub8(cpu->B, 1, cpu);
            break;
        case LD_B_U8:
            cpu->B = *bus_read(cpu->PC++, machine);
            break;
        case LD_U16_SP:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint16_t address_0x08 = *(uint16_t*) bus_read(cpu->PC, machine);
                bus_write_16_bit(address_0x08, cpu->SP, machine);
            }
            cpu->PC += 2;
            break;
        case INC_C:
            cpu->C = sm83_add8(cpu->C, 1, cpu);
            break;
        case DEC_C:
            cpu->C = sm83_sub8(cpu->C, 1, cpu);
            break;
        case LD_C_U8:
            cpu->C = *bus_read(cpu->PC++, machine);
            break;
        case LD_DE_U16:
            cpu->DE = *(uint16_t*) bus_read(cpu->PC, machine);
            cpu->PC += 2;
            break;
        case LD_DE_A:
            bus_write_8_bit(cpu->DE, cpu->A, machine);
            break;
        case INC_DE:
            cpu->DE++;
            break;
        case INC_D:
            cpu->D = sm83_add8(cpu->D, 1, cpu);
            break;
        case DEC_D:
            cpu->D = sm83_sub8(cpu->D, 1, cpu);
            break;
        case JR_i8:
            cpu->PC += *(int8_t*) bus_read(cpu->PC++, machine);
            break;
        case ADD_HL_DE:
            cpu->HL = sm83_add16(cpu->HL, cpu->DE, cpu);
            break;
        case LD_A_DE:
            cpu->A = *bus_read(cpu->DE, machine);
            break;
        case INC_E:
            cpu->E = sm83_add8(cpu->E, 1, cpu);
            break;
        case DEC_E:
            cpu->E = sm83_sub8(cpu->E, 1, cpu);
            break;
        case RRA:
            cpu->A = sm83_rotate_register(cpu->A, cpu);
            break;
        case JR_NZ_i8:
            // Scope allows us to declare this variable without compiler warnings
            {
                int8_t offset_0x20 = *(int8_t*) bus_read(cpu->PC++, machine);
                if (!cpu->F.zero) {
                    cpu->PC += offset_0x20;
                }
            }
            break;
        case LD_HL_U16:
            cpu->HL = *(uint16_t*) bus_read(cpu->PC, machine);
            cpu->PC += 2;
            break;
        case LDI_HL_A:
            bus_write_8_bit(cpu->HL++, cpu->A, machine);
            break;
        case INC_HL:
            cpu->HL++;
            break;
        case INC_H:
            cpu->H = sm83_add8(cpu->H, 1, cpu);
            break;
        case DEC_H:
            cpu->H = sm83_sub8(cpu->H, 1, cpu);
            break;
        case LD_H_U8:
            // Load u8 into register A and increment PC to next instruction
            cpu->H = *bus_read(cpu->PC++, machine);
            break;
        case JR_Z_i8:
            // Scope allows us to declare this variable without compiler warnings
            {
                int8_t offset_0x28 = *(int8_t*) bus_read(cpu->PC++, machine);
                if (cpu->F.zero) {
                    cpu->PC += offset_0x28;
                }
            }
            break;
        case ADD_HL_HL:
            cpu->HL = sm83_add16(cpu->HL, cpu->HL, cpu);
            break;
        case LDI_A_HL:
            cpu->A = *bus_read(cpu->HL++, machine);
            break;
        case INC_L:
            cpu->L = sm83_add8(cpu->L, 1, cpu);
            break;
        case DEC_L:
            cpu->L = sm83_sub8(cpu->L, 1, cpu);
            break;
        case JR_NC_i8:
            // Scope allows us to declare this variable without compiler warnings
            {
                int8_t offset_0x30 = *(int8_t*) bus_read(cpu->PC++, machine);
                if (!cpu->F.carry) {
                    cpu->PC += offset_0x30;
                }
            }
            break;
        case LD_SP_U16:
            cpu->SP = *(uint16_t*) bus_read(cpu->PC, machine);
            cpu->PC += 2;
            break;
        case LDD_HL_A:
            bus_write_8_bit(cpu->HL--, cpu->A, machine);
            break;
        case INC_SP:
            cpu->SP++;
            break;
        case INC_HL_8:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint16_t address = *(uint16_t*)bus_read(cpu->PC, machine);
                cpu->PC += 2;
                register8 value = *bus_read(address, machine);
                value = sm83_add8(value, 1, cpu);
                bus_write_8_bit(address, value, machine);
            }
            break;
        case DEC_HL_8:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint16_t address = *(uint16_t*)bus_read(cpu->PC, machine);
                cpu->PC += 2;
                register8 value = *bus_read(address, machine);
                value = sm83_sub8(value, 1, cpu);
                bus_write_8_bit(address, value, machine);
            }
            break;
        case INC_A:
            cpu->A = sm83_add8(cpu->A, 1, cpu);
            break;
        case DEC_A:
            cpu->A = sm83_sub8(cpu->A, 1, cpu);
            break;
        case LD_A_U8:
            // Load u8 into register A and increment PC to next instruction
            cpu->A = *bus_read(cpu->PC++, machine);
            break;
        case LD_B_B:
            cpu->B = cpu->B; // NOP, probably optimized out
            break;
        case LD_B_C:
            cpu->B = cpu->C;
            break;
        case LD_B_D:
            cpu->B = cpu->D;
            break;
        case LD_B_E:
            cpu->B = cpu->E;
            break;
        case LD_B_H:
            cpu->B = cpu->H;
            break;
        case LD_B_L:
            cpu->B = cpu->L;
            break;
        case LD_B_HL:
            cpu->B = *bus_read(cpu->HL, machine);
            break;
        case LD_B_A:
            cpu->B = cpu->A;
            break;
        case LD_C_B:
            cpu->C = cpu->B;
            break;
        case LD_C_C:
            cpu->C = cpu->C; // NOP, probably optimized out
            break;
        case LD_C_D:
            cpu->C = cpu->D;
            break;
        case LD_C_E:
            cpu->C = cpu->E;
            break;
        case LD_C_H:
            cpu->C = cpu->H;
            break;
        case LD_C_L:
            cpu->C = cpu->L;
            break;
        case LD_C_HL:
            cpu->C = *bus_read(cpu->HL, machine);
            break;
        case LD_C_A:
            cpu->C = cpu->A;
            break;
        case LD_D_B:
            cpu->D = cpu->B;
            break;
        case LD_D_C:
            cpu->D = cpu->C;
            break;
        case LD_D_D:
            cpu->D = cpu->D;
            break;
        case LD_D_E:
            cpu->D = cpu->E;
            break;
        case LD_D_H:
            cpu->D = cpu->H;
            break;
        case LD_D_L:
            cpu->D = cpu->L;
            break;
        case LD_D_HL:
            cpu->D = *bus_read(cpu->HL, machine);
            break;
        case LD_D_A:
            cpu->D = cpu->A;
            break;
        case LD_E_B:
            cpu->E = cpu->B;
            break;
        case LD_E_C:
            cpu->E = cpu->C;
            break;
        case LD_E_D:
            cpu->E = cpu->D;
            break;
        case LD_E_E:
            cpu->E = cpu->E;
            break;
        case LD_E_H:
            cpu->E = cpu->H;
            break;
        case LD_E_L:
            cpu->E = cpu->L;
            break;
        case LD_E_HL:
            cpu->E = *bus_read(cpu->HL, machine);
            break;
        case LD_E_A:
            cpu->E = cpu->A;
            break;
        case LD_H_B:
            cpu->H = cpu->B;
            break;
        case LD_H_C:
            cpu->H = cpu->C;
            break;
        case LD_H_D:
            cpu->H = cpu->D;
            break;
        case LD_H_E:
            cpu->H = cpu->E;
            break;
        case LD_H_H:
            cpu->H = cpu->H;
            break;
        case LD_H_L:
            cpu->H = cpu->L;
            break;
        case LD_H_HL:
            cpu->H = *bus_read(cpu->HL, machine);
            break;
        case LD_H_A:
            cpu->H = cpu->A;
            break;
        case LD_L_B:
            cpu->L = cpu->B;
            break;
        case LD_L_C:
            cpu->L = cpu->C;
            break;
        case LD_L_D:
            cpu->L = cpu->D;
            break;
        case LD_L_E:
            cpu->L = cpu->E;
            break;
        case LD_L_H:
            cpu->L = cpu->H;
            break;
        case LD_L_L:
            cpu->L = cpu->L;
            break;
        case LD_L_HL:
            cpu->L = *bus_read(cpu->HL, machine);
            break;
        case LD_L_A:
            cpu->L = cpu->A;
            break;
        case LD_HL_B:
            bus_write_8_bit(cpu->HL, cpu->B, machine);
            break;
        case LD_HL_C:
            bus_write_8_bit(cpu->HL, cpu->C, machine);
            break;
        case LD_HL_D:
            bus_write_8_bit(cpu->HL, cpu->D, machine);
            break;
        case LD_HL_E:
            bus_write_8_bit(cpu->HL, cpu->E, machine);
            break;
        case LD_HL_H:
            bus_write_8_bit(cpu->HL, cpu->H, machine);
            break;
        case LD_HL_L:
            bus_write_8_bit(cpu->HL, cpu->L, machine);
            break;
        case LD_HL_A:
            bus_write_8_bit(cpu->HL, cpu->A, machine);
            break;
        case LD_A_B:
            cpu->A = cpu->B;
            break;
        case LD_A_C:
            cpu->A = cpu->C;
            break;
        case LD_A_D:
            cpu->A = cpu->D;
            break;
        case LD_A_E:
            cpu->A = cpu->E;
            break;
        case LD_A_H:
            cpu->A = cpu->H;
            break;
        case LD_A_L:
            cpu->A = cpu->L;
            break;
        case LD_A_HL:
            cpu->A = *bus_read(cpu->HL, machine);
            break;
        case LD_A_A:
            cpu->A = cpu->A; // NOP, probably optimized out
            break;
        case ADD_A_B:
            cpu->F.carry = 0;
            cpu->A = sm83_add8(cpu->A, cpu->B, cpu);
            break;
        case ADD_A_C:
            cpu->F.carry = 0;
            cpu->A = sm83_add8(cpu->A, cpu->C, cpu);
            break;
        case ADD_A_D:
            cpu->F.carry = 0;
            cpu->A = sm83_add8(cpu->A, cpu->D, cpu);
            break;
        case ADD_A_E:
            cpu->F.carry = 0;
            cpu->A = sm83_add8(cpu->A, cpu->E, cpu);
            break;
        case ADD_A_H:
            cpu->F.carry = 0;
            cpu->A = sm83_add8(cpu->A, cpu->H, cpu);
            break;
        case ADD_A_L:
            cpu->F.carry = 0;
            cpu->A = sm83_add8(cpu->A, cpu->L, cpu);
            break;
        case AND_A_B:
            cpu->A = sm83_and8(cpu->A, cpu->B, cpu);
            break;
        case AND_A_C:
            cpu->A = sm83_and8(cpu->A, cpu->C, cpu);
            break;
        case AND_A_D:
            cpu->A = sm83_and8(cpu->A, cpu->D, cpu);
            break;
        case AND_A_E:
            cpu->A = sm83_and8(cpu->A, cpu->E, cpu);
            break;
        case AND_A_H:
            cpu->A = sm83_and8(cpu->A, cpu->H, cpu);
            break;
        case AND_A_L:
            cpu->A = sm83_and8(cpu->A, cpu->L, cpu);
            break;
        case AND_A_HL:
            cpu->A = sm83_and8(cpu->A, *bus_read(cpu->HL, machine), cpu);
            break;
        case AND_A_A:
            cpu->A = sm83_and8(cpu->A, cpu->A, cpu); // NOP, probably optimized out
            break;
        case XOR_A_B:
            cpu->A = sm83_xor8(cpu->A, cpu->B, cpu);
            break;
        case XOR_A_C:
            cpu->A = sm83_xor8(cpu->A, cpu->C, cpu);
            break;
        case XOR_A_D:
            cpu->A = sm83_xor8(cpu->A, cpu->D, cpu);
            break;
        case XOR_A_E:
            cpu->A = sm83_xor8(cpu->A, cpu->E, cpu);
            break;
        case XOR_A_H:
            cpu->A = sm83_xor8(cpu->A, cpu->H, cpu);
            break;
        case XOR_A_L:
            cpu->A = sm83_xor8(cpu->A, cpu->L, cpu);
            break;
        case XOR_A_HL:
            cpu->A = sm83_xor8(cpu->A, *bus_read(cpu->HL, machine), cpu);
            break;
        case XOR_A_A:
            cpu->A = sm83_xor8(cpu->A, cpu->A, cpu); // Always sets A to 0
            break;
        case OR_A_B:
            cpu->A = sm83_or8(cpu->A, cpu->B, cpu);
            break;
        case OR_A_C:
            cpu->A = sm83_or8(cpu->A, cpu->C, cpu);
            break;
        case OR_A_D:
            cpu->A = sm83_or8(cpu->A, cpu->D, cpu);
            break;
        case OR_A_E:
            cpu->A = sm83_or8(cpu->A, cpu->E, cpu);
            break;
        case OR_A_H:
            cpu->A = sm83_or8(cpu->A, cpu->H, cpu);
            break;
        case OR_A_L:
            cpu->A = sm83_or8(cpu->A, cpu->L, cpu);
            break;
        case OR_A_HL:
            cpu->A = sm83_or8(cpu->A, *bus_read(cpu->HL, machine), cpu);
            break;
        case OR_A_A:
            cpu->A = sm83_or8(cpu->A, cpu->A, cpu); // NOP, probably optimized out
            break;
        case RET_NZ:
            if (!cpu->F.zero) {
                cpu->PC = *(uint16_t*) bus_read(cpu->SP, machine);
                cpu->SP += 2;
            }
            break;
        case POP_BC:
            cpu->BC = *(uint16_t*) bus_read(cpu->SP, machine);
            cpu->SP += 2;
            break;
        case JP_16:
            // Jump to target address
            // PC was incremented before execution, so this accesses the byte(s) directly after the opcode
            cpu->PC = *(uint16_t*) bus_read(cpu->PC, machine);
            break;
        case CALL_NZ_U16:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint16_t func_addr_0xC4 = *(uint16_t*) bus_read(cpu->PC, machine);
                cpu->PC += 2; // Move PC to next opcode
                if (!cpu->F.zero) {
                    cpu->SP -= 2; // Push return address onto the stack
                    bus_write_16_bit(cpu->SP, cpu->PC, machine);
                    cpu->PC = func_addr_0xC4; // Jump to target address
                }
            }
            break;
        case PUSH_BC:
            cpu->SP -= 2;
            bus_write_16_bit(cpu->SP, cpu->BC, machine);
            break;
        case ADD_A_U8:
            cpu->F.carry = 0;
            cpu->A = sm83_add8(cpu->A, *bus_read(cpu->PC++, machine), cpu);
            break;
        case RET:
            cpu->PC = *(uint16_t*) bus_read(cpu->SP, machine);
            cpu->SP += 2;
            break;
        case PREFIX:
            if (!execute_prefix(cpu, machine)) {
                return false;
            }
            cpu->PC++; // Second half of the 2-byte opcode 0xCB XX.
            break;
        case CALL_U16:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint16_t func_addr = *(uint16_t*) bus_read(cpu->PC, machine);
                cpu->PC += 2; // Move PC to next opcode

                // Push return address onto the stack
                cpu->SP -= 2;
                bus_write_16_bit(cpu->SP, cpu->PC, machine);

                cpu->PC = func_addr; // Jump to target address
            }
            break;
        case ADC_A_U8:
            // DON'T clear the carry flag before adding, unlike add instructions
            cpu->A = sm83_add8(cpu->A, *bus_read(cpu->PC++, machine), cpu);
            break;
        case RET_NC:
            if (!cpu->F.carry) {
                cpu->PC = *(uint16_t*) bus_read(cpu->SP, machine);
                cpu->SP += 2;
            }
            break;
        case POP_DE:
            cpu->DE = *(uint16_t*) bus_read(cpu->SP, machine);
            cpu->SP += 2;
            break;
        case PUSH_DE:
            cpu->SP -= 2;
            bus_write_16_bit(cpu->SP, cpu->DE, machine);
            break;
        case SUB_A_U8:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint8_t value = *bus_read(cpu->PC++, machine);
                cpu->A = sm83_sub8(cpu->A, value, cpu);
            }
            break;
        case LD_FF00U8_A:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint8_t offset0xE0 = *bus_read(cpu->PC++, machine);
                bus_write_8_bit(0xFF00 + offset0xE0, cpu->A, machine);
            }
            break;
        case POP_HL:
            cpu->HL = *(uint16_t*) bus_read(cpu->SP, machine);
            cpu->SP += 2;
            break;
        case PUSH_HL:
            cpu->SP -= 2;
            bus_write_16_bit(cpu->SP, cpu->HL, machine);
            break;
        case AND_A_U8:
            cpu->A = sm83_and8(cpu->A, *bus_read(cpu->PC++, machine), cpu);
            break;
        case JP_HL:
            cpu->PC = cpu->HL;
            break;
        case LD_U16_A:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint16_t addr = *(uint16_t*) bus_read(cpu->PC, machine);
                bus_write_8_bit(addr, cpu->A, machine);
            }
            cpu->PC += 2;
            break;
        case XOR_A_U8:
            cpu->A = sm83_xor8(cpu->A, *bus_read(cpu->PC++, machine), cpu);
            break;
        case LD_A_FF00U8:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint8_t offset_0xF0 = *bus_read(cpu->PC++, machine);
                cpu->A = *bus_read(0xFF00 + offset_0xF0, machine);
            }
            break;
        case POP_AF:
            cpu->AF = *(uint16_t*) bus_read(cpu->SP, machine);
            cpu->SP += 2;
            break;
        case DI:
            cpu->IME = 0;
            break;
        case PUSH_AF:
            cpu->SP -= 2;
            bus_write_16_bit(cpu->SP, cpu->AF, machine);
            break;
        case OR_A_U8:
            cpu->A = sm83_or8(cpu->A, *bus_read(cpu->PC++, machine), cpu);
            break;
        case LD_A_U16:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint16_t addr = *(uint16_t*) bus_read(cpu->PC, machine);
                cpu->PC += 2;
                cpu->A = *bus_read(addr, machine);
            }
            break;
        case EI:
            cpu->IME = 0b11111111; // 0xFF
            break;
        case CP_A_U8:
            // Scope allows us to declare this variable without compiler warnings
            {
                uint8_t result = cpu->A - *bus_read(cpu->PC, machine);
                uint8_t result_for_carry = cpu->A & 0xFF - *bus_read(cpu->PC, machine) & 0xFF;
                cpu->F.zero = (result == 0);
                cpu->F.subtraction = true;
                cpu->F.half_carry = (result_for_carry > 0x0F);
                cpu->F.carry = (result_for_carry > 0xFF);
                cpu->PC++;
            }
            break;
        default:
            // Enable logging if it's off
            enable_logging();
            log_error(CRITICAL, "execute_switch(): Illegal or unimplemented instruction 0x%02x at $%04x, exiting.\n", opcode, cpu->PC - 1);
            return false;
    }
    log_error(DEBUG, "execute_switch(): Executed instruction opcode 0x%02x at $%04x\n",opcode, opcode_pc);
    return true;
}

// 0 0   0 0  0   0 0 0
//  x       y       z
//        p   q

// See https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html for more information

// This decodes instructions by patterns in their binary opcodes, rather than
// hardcoding a behaviour for each case. This is (theoretically) simpler, but
// in practice the code is much more confusing.
//
// The idea is that we only need to implement each operation once, and it will
// handle every possible combination of registers that might use the operation.
// So instead of a ton of cases for every possible register OR operation, we
// would implement OR between registers once and use the binary opcode to 
// figure out which registers to use.
static bool execute_decode(cpu_state* cpu, const machine_state* machine) {
    cpu->executing = false;
    uint8_t opcode = *bus_read(cpu->PC, machine);
    uint8_t x = (opcode & 0b11000000) >> 6;
    uint8_t y = (opcode & 0b00111000) >> 3;
    uint8_t p = (opcode & 0b00110000) >> 4;
    uint8_t q = (opcode & 0b00001000) >> 3;
    uint8_t z = (opcode & 0b00000111);

    uint8_t cc[4] = {
            !cpu->F.zero,
            cpu->F.zero,
            !cpu->F.carry,
            cpu->F.carry,
    };

    register16* register_pairs[4] = {
            &cpu->BC,
            &cpu->DE,
            &cpu->HL,
            &cpu->SP
    };

    // The NULL is (HL), to cause crashes if we try to access it without using the bus module
    register8* registers[8] = {
            &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L, NULL, &cpu->A
    };

    switch (x) {
        case 0:
            switch (z) {
                case 0:
                    // NOP (0x00)
                    if (y == 0) {
                        return true;
                    }
                    // LD (nn), SP (0x08)
                    else if (y == 1) {
                        uint16_t address = *(uint16_t*) bus_read(cpu->PC, machine);
                        cpu->PC += 2;
                        bus_write_16_bit(address, cpu->SP, machine);
                    }
                    // STOP (0x10)
                    else if (y == 2) {
                        cpu->PC++;
                    }
                    // JR i8 (0x18) and JR cc[y-4], d (0x20, 0x28, 0x30, 0x38)
                    else if ((y == 3) || (cc[y - 4])) {
                        cpu->PC += *(int8_t*) bus_read(cpu->PC, machine);
                    }
                    break;
                case 1:
                    // LD register_pairs[p], nn (0x01, 0x11, 0x21, 0x31)
                    if (q == 0) {
                        uint16_t value = *(uint16_t*) bus_read(cpu->PC, machine);
                        cpu->PC += 2;
                        *register_pairs[p] = value;
                    }
                    // ADD HL, register_pairs[p]
                    else {
                        // q is 1 bit, so this is equivalent to else if (q == 1)
                        cpu->HL = sm83_add16(cpu->HL, *register_pairs[p], cpu);
                    }
                    break;
                case 2:
                    if (q == 0) {
                        // LD (BC), A & LD (DE), A (0x02, 0x12)
                        if (p == 0 || p == 1) {
                            bus_write_8_bit(*register_pairs[p], cpu->A, machine);
                        }
                        // LD (HL+), A (0x22)
                        else if (p == 2) {
                            bus_write_8_bit(cpu->HL++, cpu->A, machine);
                        }
                        // LD (HL-), A (0x32)
                        else if (p == 3) {
                            bus_write_8_bit(cpu->HL--, cpu->A, machine);
                        }
                    }
                    else
                    {
                        // LD A, (BC) & LD A, (DE) (0x0A, 0x1A)
                        if (p == 0 || p == 1) {
                            cpu->A = *bus_read(*register_pairs[p], machine);
                        }
                            // LD A, (HL+) (0x2A)
                        else if (p == 2) {
                            cpu->A = *bus_read(cpu->HL++, machine);
                        }
                            // LD A, (HL-) (0x3A)
                        else if (p == 3) {
                            cpu->A = *bus_read(cpu->HL--, machine);
                        }
                    }
                    break;
                case 3:
                    // INC rp[p] (0x03, 0x13, 0x23, 0x33)
                    if (q == 0) {
                        *register_pairs[p] += 1;
                    }
                    // DEC rp[p] (0x0B, 0x1B, 0x2B, 0x3B)
                    else {
                        *register_pairs[p] -= 1;
                    }
                    break;
                case 4:
                    // INC (HL) 0x34)
                    if (y == 6) {
                        uint8_t value = *bus_read(cpu->HL, machine);
                        value = sm83_add8(value, 1, cpu);
                        bus_write_8_bit(cpu->HL, value, machine);
                    }
                    // INC r[y] (0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x3C)
                    else {
                        *registers[y] = sm83_add8(*registers[y], 1, cpu);
                    }
                    break;
                case 5:
                    // DEC (HL) (0x35)
                    if (y == 6) {
                        uint8_t value = *bus_read(cpu->HL, machine);
                        value = sm83_sub8(value, 1, cpu);
                        bus_write_8_bit(cpu->HL, value, machine);
                    }
                    // DEC r[y] (0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x3D)
                    else {
                        *registers[y] = sm83_sub8(*registers[y], 1, cpu);
                    }
                    break;
                case 6:
                    // LD (HL), n (0x36)
                    if (y == 6) {
                        bus_write_8_bit(cpu->HL, *bus_read(cpu->PC++, machine), machine);
                    }
                    // LD r[y], n (0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x3E)
                    else {
                        *registers[y] = *bus_read(cpu->PC++, machine);
                    }
                    break;
            }
            break;
    }
    return true;
}

bool tick(const machine_state* machine) {
    static cpu_state cpu = {
            .PC = 0x100, // Initialize program counter to ROM entry point
            .IME = 0b11111111
    };

    if (!cpu.executing) {
        cpu.remaining_execution_cycles = get_execution_time(machine, &cpu);
        cpu.executing = true;
    }
    cpu.remaining_execution_cycles--; // Update every cycle

    if (cpu.remaining_execution_cycles == 0) {
        return execute_switch(&cpu, machine);
    }
    // Copy IME byte to interrupt flag
    bus_write_8_bit(0xFFFF, cpu.IME, machine);

    return true;
}

