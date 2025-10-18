#include <stdlib.h>
#include <string.h>
#include "flag_parser.h"
#include "hashmap.h"
#include "c_util.h"

// Structure of the flag parser.
struct FlagParser {
    // Hashmap of ParserNode.
    struct hashmap *map;
    // Count of mandatory flags at the top level.
    const int mandatory_flags_count;
};

// Node in the parser tree.
struct ParserNode {
    const char *key;
    const Flag *flag;
    // Hashmap of child ParserNodes.
    struct hashmap *children;
    // Count of mandatory flags among the children.
    const int mandatory_flags_count;
};

// Hash function for ParserNode based on its key.
uint64_t hash(const void *item, const uint64_t seed0, const uint64_t seed1) {
    const ParserNode *entry = item;
    return hashmap_xxhash3(entry->key, strlen(entry->key), seed0, seed1);
}

// Compares two ParserNodes by their keys.
int compare(const void *a, const void *b, void *udata) {
    const ParserNode *ea = a;
    const ParserNode *eb = b;
    return strcmp(ea->key, eb->key);
}

// Frees a ParserNode and its children.
void free_parser_node(void *item) {
    if (!item) return;
    const ParserNode *entry = item;
    if (entry->children) {
        hashmap_free(entry->children);
    }
}

// Recursive parsing of flags.
// is_root indicates if the current node is the root.
static const ParsingResult *recursive_parse(const ParserNode *node, CmdOptions *cmd_options, const char **labels,
                                            unsigned int *current_index, const bool is_root, const bool skip_unknowns) {
    int parsed_mandatory = 0;
    // Iterate over argv until reaching the NULL terminator.

    while (labels[*current_index]) {
        const char *current_node_label = labels[*current_index];
        ParserNode search_entry = {.key = current_node_label};
        ParserNode *child = (ParserNode *) hashmap_get(node->children, &search_entry);
        if (!child) {
            // If at root and the flag is unrecognized, return PARSE_UNKNOWN_ARG.
            if (is_root) {
                if (skip_unknowns) {
                    (*current_index)++;
                    continue;
                }
                return UNKNOWN_ARG->of(UNKNOWN_ARG, current_node_label);
            }
            // If the mandatory count is already met, exit the loop.
            if (parsed_mandatory == node->mandatory_flags_count)
                break;
            // Otherwise, a mandatory flag is missing.
            return MISSING_MANDATORY_FLAG;
        }
        // Parse the flag.
        const ParsingResult *result = child->flag->parse(child->flag, cmd_options, labels, current_index);
        // Advance to the next argument. TODO
        printf("%s\t",labels[*current_index]);
        (*current_index)++;

        if (PARSE_SUCCESS != result->state)
            return result;
        if (child->flag->is_mandatory(child->flag))
            parsed_mandatory++;

        // If the node has children, parse them recursively.
        if (!child->children) {
            continue;
        }
        result = recursive_parse(child, cmd_options, labels, current_index, false, skip_unknowns);
        if (PARSE_SUCCESS != result->state)
            return result;
    }
    return parsed_mandatory == node->mandatory_flags_count ? SUCCESS : MISSING_MANDATORY_FLAG;
}

// Creates a hashmap of ParserNodes from an array of flags. //TODO
static struct hashmap *make_hashmap_nodes_from_flags(const struct FlagsArray *flags) {
    if (!flags->number_of_flags || !flags->flags) return NULL;
    struct hashmap *map = hashmap_new(sizeof(ParserNode), flags->number_of_flags, rand(), rand(), hash, compare,
                                      free_parser_node, NULL);
    for (int i = 0; i < flags->number_of_flags; i++) {
        // Count how many child flags are mandatory.
        int child_mandatory_count = 0;
        if (flags->flags[i]->children->flags && flags->flags[i]->children->number_of_flags > 0) {
            for (int j = 0; j < flags->flags[i]->children->number_of_flags; j++) {
                if (flags->flags[i]->children->flags[j]->is_mandatory(flags->flags[i]->children->flags[j]))
                    child_mandatory_count++;
            }
        }
        // Create the node for the current flag.
        ParserNode entry = {
            .key = flags->flags[i]->get_label(flags->flags[i]),
            .flag = flags->flags[i],
            .mandatory_flags_count = child_mandatory_count,
            .children = make_hashmap_nodes_from_flags(flags->flags[i]->children)
        };

        // Insert the node into the hashmap; handle duplicates if needed.
        if (hashmap_set(map, &entry)) {
            // Duplicate entries (handle if necessary).
        }
    }
    return map;
}

// Initializes the FlagParser from an array of flags.
FlagParser* init_flag_parser(struct FlagsArray *f_a) {
    int top_mandatory_count = 0;
    // Count mandatory flags at the top level.
    for (int i = 0; i < f_a->number_of_flags; i++) {
        if (f_a->flags[i]->is_mandatory(f_a->flags[i]))
            top_mandatory_count++;
    }
    const FlagParser parser = {
        .map = make_hashmap_nodes_from_flags(f_a),
        .mandatory_flags_count = top_mandatory_count
    };
    return memdup(&parser, sizeof(parser));
}

// Parses the command-line arguments using the FlagParser.
const ParsingResult *parse_flags_with_parser(
    CmdOptions *cmd_options,
    const FlagParser *parser,
    const char **labels,
    const bool skip_unknowns) {
    const ParserNode root = {.children = parser->map, .mandatory_flags_count = parser->mandatory_flags_count};
    unsigned int current_index = 0;
    return recursive_parse(&root, cmd_options, labels, &current_index, true, skip_unknowns);
}

void free_flag_parser(FlagParser *parser) {
    if (!parser) return;
    if (parser->map) hashmap_free(parser->map);
    free(parser);
}
