#include <sys/stat.h>

#include "logging.h"
#include "file.h"

bool file_exists(const char* filepath) {
    struct stat st = {0};
    return (stat(filepath, &st) == 0);
}

uint32_t file_size(const char* filepath) {
    struct stat st = {0};
    stat(filepath, &st);
    return st.st_size;
}

