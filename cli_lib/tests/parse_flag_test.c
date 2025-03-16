#include <stdio.h>
#include <assert.h>
#include "flag.h"

struct CmdOptions {
    int integer;
    unsigned int u_integer;
    bool boolean;
};

ParsingResult set_integer(CmdOptions *cmd_options, const char **arg) {
    // Parse the integer from the argument immediately following the flag.
    return parse_int(*(arg + 1), &cmd_options->integer);
}

ParsingResult set_u_integer(CmdOptions *cmd_options, const char **arg) {
    // Parse the unsigned integer from the second parameter of the flag.
    return parse_unsigned_int(*(arg + 2), &cmd_options->u_integer);
}

ParsingResult set_boolean(CmdOptions *cmd_options, const char **arg) {
    cmd_options->boolean = true;
    return PARSE_SUCCESS;
}

void test_case1_all_right() {
    struct CmdOptions option = {0, 0, 0};
    const char *argv[] = {
            "test",
            "--boolean",
            "--u_integer", "1", "10", "1",
            "--integer", "100",
            NULL
    };
    const Flag *tsp_flags[] = {
            init_flag("--integer", 1, set_integer, true),
            init_flag("--u_integer", 3, set_u_integer, true), // u_integer flag uses its second parameter.
            init_flag("--boolean", 0, set_boolean, true),
    };
    ParsingResult result = parse_flags(&option, tsp_flags, 3, 9, argv);
    assert(PARSE_SUCCESS == result);
    assert(true == option.boolean);
    assert(10 == option.u_integer);
    assert(100 == option.integer);
}

void test_case2_missing_value() {
    struct CmdOptions option = {0, 0, false};
    const char *argv[] = {
            "test",
            "--boolean",
            "--integer",  // Flag expecting a value, but none provided.
            NULL
    };
    const Flag *tsp_flags[] = {
            init_flag("--integer", 1, set_integer, true),
            init_flag("--u_integer", 3, set_u_integer, true),
            init_flag("--boolean", 0, set_boolean, true)
    };
    ParsingResult result = parse_flags(&option, tsp_flags, 3, 4, argv);
    assert(PARSE_MISSING_VALUE == result);
}

void test_case3_missing_mandatory_flag() {
    struct CmdOptions option = {0, 0, false};
    const char *argv[] = {
            "test",
            "--integer", "100",
            "--u_integer", "1", "10", "1",
            NULL
    };
    const Flag *tsp_flags[] = {
            init_flag("--integer", 1, set_integer, true),
            init_flag("--u_integer", 3, set_u_integer, true),
            init_flag("--boolean", 0, set_boolean, true) // Mandatory flag not provided.
    };
    ParsingResult result = parse_flags(&option, tsp_flags, 3, 8, argv);
    assert(PARSE_MISSING_MANDATORY_FLAG == result);
}

void test_case4_wrong_value_type() {
    struct CmdOptions option = {0, 0, false};
    const char *argv[] = {
            "test",
            "--boolean",
            "--u_integer", "1", "10", "1",
            "--integer", "not_a_number", // Non-numeric string for --integer.
            NULL
    };
    const Flag *tsp_flags[] = {
            init_flag("--integer", 1, set_integer, true),
            init_flag("--u_integer", 3, set_u_integer, true),
            init_flag("--boolean", 0, set_boolean, true)
    };
    ParsingResult result = parse_flags(&option, tsp_flags, 3, 9, argv);
    assert(PARSE_WRONG_VALUE_TYPE == result);
}

int main() {
    test_case1_all_right();
    printf("test_case1_all_right passed.\n");
    test_case2_missing_value();
    printf("test_case2_missing_value passed.\n");
    test_case3_missing_mandatory_flag();
    printf("test_case3_missing_mandatory_flag passed.\n");
    test_case4_wrong_value_type();
    printf("test_case4_wrong_value_type passed.\n");
    printf("All test passed.\n");
    return 0;
}
