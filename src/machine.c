// Manages entire virtual machine by updating each component every machine
// cycle. Each component will wait for the appropriate number of cycles if
// doing a multi-cycle operation, for more accurate timing.

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <memory.h>
#include <stdio.h>

#include <logging.h>

#include "machine.h"
#include "cpu.h"
#include "memory_controllers.h"
#include "rom.h"

bool run_machine(char* filename, uint32_t rom_size) {
    FILE* rom_file = fopen(filename, "rb");
    if (rom_file == NULL) {
        log_error(CRITICAL, "Failed to open provided ROM file %s\n", filename);
        return false;
    }
    static machine_state machine = {0};
    uint32_t machine_mem_size = 0xFFFF + 1;
    uint32_t cart_ram_size = 0x2000 * 8; // Enough space for 8 8KiB banks of external RAM
    uint8_t* machine_data = calloc(machine_mem_size + rom_size + cart_ram_size, 1);
    if (machine_data == NULL) {
        return false;
    }
    machine.console_memory = machine_data;
    machine.cartridge_rom = (machine_data + machine_mem_size);

    if (fread(machine.cartridge_rom, rom_size, 1, rom_file) != 1) {
        log_error(CRITICAL, "Failed to read all %d bytes from %s.\n", rom_size, filename);
        fclose(rom_file);
        return false;
    }
    fclose(rom_file);

    // Copy the first 2 16KiB ROM banks into RAM
    memcpy(machine.console_memory, machine.cartridge_rom, 0x7FFF);
    cart_header* cart = (cart_header*) (machine.console_memory + 0x100);
    machine.memory_controller = get_controller_type(get_cart_hardware(cart));
    machine.rom_bank_count = 2 * (1 << cart->rom_size);
    machine.ram_bank_count = ram_bank_count(cart);
    print_rom_info(cart);

    // Emulator will only enter main loop if the requested memory controller is implemented
    bool running = init_memory_controller(cart);

    while (running) {
        machine.clock++;
        running = tick(&machine);
    }
    free(machine_data);
    // Inverted because exit code 0 is success but set to true in stdbool.
    // This will return 0 if the loop is broken and 1 if a component returns false.
    return !running;

}

