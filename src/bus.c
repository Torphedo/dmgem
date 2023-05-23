#include "bus.h"
#include "memory_controllers.h"

bool bus_address_in_rom(uint16_t address) {
    return (address <= 0x7FFF);
}

bool bus_address_in_external_ram(uint16_t address) {
    return (address >= 0xA000 && address <= 0xBFFF);
}

uint8_t* bus_read(uint16_t address, const machine_state* machine) {
    // These are handled by the memory controller
    if ((bus_address_in_rom(address) || bus_address_in_external_ram(address))) {
        // Extra if statement keeps code horizontally short
        if (machine->memory_controller != NONE) {
            return controller_read(address, machine);
        }
    }
    else {
        // Normal memory read, return address of the target byte
        return &machine->console_memory[address];
    }
}

void bus_write_8_bit(uint16_t address, uint8_t value, const machine_state* machine) {
    // These are handled by the memory controller
    if ((bus_address_in_rom(address) || bus_address_in_external_ram(address))) {
        // Extra if statement keeps code horizontally short
        if (machine->memory_controller != NONE) {
            controller_write_8_bit(address, value, machine);
        }
    }
    else {
        if (!bus_address_in_rom(address)) {
            // Normal memory read, return address of the target byte
            machine->console_memory[address] = value;
        }
    }
}
void bus_write_16_bit(uint16_t address, uint16_t value, const machine_state* machine) {
    uint16_t high = (value & 0xFF00) >> 8;
    uint16_t low = value & 0x00FF;
    bus_write_8_bit(address, low, machine);
    bus_write_8_bit(address + 1, high, machine);
}

