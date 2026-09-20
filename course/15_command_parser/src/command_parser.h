#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

enum command_id {
    COMMAND_HELP,
    COMMAND_STATUS,
    COMMAND_UNKNOWN,
};

enum command_id command_parse(const char *text);

#endif /* COMMAND_PARSER_H */
