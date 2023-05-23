#include <string.h>

#include <logging.h>

#include "arguments.h"

static const char* arguments[] = {
        "--silent"
};

flags parse_arguments(int argc, char* argv[])
{
    flags output = {0};
    uint16_t options_count = sizeof(arguments) / sizeof(char*);
    for (uint32_t i = 1; i < argc; i++)
    {
        // Check if first 2 characters are "--"
        if (argv[i][0] == 0x2D && argv[i][1] == 0x2D)
        {
            for (uint16_t j = 0; j < options_count; j++)
            {
                if (strlen(argv[i]) == strlen(arguments[j]))
                {
                    bool matches = (strcmp(argv[i], arguments[j]) == 0);
                    if (matches)
                    {
                        switch(j)
                        {
                            case 0:
                                output.silent = true;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
        else
        {
            output.filename = argv[i];
            continue;
        }
    }

    return output;
}
