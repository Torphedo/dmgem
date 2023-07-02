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

bool run_machine(uint8_t* rom_data, uint32_t rom_size) {
    machine_state machine = {0};
    uint32_t machine_mem_size = 0xFFFF + 1;
    uint32_t cart_ram_size = RAM_BANK_SIZE * 8; // Enough space for 8 8KiB banks of external RAM
     
    machine.console_memory = calloc(machine_mem_size + rom_size + cart_ram_size, 1);
    if (machine.console_memory == NULL) {
        return false;
    }
    machine.cartridge_rom = (machine.console_memory + machine_mem_size);

    memcpy(machine.cartridge_rom, rom_data, rom_size);

    // Copy the first 2 16KiB ROM banks into RAM
    memcpy(machine.console_memory, machine.cartridge_rom, 0x7FFF);
    cart_header* cart = (cart_header*) (machine.console_memory + 0x100);
    machine.memory_controller = get_controller_type(get_cart_hardware(cart));
    machine.rom_bank_count = 2 * (1 << cart->rom_size);
    machine.ram_bank_count = ram_bank_count(cart);
    print_rom_info(cart);

    machine.external_ram = calloc(1, RAM_BANK_SIZE * machine.ram_bank_count);

    // Emulator will only enter main loop if the requested memory controller is implemented
    bool running = init_memory_controller(cart);

    while (running) {
        machine.clock++;
        running = tick(&machine);
    }
    free(machine.console_memory);
    // Inverted because exit code 0 is success but set to true in stdbool.
    // This will return 0 if the loop is broken and 1 if a component returns false.
    return !running;

}

