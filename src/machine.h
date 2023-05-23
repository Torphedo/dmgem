#pragma once
#include <stdbool.h>

// Memory controller types
typedef enum
{
    NONE,
    MBC1,
    MBC2,
    MBC3,
    MBC5,
    MBC6,
    MBC7
}controller_type;

typedef struct
{
    uint8_t* console_memory; // Machine's 16-bit address space
    uint8_t* cartridge_rom; // Full ROM data loaded from the input file (full cartridge ROM data)
    uint8_t* external_ram; // External cartridge RAM
    uint8_t rom_bank_count;
    uint8_t ram_bank_count;
    controller_type memory_controller;
    uint64_t clock;
}machine_state;

bool run_machine(char* loaded_rom, uint32_t rom_size);
