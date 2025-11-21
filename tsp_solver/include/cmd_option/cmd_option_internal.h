#ifndef CMD_OPTIONS_INTERNAL_H
#define CMD_OPTIONS_INTERNAL_H

#include "cmd_options.h"
#include "flag_parser.h"

/* Internal functions used by cmd_options.c */

FlagParser *cmd_options_build_cli_parser(CmdOptions * opt);

void cmd_options_parse_ini_file(CmdOptions *dest, const char *filename);

#endif // CMD_OPTIONS_INTERNAL_H