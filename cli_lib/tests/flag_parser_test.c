#include <stdio.h>
#include <assert.h>
#include <flag_parser.h>
#include "parsing_util.h"
#include "flag.h"

// CmdOptions holds the parsed values for the flags.
struct CmdOptions {
    int integer;
    unsigned int u_integer;
    bool boolean;
};

/**
 * @brief Parses an integer value and sets the integer option.
 *
 * @param cmd_options Pointer to CmdOptions structure.
 * @param arg Array of arguments.
 * @return ParsingResult indicating the outcome.
 */
ParsingResult set_integer(CmdOptions *cmd_options, const char **arg) {
    return parse_int(*(arg + 1), &cmd_options->integer);
}

/**
 * @brief Parses an unsigned integer value and sets the u_integer option.
 *
 * @param cmd_options Pointer to CmdOptions structure.
 * @param arg Array of arguments.
 * @return ParsingResult indicating the outcome.
 */
ParsingResult set_u_integer(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 2), &cmd_options->u_integer);
}

/**
 * @brief Sets the boolean flag to true.
 *
 * @param cmd_options Pointer to CmdOptions structure.
 * @param arg Array of arguments.
 * @return ParsingResult indicating success.
 */
ParsingResult set_boolean(CmdOptions *cmd_options, const char **arg) {
    cmd_options->boolean = true;
    return PARSE_SUCCESS;
}

/**
 * @brief Dummy parse function that always succeeds.
 *
 * @param cmd_options Pointer to CmdOptions structure.
 * @param arg Array of arguments.
 * @return Always returns PARSE_SUCCESS.
 */
ParsingResult dummy_parse(CmdOptions *cmd_options, const char **arg) {
    return PARSE_SUCCESS;
}

/**
 * @brief Tests basic flag parsing scenarios.
 *
 * This function checks various cases including correct parsing,
 * missing values, missing mandatory flags, wrong value types, unknown arguments,
 * extra parameters, and repeated non-mandatory flags.
 */
void test_basic_flags() {
    const Flag *flags[] = {
        init_flag("--integer", 1, set_integer, true),
        init_flag("--u_integer", 3, set_u_integer, true),
        init_flag("--boolean", 0, set_boolean, true)
    };
    const FlagParser *parser = init_flag_parser((struct FlagsArray){flags, 3});
    CmdOptions option;

    // Test: All flags provided correctly.
    option = (CmdOptions){0, 0, false};
    const char *argv1[] = {"test", "--boolean", "--u_integer", "1", "10", "1", "--integer", "100", NULL};
    ParsingResult result = parse_flags_with_parser(&option, parser, argv1+1);
    assert(PARSE_SUCCESS == result);
    assert(true == option.boolean && 100 == option.integer && 10 == option.u_integer);

    // Test: Missing value for --integer.
    option = (CmdOptions){0, 0, false};
    const char *argv2[] = {"test", "--boolean", "--integer", NULL};
    result = parse_flags_with_parser(&option, parser, argv2+1);
    assert(PARSE_MISSING_VALUE == result);

    // Test: Missing mandatory flag (--boolean not provided).
    option = (CmdOptions){0, 0, false};
    const char *argv3[] = {"test", "--integer", "100", "--u_integer", "1", "10", "1", NULL};
    result = parse_flags_with_parser(&option, parser, argv3+1);
    assert(PARSE_MISSING_MANDATORY_FLAG == result);

    // Test: Wrong value type for --integer.
    option = (CmdOptions){0, 0, false};
    const char *argv4[] = {"test", "--boolean", "--u_integer", "1", "10", "1", "--integer", "not_a_number", NULL};
    result = parse_flags_with_parser(&option, parser, argv4+1);
    assert(PARSE_WRONG_VALUE_TYPE == result);

    // Test: Unknown flag provided.
    option = (CmdOptions){0, 0, false};
    const char *argv5[] = {"test", "--unknown", "--integer", "100", "--u_integer", "1", "10", "1", "--boolean", NULL};
    result = parse_flags_with_parser(&option, parser, argv5+1);
    assert(PARSE_UNKNOWN_ARG == result);

    // Test: No flags provided.
    option = (CmdOptions){0, 0, false};
    const char *argv6[] = {"test", NULL};
    result = parse_flags_with_parser(&option, parser, argv6+1);
    assert(PARSE_MISSING_MANDATORY_FLAG == result);

    // Test: Extra parameter provided to --boolean.
    option = (CmdOptions){0, 0, false};
    const char *argv7[] = {"test", "--boolean", "extra", "--integer", "100", "--u_integer", "1", "10", "1", NULL};
    result = parse_flags_with_parser(&option, parser, argv7+1);
    assert(PARSE_UNKNOWN_ARG == result);

    // Test: Repeated non-mandatory flag.
    const Flag *flags2[] = {
        init_flag("--integer", 1, set_integer, true),
        init_flag("--u_integer", 3, set_u_integer, true),
        init_flag("--boolean", 0, set_boolean, false)
    };
    const FlagParser *parser2 = init_flag_parser((struct FlagsArray){flags2, 3});
    option = (CmdOptions){0, 0, false};
    const char *argv8[] = {"test", "--boolean", "--boolean", "--integer", "100", "--u_integer", "1", "10", "1", NULL};
    result = parse_flags_with_parser(&option, parser2, argv8+1);
    assert(PARSE_SUCCESS == result);
    assert(true == option.boolean && 100 == option.integer && 10 == option.u_integer);
}

/**
 * @brief Tests parsing of nested flags.
 *
 * This function covers one-level and two-level nested flag scenarios,
 * including cases where a mandatory child flag is missing.
 */
void test_nested_flags() {
    CmdOptions option;
    // One-level nested flag.
    const Flag *child1[] = { init_flag("--child", 1, set_integer, true) };
    const struct FlagsArray child_arr1 = {child1, 1};
    const Flag *top1[] = {
        init_flag_with_children("--parent", 0, dummy_parse, true, child_arr1),
        init_flag("--boolean", 0, set_boolean, true)
    };
    const FlagParser *parser1 = init_flag_parser((struct FlagsArray){top1, 2});
    option = (CmdOptions){0, 0, false};
    const char *argv1[] = {"test", "--parent", "--child", "150", "--boolean", NULL};
    ParsingResult result = parse_flags_with_parser(&option, parser1, argv1+1);
    assert(PARSE_SUCCESS == result);
    assert(150 == option.integer && true == option.boolean);

    // Two-level nested flag.
    const Flag *grandchild[] = { init_flag("--grandchild", 1, set_integer, true) };
    const struct FlagsArray grandchild_arr = {grandchild, 1};
    const Flag *child2[] = { init_flag_with_children("--child", 0, dummy_parse, true, grandchild_arr) };
    const struct FlagsArray child_arr2 = {child2, 1};
    const Flag *top2[] = {
        init_flag_with_children("--parent", 0, dummy_parse, true, child_arr2),
        init_flag("--boolean", 0, set_boolean, true)
    };
    const FlagParser *parser2 = init_flag_parser((struct FlagsArray){top2, 2});
    option = (CmdOptions){0, 0, false};
    const char *argv2[] = {"test", "--parent", "--child", "--grandchild", "250", "--boolean", NULL};
    result = parse_flags_with_parser(&option, parser2, argv2+1);
    assert(PARSE_SUCCESS == result);
    assert(250 == option.integer && true == option.boolean);

    // Test: Mandatory nested flag missing.
    option = (CmdOptions){0, 0, false};
    const char *argv3[] = {"test", "--parent", "--boolean", "--child", "150", NULL};
    result = parse_flags_with_parser(&option, parser1, argv3+1);
    assert(PARSE_MISSING_MANDATORY_FLAG == result);

    // Test: Optional nested flag omitted.
    const Flag *child3[] = { init_flag("--child", 1, set_integer, false) };
    const struct FlagsArray child_arr3 = {child3, 1};
    const Flag *top3[] = {
        init_flag_with_children("--parent", 0, dummy_parse, true, child_arr3),
        init_flag("--boolean", 0, set_boolean, true)
    };
    const FlagParser *parser3 = init_flag_parser((struct FlagsArray){top3, 2});
    option = (CmdOptions){0, 0, false};
    const char *argv4[] = {"test", "--parent", "--boolean", NULL};
    result = parse_flags_with_parser(&option, parser3, argv4+1);
    assert(PARSE_SUCCESS == result);
    assert(true == option.boolean);
}

/**
 * @brief Tests comprehensive parsing with multiple groups and nested flags.
 *
 * This function validates a complex configuration with extra groups,
 * nested structures, and various flag types.
 */
void test_exhaustive_flags() {
    CmdOptions option = {0, 0, false};

    // Group 1: Two child flags.
    const Flag *group1_children[] = {
        init_flag("--child1", 0, dummy_parse, false),
        init_flag("--child2", 0, dummy_parse, false)
    };
    const struct FlagsArray group1_arr = {group1_children, 2};

    // Group 2: Child4 has a nested grandchild.
    const Flag *group2_child4_children[] = {
        init_flag("--grandchild1", 0, dummy_parse, true)
    };
    const struct FlagsArray group2_child4_arr = {group2_child4_children, 1};

    // Group 2: Two child flags.
    const Flag *group2_children[] = {
        init_flag("--child3", 0, dummy_parse, false),
        init_flag_with_children("--child4", 0, dummy_parse, true, group2_child4_arr)
    };
    const struct FlagsArray group2_arr = {group2_children, 2};

    // Exhaustive flags including extra groups and options.
    const Flag *exhaustive_flags[] = {
        init_flag("--extra1", 0, dummy_parse, false),
        init_flag_with_children("--group1", 0, dummy_parse, true, group1_arr),
        init_flag("--set_int", 1, set_integer, true),
        init_flag("--set_uint", 3, set_u_integer, true),
        init_flag("--set_bool", 0, set_boolean, true),
        init_flag("--extra2", 0, dummy_parse, false),
        init_flag_with_children("--group2", 0, dummy_parse, true, group2_arr)
    };
    const FlagParser *parser = init_flag_parser((struct FlagsArray){exhaustive_flags, 7});

    // Test: Complex flag configuration.
    const char *argv[] = {
        "test",
        "--extra1",
        "--group1",
            "--child1",
            "--child2",
        "--set_int", "111",
        "--set_uint", "a", "222", "b",
        "--set_bool",
        "--extra2",
        "--group2",
            "--child3",
            "--child4",
                "--grandchild1",
        NULL
    };
    const ParsingResult result = parse_flags_with_parser(&option, parser, argv+1);
    assert(PARSE_SUCCESS == result);
    assert(111 == option.integer && 222 == option.u_integer && true == option.boolean);
}

/**
 * @brief Main entry point.
 *
 * Executes all test cases to verify the flag parser functionality.
 */
int main() {
    test_basic_flags();
    test_nested_flags();
    test_exhaustive_flags();
    printf("All tests passed.\n");
    return 0;
}
