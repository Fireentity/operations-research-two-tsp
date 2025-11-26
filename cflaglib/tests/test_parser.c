#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#include "flag_parser.h"
#include "parsing_result.h"

#define UNSET_INT (-9999)

typedef struct {
    bool show_help;
    int items;
    unsigned int threads;
    double probability;
    const char *output_file;

    bool enable_feature_a;
    int feature_a_param;

    int required_count;
    const char *config_file;
} TestOptions;

static void reset_options(TestOptions *opts) {
    memset(opts, 0, sizeof(TestOptions));
    opts->feature_a_param = UNSET_INT;
}

void test_basic_parsing() {
    printf("Running test_basic_parsing...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    flag_parser_add_bool(parser, "--help", "-h", "", &opts.show_help, FLAG_OPTIONAL);
    flag_parser_add_int(parser, "--items", "-i", "", &opts.items, FLAG_OPTIONAL);
    flag_parser_add_uint(parser, "--threads", "-t", "", &opts.threads, FLAG_OPTIONAL);
    flag_parser_add_double(parser, "--prob", "-p", "", &opts.probability, FLAG_OPTIONAL);
    flag_parser_add_string(parser, "--out", "-o", "", &opts.output_file, FLAG_OPTIONAL);

    const char *args[] = {
        "-h",
        "-i", "-50",
        "--threads", "8",
        "-p", "0.75",
        "--out", "results.txt"
    };

    const ParsingResult *res = flag_parser_parse(parser, 9, args, false);

    assert(res == SUCCESS);
    assert(opts.show_help == true);
    assert(opts.items == -50);
    assert(opts.threads == 8);
    assert(opts.probability > 0.749 && opts.probability < 0.751);
    assert(strcmp(opts.output_file, "results.txt") == 0);

    flag_parser_free(parser);
}

void test_error_unknown_arg() {
    printf("Running test_error_unknown_arg...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);
    flag_parser_add_bool(parser, "--help", "-h", "", &opts.show_help, FLAG_OPTIONAL);

    const char *args1[] = {"--help", "--unknown-flag"};
    const ParsingResult *res1 = flag_parser_parse(parser, 2, args1, false);
    assert(res1 == UNKNOWN_ARG);

    flag_parser_reset_visited(parser);

    const char *args2[] = {"--help", "--unknown-flag"};
    const ParsingResult *res2 = flag_parser_parse(parser, 2, args2, true);

    assert(res2 == SUCCESS);
    assert(opts.show_help == true);

    flag_parser_free(parser);
}

void test_error_missing_value() {
    printf("Running test_error_missing_value...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    flag_parser_add_int(parser, "--items", "-i", "", &opts.items, FLAG_OPTIONAL);
    flag_parser_add_bool(parser, "--help", "-h", "", &opts.show_help, FLAG_OPTIONAL);

    const char *args1[] = {"--items"};
    assert(flag_parser_parse(parser, 1, args1, false) == MISSING_VALUE);

    flag_parser_reset_visited(parser);

    const char *args2[] = {"--items", "--help"};
    assert(flag_parser_parse(parser, 2, args2, false) == MISSING_VALUE);

    flag_parser_reset_visited(parser);

    const char *args3[] = {"--items", "-10"};
    assert(flag_parser_parse(parser, 2, args3, false) == SUCCESS);
    assert(opts.items == -10);

    flag_parser_free(parser);
}

void test_error_wrong_value_type() {
    printf("Running test_error_wrong_value_type...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    flag_parser_add_int(parser, "--int", NULL, "", &opts.items, FLAG_OPTIONAL);
    flag_parser_add_uint(parser, "--uint", NULL, "", &opts.threads, FLAG_OPTIONAL);
    flag_parser_add_double(parser, "--float", NULL, "", &opts.probability, FLAG_OPTIONAL);

    const char *args_int[] = {"--int", "abc"};
    assert(flag_parser_parse(parser, 2, args_int, false) == WRONG_VALUE_TYPE);

    flag_parser_reset_visited(parser);

    const char *args_uint[] = {"--uint", "-5"};
    assert(flag_parser_parse(parser, 2, args_uint, false) == WRONG_VALUE_TYPE);

    flag_parser_reset_visited(parser);

    const char *args_float[] = {"--float", "1.2.3"};
    assert(flag_parser_parse(parser, 2, args_float, false) == WRONG_VALUE_TYPE);

    flag_parser_reset_visited(parser);

    const char *args_overflow[] = {"--int", "99999999999999999999999999999"};
    assert(flag_parser_parse(parser, 2, args_overflow, false) == WRONG_VALUE_TYPE);

    flag_parser_free(parser);
}

void test_mandatory_flags() {
    printf("Running test_mandatory_flags...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    flag_parser_add_int(parser, "--count", NULL, "", &opts.required_count, FLAG_MANDATORY);
    flag_parser_add_bool(parser, "--help", "-h", "", &opts.show_help, FLAG_OPTIONAL);

    const char *args1[] = {"-h"};
    const ParsingResult *res_parse1 = flag_parser_parse(parser, 1, args1, false);
    assert(res_parse1 == SUCCESS);
    assert(flag_parser_validate(parser) == MISSING_MANDATORY_FLAG);

    reset_options(&opts);
    flag_parser_free(parser);

    parser = flag_parser_new(&opts);
    flag_parser_add_int(parser, "--count", NULL, "", &opts.required_count, FLAG_MANDATORY);
    flag_parser_add_bool(parser, "--help", "-h", "", &opts.show_help, FLAG_OPTIONAL);

    const char *args2[] = {"--count", "100", "-h"};
    const ParsingResult *res_parse2 = flag_parser_parse(parser, 3, args2, false);
    const ParsingResult *res_valid2 = flag_parser_validate(parser);

    assert(res_parse2 == SUCCESS);
    assert(res_valid2 == SUCCESS);
    assert(opts.required_count == 100);
    assert(opts.show_help == true);

    flag_parser_free(parser);
}

void test_error_usage() {
    printf("Running test_error_usage...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    flag_parser_add_int(parser, "--count", NULL, "", &opts.required_count, FLAG_MANDATORY);

    const char *args[] = {"--count", "100", "--count", "200"};
    assert(flag_parser_parse(parser, 4, args, false) == USAGE_ERROR);

    flag_parser_free(parser);
}

void test_optional_overwrite() {
    printf("Running test_optional_overwrite...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    flag_parser_add_int(parser, "--items", NULL, "", &opts.items, FLAG_OPTIONAL);

    const char *args[] = {"--items", "100", "--items", "200"};
    assert(flag_parser_parse(parser, 4, args, false) == USAGE_ERROR);
    assert(opts.items == 100);

    flag_parser_free(parser);
}

void test_mark_visited() {
    printf("Running test_mark_visited (INI file emulation)...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    flag_parser_add_int(parser, "--count", NULL, "", &opts.required_count, FLAG_MANDATORY);
    flag_parser_add_string(parser, "--config", NULL, "", &opts.config_file, FLAG_OPTIONAL);

    opts.required_count = 500;
    assert(flag_parser_mark_visited(parser, "--count") == true);

    const char *args[] = {"--config", "my.ini"};
    assert(flag_parser_parse(parser, 2, args, false) == SUCCESS);

    assert(flag_parser_validate(parser) == SUCCESS);
    assert(opts.required_count == 500);
    assert(strcmp(opts.config_file, "my.ini") == 0);

    flag_parser_free(parser);
}

void test_logical_grouping() {
    printf("Running test_logical_grouping...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    flag_parser_add_bool(parser, "--feature-a", NULL, "", &opts.enable_feature_a, FLAG_OPTIONAL);
    flag_parser_add_int(parser, "--param-a", NULL, "", &opts.feature_a_param, FLAG_OPTIONAL);

    const char *args1[] = {"--feature-a", "--param-a", "0"};
    assert(flag_parser_parse(parser, 3, args1, false) == SUCCESS);
    assert(opts.enable_feature_a == true);
    assert(opts.feature_a_param == 0);

    reset_options(&opts);
    flag_parser_free(parser);

    parser = flag_parser_new(&opts);
    flag_parser_add_bool(parser, "--feature-a", NULL, "", &opts.enable_feature_a, FLAG_OPTIONAL);
    flag_parser_add_int(parser, "--param-a", NULL, "", &opts.feature_a_param, FLAG_OPTIONAL);

    const char *args2[] = {"--feature-a"};
    assert(flag_parser_parse(parser, 1, args2, false) == SUCCESS);
    assert(opts.enable_feature_a == true);
    assert(opts.feature_a_param == UNSET_INT);

    flag_parser_free(parser);
}

void test_terminator() {
    printf("Running test_terminator (--)...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    flag_parser_add_int(parser, "--items", "-i", "", &opts.items, FLAG_OPTIONAL);
    flag_parser_add_string(parser, "--out", "-o", "", &opts.output_file, FLAG_OPTIONAL);

    const char *args[] = {"-i", "10", "--", "-o", "filename.txt"};
    assert(flag_parser_parse(parser, 5, args, false) == SUCCESS);
    assert(opts.items == 10);
    assert(opts.output_file == NULL);

    flag_parser_free(parser);
}

int main() {
    printf("--- Starting Flag Parser Test Suite ---\n\n");
    test_basic_parsing();
    test_error_unknown_arg();
    test_error_missing_value();
    test_error_wrong_value_type();
    test_mandatory_flags();
    test_error_usage();
    test_optional_overwrite();
    test_mark_visited();
    test_logical_grouping();
    test_terminator();
    printf("\n--- All tests passed successfully! ---\n");
    return 0;
}
