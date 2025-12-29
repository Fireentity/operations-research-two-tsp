#ifndef CMD_OPTION_INTERNAL_H
#define CMD_OPTION_INTERNAL_H

#include "cmd_options.h"
#include <stddef.h>

#include "flag_parser.h"

/* Enum describing all supported option types.
   Special cases (e.g., TSP mode) require custom parsing. */
typedef enum {
    OPT_INT,
    OPT_UINT,
    OPT_DOUBLE,
    OPT_UDOUBLE,
    OPT_BOOL,
    OPT_STRING,
    OPT_TSP_MODE
} OptionType;

/* Central metadata entry describing one option.
   This ensures a single authoritative mapping between CLI,
   INI keys, and fields inside CmdOptions. */
typedef struct {
    const char *cli_long;
    const char *cli_short;
    const char *help;

    const char *ini_section;
    const char *ini_name;

    OptionType type;
    size_t offset;
} OptionMeta;

/* Returns pointer to the complete metadata registry. */
const OptionMeta *cmd_options_get_metadata(void);

/* Returns number of metadata entries. */
size_t cmd_options_get_metadata_count(void);

FlagParser *cmd_options_build_cli_parser(CmdOptions *opt);

void cmd_options_parse_ini_file(CmdOptions *dest, const char *filename);

#endif // CMD_OPTION_INTERNAL_H
