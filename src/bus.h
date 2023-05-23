// Memory bus module that takes addresses from the CPU and forwards them to the appropriate module for read/write

#include <stdint.h>
#include <stdbool.h>

#include "machine.h"

// Returns true if the address is in the range of the external cartridge RAM bank
bool bus_address_in_external_ram(uint16_t address);
// Returns true if the address is in ROM
bool bus_address_in_rom(uint16_t address);

/// Allows data to be read from several multi-bank sources, such as cartridge ROM/RAM. If trying to read from
/// a section that uses banking, the cartridge memory controller will be invoked to get the real address of
/// the data in the appropriate bank.
/// \param address Address to read
/// \param machine Pointer to the emulator state
/// \return Pointer to the data the CPU is trying to read. Cast to a uint16_t* to read 2 bytes from the address.
uint8_t* bus_read(uint16_t address, const machine_state* machine);

void bus_write_8_bit(uint16_t address, uint8_t value, const machine_state* machine);
void bus_write_16_bit(uint16_t address, uint16_t value, const machine_state* machine);

