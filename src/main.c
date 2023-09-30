#include <stdint.h>
#include <malloc.h>
#include <sys/stat.h>

#include "logging.h"
#include "rom.h"
#include "machine.h"
#include "arguments.h"

int main(int argc, char* argv[]) {
    // Parse command-line arguments.
    flags options = parse_arguments(argc, argv);

    if (options.filename == NULL) {
        LOG_MSG(error, "No filenames detected.\n");
        return 1;
    }

    char* filename = options.filename;

    // Make sure file exists
    struct stat st = {0};
    if (stat(filename, &st) != 0) {
        LOG_MSG(error, "%s doesn't exist.\n", filename);
        return false;
    }

    uint32_t rom_size = st.st_size;
    uint8_t* rom_data = calloc(rom_size, 1);
    if (rom_data == NULL) {
        LOG_MSG(error, "Failed to allocate %d bytes for ROM\n", rom_size);
        return false;
    }

    FILE* rom_file = fopen(filename, "rb");
    if (rom_file == NULL) {
        LOG_MSG(error, "Failed to open provided ROM file %s\n", filename);
        return false;
    }

    if (fread(rom_data, rom_size, 1, rom_file) != 1) {
        LOG_MSG(error, "Failed to read all %d bytes from %s.\n", rom_size, filename);
        fclose(rom_file);
        return false;
    }
    fclose(rom_file);
    LOG_MSG(debug, "Loaded %s (%d bytes)\n", filename, rom_size);

    // Main emulation loop
    uint8_t exit_code = run_machine(rom_data, rom_size);
    free(rom_data);
    return exit_code;
}

