#include <stdbool.h>
#include <math.h>

#include "memory_controllers.h"
#include "bus.h"

enum {
    ENABLE_RAM = 0x1FFF,
    ROM_BANK = 0x3FFF,
    RAM_BANK = 0x5FFF,
    MODE_FLAG = 0x7FFF
}MBC1_fields;

static struct {
    uint8_t rom_bank;
    uint8_t ram_bank;
    bool mode;
    bool ram_enabled;
}controller_state;

// Size of a 16KiB ROM bank.
#define ROM_BANK_SIZE 0x4000
// Size of an 8KiB RAM bank.
#define RAM_BANK_SIZE 0x2000

// A pointer to this is returned when reading from disabled cartridge RAM, so that all the read data will be 0xFF.
static const uint64_t invalid_data = 0xFFFFFFFFFFFFFFFF;

static controller_type memory_controller = NONE;
static uint8_t rom_size = 0;

controller_type get_controller_type(hardware_flags flags) {
    if (flags.MBC1) {
        return MBC1;
    }
    else if (flags.MBC2) {
        return MBC2;
    }
    else if (flags.MBC3) {
        return MBC3;
    }
    else if (flags.MBC5) {
        return MBC5;
    }
    else if (flags.MBC6) {
        return MBC6;
    }
    else if (flags.MBC7) {
        return MBC7;
    }
    else {
        return NONE;
    }
}

uint8_t zero_bank_number(uint8_t rom_bank_count, uint8_t ram_bank_count) {
    if (rom_bank_count <= 32) {
        return 0;
    }
    if (rom_bank_count <= 64) {
        return ram_bank_count << 4;
    }
    if (rom_bank_count <= 128) {
        return ram_bank_count << 5;
    }

    return 0;
}

uint8_t high_bank_number(uint8_t rom_bank_count, uint8_t ram_bank_count) {
    // ROM bank number capped to the number of available banks
    uint8_t base_bank_num = controller_state.rom_bank;
    if (base_bank_num > rom_bank_count) {
        base_bank_num = rom_bank_count;
    }

    if (rom_bank_count <= 32) {
        return base_bank_num;
    }
    if (rom_bank_count <= 64) {
        // AND value with the lowest bit of the ram bank count
        return base_bank_num & ((ram_bank_count & 0b00000001) << 5);
    }
    if (rom_bank_count <= 128) {
        // AND value with the lowest 2 bits of the ram bank count
        return base_bank_num & ((ram_bank_count & 0b00000011) << 5);
    }

    return base_bank_num;
}

bool init_memory_controller(cart_header* cart) {
    hardware_flags hardware = get_cart_hardware(cart);
    memory_controller = get_controller_type(hardware);

    return true;
}

uint8_t* controller_read(uint16_t address, const machine_state* machine) {
    switch (machine->memory_controller) {
        case MBC1:
            if (address <= 0x3FFF) {
                if (controller_state.mode == 1) {
                    uint8_t zero_bank_num = zero_bank_number(machine->rom_bank_count, machine->ram_bank_count);
                    return &machine->cartridge_rom[(ROM_BANK_SIZE * zero_bank_num) + address];
                }
                else {
                    break;
                }
            }
            else if (address <= 0x7FFF) {
                // Offset within the bank that the address is pointing to
                uint16_t relative_address = address - ROM_BANK_SIZE;
                uint8_t high_bank_num = high_bank_number(machine->rom_bank_count, machine->ram_bank_count) + 1;
                return &machine->cartridge_rom[(ROM_BANK_SIZE * high_bank_num) + relative_address];
            }
            else if (bus_address_in_external_ram(address)) {
                if (!controller_state.ram_enabled) {
                    return (uint8_t*) &invalid_data;
                }

                uint16_t relative_address = address - 0xA000;
                if (controller_state.mode == 1) {
                    return &machine->external_ram[(RAM_BANK_SIZE * controller_state.ram_bank) + relative_address];
                }
                else {
                    return &machine->external_ram[relative_address];
                }
            }
        default:
            break;
    }
    return &machine->console_memory[address];
}

void controller_write_8_bit(uint16_t address, uint8_t value, const machine_state* machine) {
    switch (machine->memory_controller) {
        case MBC1:
            if (address <= ENABLE_RAM) {
                controller_state.ram_enabled = (value == 0xA);
            }
            else if (address <= ROM_BANK) {
                // Limit the bitmask so that it's impossible to write a number to controller_state.rom_bank
                // larger than the number of available banks.
                uint8_t bitmask = (machine->rom_bank_count - 1) & 0b00011111; // Exclude 3 bits because this is a 5-bit field

                controller_state.rom_bank = value & bitmask;
                if (controller_state.rom_bank == 0) {
                    controller_state.rom_bank = 1;
                }
            }
            else if (address <= RAM_BANK) {
                controller_state.ram_bank = value & 0b00000011; // Get lowest 2 bits of value
            }
            else if (address <= MODE_FLAG) {
                controller_state.mode = value & 0b00000001;
            }

            if (bus_address_in_external_ram(address)) {
                if (!controller_state.ram_enabled) {
                    return;
                }
                else {
                    if (machine->ram_bank_count <= 1) {
                        // The mod operator here stops an out of bounds write
                        uint16_t relative_address = (address - 0xA000) % RAM_BANK_SIZE;
                        machine->external_ram[relative_address] = value;
                    }
                    if (controller_state.mode == 0) {
                        uint16_t relative_address = address - 0xA000;
                        machine->external_ram[relative_address] = value;
                    }
                    else {
                        uint32_t relative_address = (RAM_BANK_SIZE * controller_state.ram_bank) + address - 0xA000;
                        machine->external_ram[relative_address] = value;
                    }
                }
            }
        default:
            break;
    }
}

