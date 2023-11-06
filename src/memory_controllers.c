#include <stdbool.h>

#include "machine.h"
#include "memory_controllers.h"
#include "bus.h"

static struct {
    uint8_t rom_bank: 5;
    uint8_t ram_bank: 2;
    bool mode: 1;
    bool ram_enabled: 1;
}controller_state;

enum {
    RANGE_MIN = 0x000
};

typedef enum {
    MBC1_W_RANGE_ENABLE_RAM = 0x1FFF,
    MBC1_W_RANGE_ROM_BANK = 0x3FFF,
    MBC1_W_RANGE_RAM_BANK = 0x5FFF,
    MBC1_W_RANGE_MODE_SEL = 0x7FFF,
    MBC1_W_RANGE_EXT_RAM_LOW = 0xA000,
    MBC1_W_RANGE_EXT_RAM_HIGH = 0xBFFF,
}mbc1_w_range;

typedef enum {
    MBC1_R_RANGE_ROM_0 = 0x3FFF,
    MBC1_R_RANGE_ROM_HIGH = 0x7FFF,
}mbc1_r_range;

typedef enum {
    MBC3_W_RANGE_MIN = 0x0000,
    MBC3_W_RANGE_
}mbc3_w_range;

bool in_range(uint16_t value, uint16_t low, uint16_t high) {
    return (value >= low && value <= high);
}

bool range_mbc1_enable_ram(uint16_t addr) {
    return in_range(addr, RANGE_MIN, MBC1_W_RANGE_ENABLE_RAM);
}

bool range_mbc1_rom_bank(uint16_t addr) {
    return in_range(addr, MBC1_W_RANGE_ENABLE_RAM, MBC1_W_RANGE_ROM_BANK);
}

bool range_mbc1_ram_bank(uint16_t addr) {
    return in_range(addr, MBC1_W_RANGE_ROM_BANK, MBC1_W_RANGE_RAM_BANK);
}

bool range_mbc1_mode_sel(uint16_t addr) {
    return in_range(addr, MBC1_W_RANGE_RAM_BANK, MBC1_W_RANGE_MODE_SEL);
}

bool range_mbc1_ext_ram(uint16_t addr) {
    return in_range(addr, MBC1_W_RANGE_EXT_RAM_LOW, MBC1_W_RANGE_EXT_RAM_HIGH);
}

bool range_mbc1_rom_0(uint16_t addr) {
    return in_range(addr, RANGE_MIN, MBC1_R_RANGE_ROM_0);
}

bool range_mbc1_rom_high(uint16_t addr) {
    return in_range(addr, MBC1_R_RANGE_ROM_0, MBC1_R_RANGE_ROM_HIGH);
}

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
    else if (rom_bank_count <= 64) {
        return ram_bank_count << 4;
    }
    else if (rom_bank_count <= 128) {
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
    else if (rom_bank_count <= 64) {
        // AND value with the lowest bit of the ram bank count
        return base_bank_num & ((ram_bank_count & 0b00000001) << 5);
    }
    else if (rom_bank_count <= 128) {
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

uint8_t* mbc1_read(uint16_t addr, const machine_state* machine) {
    // Read from ROM bank 0
    if (range_mbc1_rom_0(addr)) {
        if (controller_state.mode != 0) {
            uint8_t zero_bank_num = zero_bank_number(machine->rom_bank_count, machine->ram_bank_count);
            addr += (ROM_BANK_SIZE * zero_bank_num);
        }
        return &machine->cartridge_rom[addr];
    }
    else if (range_mbc1_rom_high(addr)) {
        // Offset within the bank that the addr is pointing to
        uint8_t high_bank_num = high_bank_number(machine->rom_bank_count, machine->ram_bank_count) + 1;
        uint16_t offset = ROM_BANK_SIZE * high_bank_num + (addr - ROM_BANK_SIZE);
        return &machine->cartridge_rom[offset];
    }
    else if (range_mbc1_ext_ram(addr)) {
        // Trying to read from disabled RAM always returns 0xFF values.
        if (!controller_state.ram_enabled) {
            return (uint8_t*) &invalid_data;
        }
    
        uint16_t offset = addr - MBC1_W_RANGE_EXT_RAM_LOW;
        if (controller_state.mode == 1) {
            offset += RAM_BANK_SIZE * controller_state.ram_bank;
        }
        return &machine->external_ram[offset];
    }
    return (uint8_t*) &invalid_data;
}

uint8_t* controller_read(uint16_t addr, const machine_state* machine) {
    switch (machine->memory_controller) {
    case MBC1:
        return mbc1_read(addr, machine);
        break;
    default:
        break;
    }
    return &machine->console_memory[addr];
}

void write_mbc1_8(uint16_t addr, uint8_t value, const machine_state* machine) {
    if (range_mbc1_enable_ram(addr)) {
        controller_state.ram_enabled = ((value | 0xF) == 0xA);
    }
    else if (range_mbc1_rom_bank(addr)) {
        // Exclude 3 bits because this is a 5-bit field.
        // Stops us from writing a bank number larger than the number of
        // available banks.
        uint8_t bitmask = (machine->rom_bank_count - 1) & 0b00011111;
    
        controller_state.rom_bank = value & bitmask;
        if (controller_state.rom_bank == 0) {
            controller_state.rom_bank = 1;
        }
    }
    else if (range_mbc1_ram_bank(addr)) {
        // Get lowest 2 bits of value
        controller_state.ram_bank = value & 0b00000011;
    }
    else if (range_mbc1_mode_sel(addr)) {
        controller_state.mode = value & 0b00000001;
    }
    else if (range_mbc1_ext_ram(addr)) {
        // Early exit if RAM is locked.
        if (!controller_state.ram_enabled) {
            return;
        }

        // Offset from RAM start
        uint32_t relative_address = addr - MBC1_W_RANGE_EXT_RAM_LOW;

        if (machine->ram_bank_count <= 1) {
            // The mod operator here stops an out of bounds write
            relative_address %= RAM_BANK_SIZE;
        }
        // If mode flag is 0, we just use the offset.
        if (controller_state.mode == 1) {
            relative_address += (RAM_BANK_SIZE * controller_state.ram_bank);
        }
        machine->external_ram[relative_address] = value;
    }
}

void controller_write_8_bit(uint16_t addr, uint8_t value, const machine_state* machine) {
    switch (machine->memory_controller) {
        case MBC1:
            write_mbc1_8(addr, value, machine);
            break;
        default:
            break;
    }
}

