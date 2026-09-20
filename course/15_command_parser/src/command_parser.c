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

    if (strcmp(text, "reset") == 0) {
        return COMMAND_RESET;
    }

    return COMMAND_UNKNOWN;
}
