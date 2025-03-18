#ifndef FLAG_PARSER_H
#define FLAG_PARSER_H
#include "flag.h"
typedef struct FlagParser FlagParser;
typedef struct ParserNode ParserNode;

FlagParser *init_flag_parser(struct FlagsArray flags);

ParsingResult parse_flags_with_parser(CmdOptions* cmd_options, const FlagParser *parser, const int argc, const char **argv);

#endif //FLAG_PARSER_H
