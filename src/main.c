#include <stdint.h>
#include <malloc.h>
#include <sys/stat.h>

#include <logging.h>

#include "rom.h"
#include "machine.h"
#include "arguments.h"

int main(int argc, char* argv[]) {
    // Parse command-line arguments.
    flags options = parse_arguments(argc, argv);

    if (options.filename == NULL) {
        log_error(CRITICAL, "main(): No filenames detected.\n");
        return 1;
    }
    if (options.silent) {
        disable_logging();
    }

    char* filename = options.filename;

    // Make sure file exists
    struct stat st = {0};
    if (stat(filename, &st) != 0) {
        log_error(CRITICAL, "main(): %s doesn't exist.\n", filename);
        return false;
    }

    uint32_t rom_size = st.st_size;
    uint8_t* rom_data = calloc(rom_size, 1);
    if (rom_data == NULL) {
        log_error(CRITICAL, "main(): Failed to allocate %d bytes for ROM\n", rom_size);
        return false;
    }

    FILE* rom_file = fopen(filename, "rb");
    if (rom_file == NULL) {
        log_error(CRITICAL, "Failed to open provided ROM file %s\n", filename);
        return false;
    }

    if (fread(rom_data, rom_size, 1, rom_file) != 1) {
        log_error(CRITICAL, "main(): Failed to read all %d bytes from %s.\n", rom_size, filename);
        fclose(rom_file);
        return false;
    }
    fclose(rom_file);
    log_error(DEBUG, "main(): Loaded %s (%d bytes)\n", filename, rom_size);

    // Main emulation loop
    uint8_t exit_code = run_machine(rom_data, rom_size);
    free(rom_data);
    return exit_code;
}

