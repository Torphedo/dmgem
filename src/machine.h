#pragma once
#include <stdint.h>
#include <stdbool.h>

// Memory controller types
typedef enum {
    NONE,
    MBC1,
    MBC2,
    MBC3,
    MBC5,
    MBC6,
    MBC7
}controller_type;

typedef enum {
   RAM_BANK_SIZE = 0x2000
}machine_constants;

typedef struct {
    uint8_t* console_memory; // Machine's 16-bit address space
    uint8_t* cartridge_rom; // ROM file (full cartridge data)
    uint8_t* external_ram; // External cartridge RAM
    uint8_t rom_bank_count;
    uint8_t ram_bank_count;
    controller_type memory_controller;
    uint64_t clock;
}machine_state;

bool run_machine(uint8_t* rom_data, uint32_t rom_size);

