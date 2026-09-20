#include "command_parser.h"

#include <string.h>

enum command_id command_parse(const char *text)
{
    if (strcmp(text, "help") == 0) {
        return COMMAND_HELP;
    }

    if (strcmp(text, "status") == 0) {
        return COMMAND_STATUS;
    }

    return COMMAND_UNKNOWN;
}
