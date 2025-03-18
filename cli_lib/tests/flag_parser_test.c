#include <stdio.h>
#include <assert.h>
#include <flag_parser.h>
#include "flag.h"

struct CmdOptions {
    int integer;
    unsigned int u_integer;
    bool boolean;
};

ParsingResult set_integer(CmdOptions *cmd_options, const char **arg) {
    return parse_int(*(arg + 1), &cmd_options->integer);
}

ParsingResult set_u_integer(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 2), &cmd_options->u_integer);
}

ParsingResult set_boolean(CmdOptions *cmd_options, const char **arg) {
    cmd_options->boolean = true;
    return PARSE_SUCCESS;
}

ParsingResult dummy_parse(CmdOptions *cmd_options, const char **arg) {
    return PARSE_SUCCESS;
}

struct FlagsArray empty_children = {NULL, 0};

void test_case1_all_right() {
    CmdOptions option = {0, 0, false};
    const char *argv[] = {
        "test",
        "--boolean",
        "--u_integer", "1", "10", "1",
        "--integer", "100",
        NULL
    };
    const Flag *tsp_flags[] = {
        init_flag("--integer", 1, set_integer, true, empty_children),
        init_flag("--u_integer", 3, set_u_integer, true, empty_children),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const FlagParser *flag_parser = init_flag_parser((struct FlagsArray){tsp_flags, 3});
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 9, argv);
    assert(PARSE_SUCCESS == result);
    assert(true == option.boolean);
    assert(10 == option.u_integer);
    assert(100 == option.integer);
}

void test_case2_missing_value() {
    CmdOptions option = {0, 0, false};
    const char *argv[] = {
        "test",
        "--boolean",
        "--integer",
        NULL
    };
    const Flag *tsp_flags[] = {
        init_flag("--integer", 1, set_integer, true, empty_children),
        init_flag("--u_integer", 3, set_u_integer, true, empty_children),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const FlagParser *flag_parser = init_flag_parser((struct FlagsArray){tsp_flags, 3});
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 4, argv);
    assert(PARSE_MISSING_VALUE == result);
}

void test_case3_missing_mandatory_flag() {
    CmdOptions option = {0, 0, false};
    const char *argv[] = {
        "test",
        "--integer", "100",
        "--u_integer", "1", "10", "1",
        NULL
    };
    const Flag *tsp_flags[] = {
        init_flag("--integer", 1, set_integer, true, empty_children),
        init_flag("--u_integer", 3, set_u_integer, true, empty_children),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const FlagParser *flag_parser = init_flag_parser((struct FlagsArray){tsp_flags, 3});
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 8, argv);
    assert(PARSE_MISSING_MANDATORY_FLAG == result);
}

void test_case4_wrong_value_type() {
    CmdOptions option = {0, 0, false};
    const char *argv[] = {
        "test",
        "--boolean",
        "--u_integer", "1", "10", "1",
        "--integer", "not_a_number",
        NULL
    };
    const Flag *tsp_flags[] = {
        init_flag("--integer", 1, set_integer, true, empty_children),
        init_flag("--u_integer", 3, set_u_integer, true, empty_children),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const FlagParser *flag_parser = init_flag_parser((struct FlagsArray){tsp_flags, 3});
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 9, argv);
    assert(PARSE_WRONG_VALUE_TYPE == result);
}

void test_case5_nested_flags() {
    CmdOptions option = {0, 0, false};
    const Flag *child_flags[] = {
        init_flag("--child", 1, set_integer, true, empty_children)
    };
    const struct FlagsArray child_array = {child_flags, 1};
    const Flag *top_flags[] = {
        init_flag("--parent", 0, dummy_parse, true, child_array),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const struct FlagsArray top_array = {top_flags, 2};
    const char *argv[] = {"test", "--parent", "--child", "150", "--boolean", NULL};
    const FlagParser *flag_parser = init_flag_parser(top_array);
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 5, argv);
    assert(PARSE_SUCCESS == result);
    assert(150 == option.integer);
    assert(true == option.boolean);
}

void test_case6_double_nested_flags() {
    CmdOptions option = {0, 0, false};
    const Flag *grandchild = init_flag("--grandchild", 1, set_integer, true, empty_children);
    const struct FlagsArray grandchild_array = {&grandchild, 1};
    const Flag *child = init_flag("--child", 0, dummy_parse, true, grandchild_array);
    const struct FlagsArray child_array = {&child, 1};
    const Flag *parent = init_flag("--parent", 0, dummy_parse, true, child_array);
    const Flag *top_flags[] = {
        parent,
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const struct FlagsArray top_array = {top_flags, 2};
    const char *argv[] = {"test", "--parent", "--child", "--grandchild", "250", "--boolean", NULL};
    const FlagParser *flag_parser = init_flag_parser(top_array);
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 6, argv);
    assert(PARSE_SUCCESS == result);
    assert(250 == option.integer);
    assert(true == option.boolean);
}

void test_case7_unknown_flag() {
    CmdOptions option = {0, 0, false};
    const char *argv[] = {
        "test",
        "--unknown",
        "--integer", "100",
        "--u_integer", "1", "10", "1",
        "--boolean",
        NULL
    };
    const Flag *tsp_flags[] = {
        init_flag("--integer", 1, set_integer, true, empty_children),
        init_flag("--u_integer", 3, set_u_integer, true, empty_children),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const FlagParser *flag_parser = init_flag_parser((struct FlagsArray){tsp_flags, 3});
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 9, argv);
    assert(PARSE_UNKNOWN_ARG == result);
}

void test_case8_empty_flags() {
    CmdOptions option = {0, 0, false};
    const char *argv[] = {"test", NULL};
    const Flag *tsp_flags[] = {
        init_flag("--integer", 1, set_integer, true, empty_children),
        init_flag("--u_integer", 3, set_u_integer, true, empty_children),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const FlagParser *flag_parser = init_flag_parser((struct FlagsArray){tsp_flags, 3});
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 1, argv);
    assert(PARSE_MISSING_MANDATORY_FLAG == result);
}

void test_case9_repeated_non_mandatory_flag() {
    CmdOptions option = {0, 0, false};
    const Flag *tsp_flags[] = {
        init_flag("--integer", 1, set_integer, true, empty_children),
        init_flag("--u_integer", 3, set_u_integer, true, empty_children),
        init_flag("--boolean", 0, set_boolean, false, empty_children)
    };
    const char *argv[] = {
        "test",
        "--boolean",
        "--boolean",
        "--integer", "100",
        "--u_integer", "1", "10", "1",
        NULL
    };
    const FlagParser *flag_parser = init_flag_parser((struct FlagsArray){tsp_flags, 3});
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 8, argv);
    assert(PARSE_SUCCESS == result);
    assert(true == option.boolean);
    assert(100 == option.integer);
    assert(10 == option.u_integer);
}

void test_case10_out_of_order_child() {
    CmdOptions option = {0, 0, false};
    const Flag *child_flags[] = {
        init_flag("--child", 1, set_integer, true, empty_children)
    };
    const struct FlagsArray child_array = {child_flags, 1};
    const Flag *top_flags[] = {
        init_flag("--parent", 0, dummy_parse, true, child_array),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const struct FlagsArray top_array = {top_flags, 2};
    const char *argv[] = {"test", "--parent", "--boolean", "--child", "150", NULL};
    const FlagParser *flag_parser = init_flag_parser(top_array);
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 5, argv);
    assert(PARSE_MISSING_MANDATORY_FLAG == result);
}

void test_case11_extra_parameters() {
    CmdOptions option = {0, 0, false};
    const char *argv[] = {
        "test",
        "--boolean",
        "extra",
        "--integer", "100",
        "--u_integer", "1", "10", "1",
        NULL
    };
    const Flag *tsp_flags[] = {
        init_flag("--integer", 1, set_integer, true, empty_children),
        init_flag("--u_integer", 3, set_u_integer, true, empty_children),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const FlagParser *flag_parser = init_flag_parser((struct FlagsArray){tsp_flags, 3});
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 9, argv);
    assert(PARSE_UNKNOWN_ARG == result);
}

void test_case12_optional_child_omitted() {
    CmdOptions option = {0, 0, false};
    const Flag *child_flags[] = {
        init_flag("--child", 1, set_integer, false, empty_children)
    };
    const struct FlagsArray child_array = {child_flags, 1};
    const Flag *top_flags[] = {
        init_flag("--parent", 0, dummy_parse, true, child_array),
        init_flag("--boolean", 0, set_boolean, true, empty_children)
    };
    const struct FlagsArray top_array = {top_flags, 2};
    const char *argv[] = {"test", "--parent", "--boolean", NULL};
    const FlagParser *flag_parser = init_flag_parser(top_array);
    const ParsingResult result = parse_flags_with_parser(&option, flag_parser, 4, argv);
    assert(PARSE_SUCCESS == result);
    assert(true == option.boolean);
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
    test_case5_nested_flags();
    printf("test_case5_nested_flags passed.\n");
    test_case6_double_nested_flags();
    printf("test_case6_double_nested_flags passed.\n");
    test_case7_unknown_flag();
    printf("test_case7_unknown_flag passed.\n");
    test_case8_empty_flags();
    printf("test_case8_empty_flags passed.\n");
    test_case9_repeated_non_mandatory_flag();
    printf("test_case9_repeated_non_mandatory_flag passed.\n");
    test_case10_out_of_order_child();
    printf("test_case10_out_of_order_child passed.\n");
    test_case11_extra_parameters();
    printf("test_case11_extra_parameters passed.\n");
    test_case12_optional_child_omitted();
    printf("test_case12_optional_child_omitted passed.\n");
    printf("All tests passed.\n");
    return 0;
}
