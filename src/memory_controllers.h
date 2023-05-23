#pragma once
#include <stdint.h>
#include <stdbool.h>

#include "rom.h"
#include "machine.h"

uint8_t* controller_read(uint16_t address, const machine_state* machine);
void controller_write_8_bit(uint16_t address, uint8_t value, const machine_state* machine);
controller_type get_controller_type(hardware_flags flags);
bool init_memory_controller(cart_header* cart);

