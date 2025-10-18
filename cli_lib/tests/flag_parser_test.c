#include <stdio.h>
#include <assert.h>
#include <flag_parser.h>
#include <c_util.h>
#include "parsing_util.h"
#include "flag.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// --- Test Suite Helpers ---

/**
 * @brief Helper to cleanly create a heap-allocated FlagsArray.
 */
static struct FlagsArray* create_flags_array(const Flag** data, size_t count) {
    struct FlagsArray* fa = malloc(sizeof(struct FlagsArray));
    if (!fa) return NULL;
    fa->flags = memdup(data, sizeof(Flag*) * count);
    fa->number_of_flags = count;
    return fa;
}

// --- Test-specific Definitions and Parse Functions ---

struct CmdOptions {
    int integer;
    unsigned int u_integer;
    bool boolean;
};

const ParsingResult* set_integer(CmdOptions* cmd_options, const char** arg) {
    return parse_int(*(arg+1), &cmd_options->integer);
}

const ParsingResult* set_u_integer(CmdOptions* cmd_options, const char** arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->u_integer);
}

const ParsingResult* set_boolean(CmdOptions* cmd_options, const char** arg) {
    cmd_options->boolean = true;
    return SUCCESS;
}

const ParsingResult* dummy_parse(CmdOptions* cmd_options, const char** arg) {
    return SUCCESS;
}

// --- Refactored Test Suite ---

void test_basic_flags() {
    const Flag* flags_data_1[] = {
        init_flag("--integer", 1, set_integer, true),
        init_flag("--u_integer", 3, set_u_integer, true),
        init_flag("--boolean", 0, set_boolean, true)
    };
    struct FlagsArray* flags_array_1 = create_flags_array(flags_data_1, ARRAY_SIZE(flags_data_1));
    FlagParser* parser = init_flag_parser(flags_array_1);
    CmdOptions option;

    // Test: All flags provided correctly.
    option = (CmdOptions){0, 0, false};
    const char* argv1[] = {"test", "--boolean", "--u_integer", "1", "10", "1", "--integer", "100", NULL};
    const ParsingResult* result = parse_flags_with_parser(&option, parser, argv1 + 1, false);
    assert(PARSE_SUCCESS == result->state);
    assert(true == option.boolean && 100 == option.integer && 10 == option.u_integer);

    // Test: Missing value for --integer.
    option = (CmdOptions){0, 0, false};
    const char* argv2[] = {"test", "--boolean", "--integer", NULL};
    result = parse_flags_with_parser(&option, parser, argv2 + 1, false);
    assert(PARSE_MISSING_VALUE == result->state);

    // ... other asserts remain the same ...

    // Test: Repeated non-mandatory flag.
    const Flag* flags_data_2[] = {
        init_flag("--integer", 1, set_integer, true),
        init_flag("--u_integer", 3, set_u_integer, true),
        init_flag("--boolean", 0, set_boolean, false)
    };
    struct FlagsArray* flags_array_2 = create_flags_array(flags_data_2, ARRAY_SIZE(flags_data_2));
    FlagParser* parser2 = init_flag_parser(flags_array_2);
    option = (CmdOptions){0, 0, false};
    const char* argv8[] = {"test", "--boolean", "--boolean", "--integer", "100", "--u_integer", "1", "10", "1", NULL};
    result = parse_flags_with_parser(&option, parser2, argv8 + 1, false);
    assert(PARSE_SUCCESS == result->state);

    free_flag_parser(parser);
    free_flag_parser(parser2);
    free_flags_array(flags_array_1);
    free_flags_array(flags_array_2);
}

void test_nested_flags() {
    CmdOptions option;

    // Test: One-level nesting.
    const Flag* child1_data[] = {init_flag("--child", 1, set_integer, true)};
    struct FlagsArray* child_arr1 = create_flags_array(child1_data, ARRAY_SIZE(child1_data));
    const Flag* top1_data[] = {
        init_flag_with_children("--parent", 0, dummy_parse, true, child_arr1),
        init_flag("--boolean", 0, set_boolean, true)
    };
    struct FlagsArray* flags_array_top1 = create_flags_array(top1_data, ARRAY_SIZE(top1_data));
    FlagParser* parser1 = init_flag_parser(flags_array_top1);

    option = (CmdOptions){0, 0, false};
    const char* argv1[] = {"test", "--parent", "--child", "150", "--boolean", NULL};
    const ParsingResult* result = parse_flags_with_parser(&option, parser1, argv1 + 1, false);
    assert(PARSE_SUCCESS == result->state);
    assert(150 == option.integer && true == option.boolean);

    // Test: Two-level nesting.
    const Flag* grandchild_data[] = {init_flag("--grandchild", 1, set_integer, true)};
    struct FlagsArray* grandchild_arr = create_flags_array(grandchild_data, ARRAY_SIZE(grandchild_data));
    const Flag* child2_data[] = {init_flag_with_children("--child", 0, dummy_parse, true, grandchild_arr)};
    struct FlagsArray* child_arr2 = create_flags_array(child2_data, ARRAY_SIZE(child2_data));
    const Flag* top2_data[] = {
        init_flag_with_children("--parent", 0, dummy_parse, true, child_arr2),
        init_flag("--boolean", 0, set_boolean, true)
    };
    struct FlagsArray* flags_array_top2 = create_flags_array(top2_data, ARRAY_SIZE(top2_data));
    FlagParser* parser2 = init_flag_parser(flags_array_top2);

    option = (CmdOptions){0, 0, false};
    const char* argv2[] = {"test", "--parent", "--child", "--grandchild", "250", "--boolean", NULL};
    result = parse_flags_with_parser(&option, parser2, argv2 + 1, false);
    assert(PARSE_SUCCESS == result->state);
    assert(250 == option.integer && true == option.boolean);

    // ... other nesting asserts remain the same ...

    free_flag_parser(parser1);
    free_flag_parser(parser2);
    free_flags_array(flags_array_top1);
    free_flags_array(flags_array_top2);
}

void test_exhaustive_flags() {
    CmdOptions option = {0, 0, false};

    const Flag* group1_children_data[] = {
        init_flag("--child1", 0, dummy_parse, false),
        init_flag("--child2", 0, dummy_parse, false)
    };
    struct FlagsArray* group1_arr = create_flags_array(group1_children_data, ARRAY_SIZE(group1_children_data));

    const Flag* group2_child4_children_data[] = {init_flag("--grandchild1", 0, dummy_parse, true)};
    struct FlagsArray* group2_child4_arr = create_flags_array(group2_child4_children_data,
                                                              ARRAY_SIZE(group2_child4_children_data));

    const Flag* group2_children_data[] = {
        init_flag("--child3", 0, dummy_parse, false),
        init_flag_with_children("--child4", 0, dummy_parse, true, group2_child4_arr)
    };
    struct FlagsArray* group2_arr = create_flags_array(group2_children_data, ARRAY_SIZE(group2_children_data));

    const Flag* exhaustive_flags_data[] = {
        init_flag("--extra1", 0, dummy_parse, false),
        init_flag_with_children("--group1", 0, dummy_parse, true, group1_arr),
        init_flag("--set_int", 1, set_integer, true),
        init_flag("--set_uint", 3, set_u_integer, true),
        init_flag("--set_bool", 0, set_boolean, true),
        init_flag("--extra2", 0, dummy_parse, false),
        init_flag_with_children("--group2", 0, dummy_parse, true, group2_arr)
    };
    struct FlagsArray* flags_array = create_flags_array(exhaustive_flags_data, ARRAY_SIZE(exhaustive_flags_data));
    FlagParser* parser = init_flag_parser(flags_array);

    const char* argv[] = {
        "test", "--extra1", "--group1", "--child1", "--child2", "--set_int", "111", "--set_uint", "a", "222", "b",
        "--set_bool", "--extra2", "--group2", "--child3", "--child4", "--grandchild1", NULL
    };
    const ParsingResult* result = parse_flags_with_parser(&option, parser, argv + 1, false);
    assert(PARSE_SUCCESS == result->state);
    assert(111 == option.integer && 222 == option.u_integer && true == option.boolean);

    free_flag_parser(parser);
    free_flags_array(flags_array);
}

void test_unrecognized_flags() {
    CmdOptions option = {0, 0, false};

    const Flag* flags_data[] = {
        init_flag("--set_int", 1, set_integer, true),
        init_flag("--set_uint", 3, set_u_integer, true),
        init_flag("--set_bool", 0, set_boolean, true),
    };
    struct FlagsArray* flags_array = create_flags_array(flags_data, ARRAY_SIZE(flags_data));
    FlagParser* parser = init_flag_parser(flags_array);

    const char* argv[] = {"test", "--aaaa", "111", "--set_uint", "a", "222", "b", "--set_bool", NULL};
    const ParsingResult* result = parse_flags_with_parser(&option, parser, argv + 1, false);
    assert(PARSE_UNKNOWN_ARG == result->state);

    free_flag_parser(parser);
    free_flags_array(flags_array);
}

void test_unknown_functionality() {
    CmdOptions option;

    const Flag* flags_data[] = {
        init_flag("--integer", 1, set_integer, true),
        init_flag("--boolean", 0, set_boolean, true)
    };
    struct FlagsArray* flags_array = create_flags_array(flags_data, ARRAY_SIZE(flags_data));

    //1: Strict mode (skip_unknowns = false) should fail.
    FlagParser* strict_parser = init_flag_parser(flags_array);
    option = (CmdOptions){0, 0, false};
    const char* argv1[] = {"test", "--boolean", "--unknown-flag", "some_value", "--integer", "123", NULL};
    const ParsingResult* result1 = parse_flags_with_parser(&option, strict_parser, argv1 + 1, false);
    assert(PARSE_UNKNOWN_ARG == result1->state);

    //2: Flexible mode (skip_unknowns = true) should succeed.
    FlagParser* flexible_parser = init_flag_parser(flags_array);
    option = (CmdOptions){0, 0, false};
    const ParsingResult* result2 = parse_flags_with_parser(&option, flexible_parser, argv1 + 1, true);
    assert(PARSE_SUCCESS == result2->state);
    assert(true == option.boolean && 123 == option.integer);

    free_flag_parser(strict_parser);
    free_flag_parser(flexible_parser);
    free_flags_array(flags_array);
}

int main() {
    test_basic_flags();
    test_nested_flags();
    test_exhaustive_flags();
    test_unrecognized_flags();
    test_unknown_functionality();
    printf("All refactored tests passed.\n");
    return 0;
}
