#include <stdlib.h>
#include <string.h>
#include "flag_parser.h"
#include "flag.h"
#include "hashmap.h"
#include "c_util.h"

struct FlagParser {
    // hashmap of ParserNode
    const struct hashmap *map;
};

struct ParserNode {
    const char *key;
    const Flag *flag;
    // hashmap of ParserNode
    struct hashmap *children;
};

uint64_t hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const ParserNode *entry = *(const ParserNode **) item;
    return hashmap_xxhash3(entry->key, strlen(entry->key), seed0, seed1);
}

int compare(const void *a, const void *b, void *udata) {
    const ParserNode *ea = *(const ParserNode **) a;
    const ParserNode *eb = *(const ParserNode **) b;
    return strcmp(ea->key, eb->key);
}

void free_flag(void *item) {
    if(!item) return;
    struct ParserNode* entry = (ParserNode *)item;
    if(entry->children){
        hashmap_free(entry->children);
    }
}

static struct hashmap *make_hashmap_nodes_from_flags(const struct FlagsArray flags) {
    if (!flags.number_of_flags || !flags.flags) return NULL;

    struct hashmap *map = hashmap_new(sizeof(ParserNode), flags.number_of_flags, rand(), rand(), hash, compare,
                                      free_flag, NULL);
    for (int i = 0; i < flags.number_of_flags; i++) {
        ParserNode entry = {
                .key = flags.flags[i]->get_label(flags.flags[i]),
                .flag = flags.flags[i],
                .children = make_hashmap_nodes_from_flags(flags.flags[i]->children)
        };

        if (hashmap_set(map, &entry)) {
            // TODO Handle duplicate flag labels if necessary
        }
    }
    return map;
}

FlagParser *init_flag_parser(const struct FlagsArray flags) {
    struct hashmap *map = make_hashmap_nodes_from_flags(flags);
    FlagParser stack = {
            .map = map
    };

    return (FlagParser *) malloc_from_stack(&stack, sizeof(stack));
}



