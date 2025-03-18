#include <stdlib.h>
#include <string.h>
#include "flag_parser.h"
#include "hashmap.h"
#include "c_util.h"

struct FlagParser {
    // hashmap of ParserNode
    struct hashmap *map;
    const int mandatory_flags_count;
};

struct ParserNode {
    const char *key;
    const Flag *flag;
    // hashmap of ParserNode
    struct hashmap *children;
    const int mandatory_flags_count;
};

uint64_t hash(const void *item, const uint64_t seed0, const uint64_t seed1) {
    const ParserNode *entry = item;
    return hashmap_xxhash3(entry->key, strlen(entry->key), seed0, seed1);
}

int compare(const void *a, const void *b, void *udata) {
    const ParserNode *ea = a;
    const ParserNode *eb = b;
    return strcmp(ea->key, eb->key);
}

void free_parser_node(void *item) {
    if (!item) return;
    const ParserNode *entry = item;
    if (entry->children) {
        hashmap_free(entry->children);
    }
}

static ParsingResult recursive_parse(const ParserNode *node, CmdOptions *cmd_options, const int argc, const char **argv,
                                     unsigned int *current_index, const bool is_root) {
    int parsed_mandatory = 0;
    // last argv is null so -1
    while (*current_index < argc) {
        if (!argv[*current_index]) break; //TODO controlla perche serve
        ParserNode search_entry = {.key = argv[*current_index]};
        ParserNode *child = (ParserNode *) hashmap_get(node->children, &search_entry);
        if (!child) {
            // If it's root then the flag is not recognized at all so return PARSE_UNKNOWN_ARG
            if (is_root)
                return PARSE_UNKNOWN_ARG;
            // If it's a children and has already reached the mandatory count then parent needs to continue the check so we break
            if (parsed_mandatory == node->mandatory_flags_count)
                break;
            // If it's a children and has not reached the mandatory count we return PARSE_MISSING_MANDATORY_FLAG
            return PARSE_MISSING_MANDATORY_FLAG;
        }
        // Make the flag parse
        ParsingResult result = child->flag->parse(child->flag, cmd_options, argv, current_index);
        // Flag has been parsed so advance in argv
        (*current_index)++;

        if (PARSE_SUCCESS != result)
            return result;
        if (child->flag->is_mandatory(child->flag))
            parsed_mandatory++;

        // If the node found has children we first make them parse the incoming labels
        if (child->children) {
            result = recursive_parse(child, cmd_options, argc, argv, current_index, false);
            if (PARSE_SUCCESS != result)
                return result;
        }
    }
    return (parsed_mandatory == node->mandatory_flags_count) ? PARSE_SUCCESS : PARSE_MISSING_MANDATORY_FLAG;
}


static struct hashmap *make_hashmap_nodes_from_flags(const struct FlagsArray flags) {
    if (!flags.number_of_flags || !flags.flags) return NULL;
    struct hashmap *map = hashmap_new(sizeof(ParserNode), flags.number_of_flags, rand(), rand(), hash, compare,
                                      free_parser_node, NULL);
    for (int i = 0; i < flags.number_of_flags; i++) {
        int child_mandatory_count = 0;
        if (flags.flags[i]->children.flags && flags.flags[i]->children.number_of_flags > 0) {
            for (int j = 0; j < flags.flags[i]->children.number_of_flags; j++) {
                if (flags.flags[i]->children.flags[j]->is_mandatory(flags.flags[i]->children.flags[j]))
                    child_mandatory_count++;
            }
        }
        ParserNode entry = {
            .key = flags.flags[i]->get_label(flags.flags[i]),
            .flag = flags.flags[i],
            .mandatory_flags_count = child_mandatory_count,
            .children = make_hashmap_nodes_from_flags(flags.flags[i]->children)
        };

        if (hashmap_set(map, &entry)) {
            // duplicates
        }
    }
    return map;
}


FlagParser *init_flag_parser(const struct FlagsArray flags) {
    int top_mandatory_count = 0;
    for (int i = 0; i < flags.number_of_flags; i++) {
        if (flags.flags[i]->is_mandatory(flags.flags[i]))
            top_mandatory_count++;
    }
    const FlagParser parser = {
        .map = make_hashmap_nodes_from_flags(flags),
        .mandatory_flags_count = top_mandatory_count
    };
    return malloc_from_stack(&parser, sizeof(parser));
}


ParsingResult parse_flags_with_parser(CmdOptions *cmd_options, const FlagParser *parser, const int argc,
                                      const char **argv) {
    const ParserNode root = {.children = parser->map, .mandatory_flags_count = parser->mandatory_flags_count};
    unsigned int current_index = 1;
    return recursive_parse(&root, cmd_options, argc, argv, &current_index, true);
}
