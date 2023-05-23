#include <malloc.h>
#include <sys/stat.h>

#include <logging.h>

#include "rom.h"
#include "machine.h"
#include "arguments.h"

int main(int argc, char* argv[])
{
    // Parse command-line arguments.
    flags options = parse_arguments(argc, argv);

    if (options.filename == NULL)
    {
        log_error(CRITICAL, "main(): No filenames detected.\n");
        return 1;
    }
    if (options.silent)
    {
        disable_logging();
    }

    struct stat st;
    if (stat(options.filename, &st) != 0) {
        log_error(CRITICAL, "chunk_parse_all(): Failed to get file metadata for %s\n", options.filename);
        return false;
    }
    unsigned int filesize = st.st_size;
    log_error(DEBUG, "main(): Loaded ROM %s with a size of %d bytes.\n", options.filename, filesize);

    uint8_t exit_code = run_machine(options.filename, filesize); // Main emulation loop
    return exit_code;
}
