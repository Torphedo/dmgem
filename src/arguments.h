#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    bool silent: 1;
    uint8_t : 0; // Pads out struct to next boundary
    char* filename;
}flags;



// Pass in arguments from main(), and it will return a bitfield based on a bunch of manually specified flags
flags parse_arguments(int argc, char* argv[]);
