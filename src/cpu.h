#pragma once
#include <stdint.h>
#include <stdbool.h>

#include "machine.h"

typedef uint8_t register8;
typedef uint16_t register16;

typedef enum {
    NOP         = 0x00,
    LD_BC_U16   = 0x01,
    LD_BC_A     = 0x02,
    INC_BC      = 0x03,
    INC_B       = 0x04,
    DEC_B       = 0x05,
    LD_B_U8     = 0x06,
    RLCA        = 0x07,
    LD_U16_SP   = 0x08,
    ADD_HL_BC   = 0x09,
    LD_A_BC     = 0x0A,
    DEC_BC      = 0x0B,
    INC_C       = 0x0C,
    DEC_C       = 0x0D,
    LD_C_U8     = 0x0E,
    RRCA        = 0x0F,

    STOP        = 0x10,
    LD_DE_U16   = 0x11,
    LD_DE_A     = 0x12,
    INC_DE      = 0x13,
    INC_D       = 0x14,
    DEC_D       = 0x15,
    LD_D_U8     = 0x16,
    RLA         = 0x17,
    JR_i8       = 0x18,
    ADD_HL_DE   = 0x19,
    LD_A_DE     = 0x1A,
    DEC_DE      = 0x1B,
    INC_E       = 0x1C,
    DEC_E       = 0x1D,
    LD_E_U8     = 0x1E,
    RRA         = 0x1F,

    JR_NZ_i8    = 0x20, // 2-3 machine cycles, array has it set to 2.
    LD_HL_U16   = 0x21,
    LDI_HL_A    = 0x22, // Inc/dec, needs careful attention
    INC_HL      = 0x23,
    INC_H       = 0x24,
    DEC_H       = 0x25,
    LD_H_U8     = 0x26,
    DAA         = 0x27,
    JR_Z_i8     = 0x28, // 2-3 machine cycles, array has it set to 2.
    ADD_HL_HL   = 0x29,
    LDI_A_HL    = 0x2A,
    DEC_HL      = 0x2B,
    INC_L       = 0x2C,
    DEC_L       = 0x2D,
    LD_L_U8     = 0x2E,
    CPL         = 0x2F,

    JR_NC_i8    = 0x30, // 2-3 machine cycles, array has it set to 2.
    LD_SP_U16   = 0x31, // Load U16 into SP
    LDD_HL_A    = 0x32, // Inc/dec, needs careful attention
    INC_SP      = 0x33,
    INC_HL_8    = 0x34, // Increment the byte in RAM at (HL)
    DEC_HL_8    = 0x35, // Decrement the byte in RAM at (HL)
    LD_HL_U8    = 0x36,
    SCF         = 0x37,
    JR_C_i8     = 0x38, // 2-3 machine cycles, array has it set to 2.
    ADD_HL_SP   = 0x39,
    LDD_A_HL    = 0x3A, // Inc/dec, needs careful attention
    DEC_SP      = 0x3B,
    INC_A       = 0x3C,
    DEC_A       = 0x3D,
    LD_A_U8     = 0x3E,
    CCF         = 0x3F,

    LD_B_B      = 0x40, // NOP?
    LD_B_C      = 0x41,
    LD_B_D      = 0x42,
    LD_B_E      = 0x43,
    LD_B_H      = 0x44,
    LD_B_L      = 0x45,
    LD_B_HL     = 0x46,
    LD_B_A      = 0x47,
    LD_C_B      = 0x48,
    LD_C_C      = 0x49,
    LD_C_D      = 0x4A,
    LD_C_E      = 0x4B,
    LD_C_H      = 0x4C,
    LD_C_L      = 0x4D,
    LD_C_HL     = 0x4E,
    LD_C_A      = 0x4F,

    LD_D_B      = 0x50,
    LD_D_C      = 0x51,
    LD_D_D      = 0x52, // NOP?
    LD_D_E      = 0x53,
    LD_D_H      = 0x54,
    LD_D_L      = 0x55,
    LD_D_HL     = 0x56,
    LD_D_A      = 0x57,
    LD_E_B      = 0x58,
    LD_E_C      = 0x59,
    LD_E_D      = 0x5A,
    LD_E_E      = 0x5B, // NOP?
    LD_E_H      = 0x5C,
    LD_E_L      = 0x5D,
    LD_E_HL     = 0x5E,
    LD_E_A      = 0x5F,

    LD_H_B      = 0x60,
    LD_H_C      = 0x61,
    LD_H_D      = 0x62,
    LD_H_E      = 0x63,
    LD_H_H      = 0x64, // NOP?
    LD_H_L      = 0x65,
    LD_H_HL     = 0x66, // NOP? Loading (HL)->H into H, equivalent to ld h, h?
    LD_H_A      = 0x67,
    LD_L_B      = 0x68,
    LD_L_C      = 0x69,
    LD_L_D      = 0x6A,
    LD_L_E      = 0x6B,
    LD_L_H      = 0x6C,
    LD_L_L      = 0x6D, // NOP?
    LD_L_HL     = 0x6E, // NOP? Loading (HL)->L into L, equivalent to ld l, l?
    LD_L_A      = 0x6F,

    LD_HL_B     = 0x70,
    LD_HL_C     = 0x71,
    LD_HL_D     = 0x72,
    LD_HL_E     = 0x73,
    LD_HL_H     = 0x74,
    LD_HL_L     = 0x75,
    HALT        = 0x76,
    LD_HL_A     = 0x77,
    LD_A_B      = 0x78,
    LD_A_C      = 0x79,
    LD_A_D      = 0x7A,
    LD_A_E      = 0x7B,
    LD_A_H      = 0x7C,
    LD_A_L      = 0x7D,
    LD_A_HL     = 0x7E,
    LD_A_A      = 0x7F, // NOP?

    ADD_A_B     = 0x80,
    ADD_A_C     = 0x81,
    ADD_A_D     = 0x82,
    ADD_A_E     = 0x83,
    ADD_A_H     = 0x84,
    ADD_A_L     = 0x85,
    ADD_A_HL    = 0x86,
    ADD_A_A     = 0x87,
    ADC_A_B     = 0x88,
    ADC_A_C     = 0x89,
    ADC_A_D     = 0x8A,
    ADC_A_E     = 0x8B,
    ADC_A_H     = 0x8C,
    ADC_A_L     = 0x8D,
    ADC_A_HL    = 0x8E,
    ADC_A_A     = 0x8F,

    SUB_A_B     = 0x90,
    SUB_A_C     = 0x91,
    SUB_A_D     = 0x92,
    SUB_A_E     = 0x93,
    SUB_A_H     = 0x94,
    SUB_A_L     = 0x95,
    SUB_A_HL    = 0x96,
    SUB_A_A     = 0x97,
    SBC_A_B     = 0x98,
    SBC_A_C     = 0x99,
    SBC_A_D     = 0x9A,
    SBC_A_E     = 0x9B,
    SBC_A_H     = 0x9C,
    SBC_A_L     = 0x9D,
    SBC_A_HL    = 0x9E,
    SBC_A_A     = 0x9F,

    AND_A_B     = 0xA0,
    AND_A_C     = 0xA1,
    AND_A_D     = 0xA2,
    AND_A_E     = 0xA3,
    AND_A_H     = 0xA4,
    AND_A_L     = 0xA5,
    AND_A_HL    = 0xA6,
    AND_A_A     = 0xA7, // NOP?
    XOR_A_B     = 0xA8,
    XOR_A_C     = 0xA9,
    XOR_A_D     = 0xAA,
    XOR_A_E     = 0xAB,
    XOR_A_H     = 0xAC,
    XOR_A_L     = 0xAD,
    XOR_A_HL    = 0xAE,
    XOR_A_A     = 0xAF,

    OR_A_B      = 0xB0,
    OR_A_C      = 0xB1,
    OR_A_D      = 0xB2,
    OR_A_E      = 0xB3,
    OR_A_H      = 0xB4,
    OR_A_L      = 0xB5,
    OR_A_HL     = 0xB6,
    OR_A_A      = 0xB7,
    CP_A_B      = 0xB8,
    CP_A_C      = 0xB9,
    CP_A_D      = 0xBA,
    CP_A_E      = 0xBB,
    CP_A_H      = 0xBC,
    CP_A_L      = 0xBD,
    CP_A_HL     = 0xBE,
    CP_A_A      = 0xBF,

    RET_NZ      = 0xC0, // 2-5 machine cycles, table has it set at 2.
    POP_BC      = 0xC1,
    JP_NZ_U16   = 0xC2, // 3-4 machine cycles, table has it set at 3.
    JP_16       = 0xC3,
    CALL_NZ_U16 = 0xC4, // 3-6 machine cycles, table has it set at 3.
    PUSH_BC     = 0xC5,
    ADD_A_U8    = 0xC6,
    RST_00      = 0xC7, // Reset (jump to $0000)
    RET_Z       = 0xC8, // 2-5 machine cycles, table has it set at 2.
    RET         = 0xC9,
    JP_Z_U16    = 0xCA, // 3-4 machine cycles, table has it set at 3.
    PREFIX      = 0xCB,
    CALL_Z_U16  = 0xCC, // 3-6 machine cycles, table has it set at 3.
    CALL_U16    = 0xCD,
    ADC_A_U8    = 0xCE,
    RST_08      = 0xCF, // Reset (jump to $0008)

    RET_NC      = 0xD0, // 2-5 machine cycles, table has it set at 2.
    POP_DE      = 0xD1,
    JP_NC_U16   = 0xD2, // 3-4 machine cycles, table has it set at 3.
    ILLEGAL_D3  = 0xD3,
    CALL_NC_U16 = 0xD4, // 3-6 machine cycles, table has it set at 3.
    PUSH_DE     = 0xD5,
    SUB_A_U8    = 0xD6,
    RST_10      = 0xD7, // Reset (jump to $0010)
    RET_C       = 0xD8, // 2-5 machine cycles, table has it set at 2.
    RETI        = 0xD9,
    JP_C_U16    = 0xDA, // 3-4 machine cycles, table has it set at 3.
    ILLEGAL_DB  = 0xDB,
    CALL_C_U16  = 0xDC, // 3-6 machine cycles, table has it set at 3.
    ILLEGAL_DD  = 0xDD,
    SBC_A_U8    = 0xDE,
    RST_18      = 0xDF, // Reset (jump to $0018)

    LD_FF00U8_A = 0xE0, // Write register A to IO port address $FF00 + a u8
    POP_HL      = 0xE1,
    LD_FF00_C_A = 0xE2, // Write register A to IO port address $FF00 + register C
    ILLEGAL_E3  = 0xE3,
    ILLEGAL_E4  = 0xE4,
    PUSH_HL     = 0xE5,
    AND_A_U8    = 0xE6,
    RST_20      = 0xE7, // Reset (jump to $0020)
    ADD_SP_i8   = 0xE8,
    JP_HL       = 0xE9,
    LD_U16_A    = 0xEA,
    ILLEGAL_EB  = 0xEB,
    ILLEGAL_EC  = 0xEC,
    ILLEGAL_ED  = 0xED,
    XOR_A_U8    = 0xEE,
    RST_28      = 0xEF, // Reset (jump to $0028)

    LD_A_FF00U8 = 0xF0,
    POP_AF      = 0xF1,
    LD_A_FF00_C = 0xF2,
    DI          = 0xF3,
    ILLEGAL_F4  = 0xF4,
    PUSH_AF     = 0xF5,
    OR_A_U8     = 0xF6,
    RST_30      = 0xF7, // Reset (jump to $0030)
    LD_HL_SPi8  = 0xF8,
    LD_SP_HL    = 0xF9,
    LD_A_U16    = 0xFA,
    EI          = 0xFB,
    ILLEGAL_FC  = 0xFC,
    ILLEGAL_FD  = 0xFD,
    CP_A_U8     = 0xFE,
    RST_38      = 0xFF // Reset (jump to $0038)
}unprefixed_opcode;

typedef enum {
    RLC_B   = 0x00,
    RLC_C   = 0x01,
    RLC_D   = 0x02,
    RLC_E   = 0x03,
    RLC_H   = 0x04,
    RLC_L   = 0x05,
    RLC_HL  = 0x06,
    RLC_A   = 0x07,
           
    RRC_B   = 0x08,
    RRC_C   = 0x09,
    RRC_D   = 0x0A,
    RRC_E   = 0x0B,
    RRC_H   = 0x0C,
    RRC_L   = 0x0D,
    RRC_HL  = 0x0E,
    RRC_A   = 0x0F,
           
    RL_B    = 0x10,
    RL_C    = 0x11,
    RL_D    = 0x12,
    RL_E    = 0x13,
    RL_H    = 0x14,
    RL_L    = 0x15,
    RL_HL   = 0x16,
    RL_A    = 0x17,
           
    RR_B    = 0x18,
    RR_C    = 0x19,
    RR_D    = 0x1A,
    RR_E    = 0x1B,
    RR_H    = 0x1C,
    RR_L    = 0x1D,
    RR_HL   = 0x1E,
    RR_A    = 0x1F,
           
    SLA_B   = 0x20,
    SLA_C   = 0x21,
    SLA_D   = 0x22,
    SLA_E   = 0x23,
    SLA_H   = 0x24,
    SLA_L   = 0x25,
    SLA_HL  = 0x26,
    SLA_A   = 0x27,
           
           
    SRA_B   = 0x28,
    SRA_C   = 0x29,
    SRA_D   = 0x2A,
    SRA_E   = 0x2B,
    SRA_H   = 0x2C,
    SRA_L   = 0x2D,
    SRA_HL  = 0x2E,
    SRA_A   = 0x2F,
           
    // Swap the first and second 4 bits in an 8-bit value/register
    SWAP_B  = 0x30,
    SWAP_C  = 0x31,
    SWAP_D  = 0x32,
    SWAP_E  = 0x33,
    SWAP_H  = 0x34,
    SWAP_L  = 0x35,
    SWAP_HL = 0x36,
    SWAP_A  = 0x37,

    SRL_B   = 0x38,
    SRL_C   = 0x39,
    SRL_D   = 0x3A,
    SRL_E   = 0x3B,
    SRL_H   = 0x3C,
    SRL_L   = 0x3D,
    SRL_HL  = 0x3E,
    SRL_A   = 0x3F
}prefixed_opcode;

// The number of CPU cycles that each instruction takes.
static const uint8_t opcode_cycles[] = {
    1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,
    1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
    2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1,
    2, 3, 2, 2, 3, 3, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    2, 3, 3, 4, 3, 4, 2, 3, 2, 4, 3, 1, 3, 6, 2, 4,
    2, 3, 3, 1, 3, 4, 2, 4, 2, 4, 3, 1, 3, 1, 2, 4,
    3, 3, 2, 1, 1, 4, 2, 4, 4, 1, 4, 1, 1, 1, 2, 4,
    3, 3, 2, 1, 1, 4, 2, 4, 3, 2, 4, 1, 1, 1, 2, 4
};

typedef struct {
    union {
        register16 AF;
        struct {
            struct {
                uint8_t zero: 1;
                uint8_t subtraction: 1;
                uint8_t half_carry: 1;
                uint8_t carry: 1;
                uint8_t unused: 4;
            }F;
            register8 A;
        };
    };
    union {
        register16 BC;
        struct {
            register8 C;
            register8 B;
        };
    };
    union {
        register16 DE;
        struct {
            register8 E;
            register8 D;
        };
    };
    union {
        register16 HL;
        struct {
            register8 L;
            register8 H;
        };
    };
    register16 SP;
    register16 PC;
    register8 IME;
    bool executing;

    // Number of machine cycles left until the current operation executes
    uint8_t remaining_execution_cycles;
}cpu_state;

bool tick(const machine_state* machine);

