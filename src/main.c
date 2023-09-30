#include <stdint.h>
#include <malloc.h>
#include <sys/stat.h>

#include "logging.h"
#include "file.h"

#include "rom.h"
#include "machine.h"
#include "arguments.h"

void print_instructions() {
    LOG_MSG(info, "Usage: dmgem [ROM filepath]\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        LOG_MSG(error, "No ROM file provided.\n");
        print_instructions();
        return 1;
    }

    char* filename = argv[1];

    if (!file_exists(filename)) {
        LOG_MSG(error, "%s doesn't exist.\n", filename);
        print_instructions();
        return 1;
    }
    
    uint32_t rom_size = file_size(filename);
    uint8_t* rom_data = calloc(rom_size, 1);
    if (rom_data == NULL) {
        LOG_MSG(error, "Failed to allocate %d bytes for ROM\n", rom_size);
        return 1;
    }

    FILE* rom_file = fopen(filename, "rb");
    if (rom_file == NULL) {
        LOG_MSG(error, "Failed to open provided ROM file %s\n", filename);
        print_instructions();
        return 1;
    }

    if (fread(rom_data, rom_size, 1, rom_file) != 1) {
        LOG_MSG(error, "Failed to read all %d bytes from %s\n", rom_size, filename);
        fclose(rom_file);
        return 1;
    }
    fclose(rom_file);
    LOG_MSG(debug, "Loaded %s (%d bytes)\n", filename, rom_size);

    // Main emulation loop
    uint8_t exit_code = run_machine(rom_data, rom_size);
    free(rom_data);
    return exit_code;
}

