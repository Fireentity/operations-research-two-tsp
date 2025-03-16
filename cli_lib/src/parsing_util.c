#include <c_util.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <flag.h>
#include <parsing_util.h>
#include "hashmap.h"

ParsingResult parse_unsigned_int(const char *arg, unsigned int *parsed) {
    if (!arg) return PARSE_MISSING_VALUE;
    char *end;
    errno = 0;
    const unsigned int val = strtoul(arg, &end, 10);
    if (errno || *end != '\0' || val > UINT_MAX) // Check conversion errors and overflow
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = (unsigned int) val;
    return PARSE_SUCCESS;
}

ParsingResult parse_int(const char *arg, int *parsed) {
    if (!arg) return PARSE_MISSING_VALUE;
    char *end;
    errno = 0;
    const long val = strtol(arg, &end, 10);
    if (errno || *end != '\0' || val < INT_MIN || val > INT_MAX) // Verify range and conversion
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = (int) val;
    return PARSE_SUCCESS;
}

ParsingResult parse_float(const char *arg, float *parsed) {
    if (!arg) return PARSE_MISSING_VALUE;
    char *end;
    errno = 0;
    const float val = strtof(arg, &end);
    if (errno || *end != '\0') // Ensure complete conversion
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = val;
    return PARSE_SUCCESS;
}

ParsingResult parse_bool(const char *arg, bool *parsed) {
    if (!arg) return PARSE_MISSING_VALUE;
    if (!strcasecmp(arg, "true") || strcmp(arg, "1") == 0) {
        *parsed = true;
        return PARSE_SUCCESS;
    }
    if (!strcasecmp(arg, "false") || strcmp(arg, "0") == 0) {
        *parsed = false;
        return PARSE_SUCCESS;
    }
    return PARSE_WRONG_VALUE_TYPE;
}

typedef struct {
    const char *key;
    const Flag *flag;
    bool parsed;
} FlagEntry;

uint64_t flag_entry_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const FlagEntry *e = item;
    return hashmap_xxhash3(e->key, strlen(e->key), seed0, seed1);
}

int flag_entry_compare(const void *a, const void *b, void *udata) {
    const FlagEntry *ea = a;
    const FlagEntry *eb = b;
    return strcmp(ea->key, eb->key);
}

struct hashmap *init_hashmap(const Flag **flags, int number_of_flags) {
    struct hashmap *map = hashmap_new(sizeof(FlagEntry), 0, rand(), rand(),
                                      flag_entry_hash, flag_entry_compare, NULL, NULL);
    for (int i = 0; i < number_of_flags; i++) {
        FlagEntry entry = {
                .key = flags[i]->get_label(flags[i]),
                .flag = flags[i],
                .parsed = false
        };

        if (hashmap_set(map, &entry)) {
            // TODO: Handle duplicate flag labels if necessary
        }
    }
    return map;
}

DEFINE_COUNT_IF(Flag*, number_of_flags)
static bool is_mandatory(const Flag *arr[], const Flag *flag) {
    return flag->is_mandatory(flag);
}

ParsingResult parse_flags(CmdOptions *cmd_options,
                          const Flag **tsp_flags,
                          int number_of_flags,
                          int argc,
                          const char **argv) {

    const int mandatory_flags = count_number_of_flags_if(tsp_flags, number_of_flags, is_mandatory); // Count required flags
    int parsed_mandatory_flags = 0;
    struct hashmap *map = init_hashmap(tsp_flags, number_of_flags);

    for (unsigned int current_param = 1; current_param < argc - 1; current_param++) {
        FlagEntry search_entry = {argv[current_param], NULL, false};
        FlagEntry *found_entry = (FlagEntry *) hashmap_get(map, &search_entry);
        if (!found_entry) return PARSE_UNKNOWN_ARG; // Unknown flag encountered
        ParsingResult result = found_entry->flag->parse(found_entry->flag, cmd_options, argv, &current_param);
        // TODO: Additional algorithm steps if needed
        if (PARSE_SUCCESS != result) {
            return result;
        }
        if (found_entry->flag->is_mandatory(found_entry->flag))
            parsed_mandatory_flags++;
        found_entry->parsed = true;
    }

    return parsed_mandatory_flags != mandatory_flags ? PARSE_MISSING_MANDATORY_FLAG : PARSE_SUCCESS;
}
