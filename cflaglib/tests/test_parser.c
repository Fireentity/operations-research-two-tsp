#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "flag_parser.h"
#include "parsing_result.h"

// --- Test Subject: A struct to hold our options ---

// A marker for testing "unset" logic (for the logical grouping test)
#define UNSET_INT -9999

typedef struct {
    // All our types
    bool show_help;
    int items;
    unsigned int threads;
    float probability;
    const char *output_file;

    // For logical group tests
    bool enable_feature_a; // Was 'vns'
    int feature_a_param; // Was 'kicks'

    // For mandatory tests
    int required_count; // Was 'nodes'
    const char *config_file;
} TestOptions;

/**
 * @brief Helper to reset the options struct before each test.
 */
static void reset_options(TestOptions *opts) {
    memset(opts, 0, sizeof(TestOptions));
    // Set special "unset" values
    opts->feature_a_param = UNSET_INT;
}

// --- Test Suite ---

void test_basic_parsing() {
    printf("Running test_basic_parsing...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    // 1. Register flags
    flag_parser_add_bool(parser, "--help", "-h", "", &opts.show_help, FLAG_OPTIONAL);
    flag_parser_add_int(parser, "--items", "-i", "", &opts.items, FLAG_OPTIONAL);
    flag_parser_add_uint(parser, "--threads", "-t", "", &opts.threads, FLAG_OPTIONAL);
    flag_parser_add_float(parser, "--prob", "-p", "", &opts.probability, FLAG_OPTIONAL);
    flag_parser_add_string(parser, "--out", "-o", "", &opts.output_file, FLAG_OPTIONAL);

    // 2. Define arguments
    const char *args[] = {
        "-h", // bool
        "-i", "-50", // int (negative)
        "--threads", "8", // uint
        "-p", "0.75", // float
        "--out", "results.txt" // string
    };

    // 3. Parse
    const ParsingResult *res = flag_parser_parse(parser, 9, args, false);

    // 4. Assert
    assert(res == SUCCESS);
    assert(opts.show_help == true);
    assert(opts.items == -50);
    assert(opts.threads == 8);
    // Be careful comparing floats
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

    // 1. Parse with skip_unknowns = false
    const char *args1[] = {"--help", "--unknown-flag"};
    const ParsingResult *res1 = flag_parser_parse(parser, 2, args1, false);

    // 2. Assert failure
    assert(res1 == UNKNOWN_ARG);

    // 3. Parse with skip_unknowns = true
    const char *args2[] = {"--help", "--unknown-flag"};
    const ParsingResult *res2 = flag_parser_parse(parser, 2, args2, true);

    // 4. Assert success
    assert(res2 == SUCCESS);
    assert(opts.show_help == true); // The known flag should still be parsed

    flag_parser_free(parser);
}

void test_error_missing_value() {
    printf("Running test_error_missing_value...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);
    flag_parser_add_int(parser, "--items", "-i", "", &opts.items, FLAG_OPTIONAL);
    flag_parser_add_bool(parser, "--help", "-h", "", &opts.show_help, FLAG_OPTIONAL);

    // 1. Value missing at the end
    const char *args1[] = {"--items"};
    const ParsingResult *res1 = flag_parser_parse(parser, 1, args1, false);
    assert(res1 == MISSING_VALUE);

    // 2. Value missing before another flag
    const char *args2[] = {"--items", "--help"};
    const ParsingResult *res2 = flag_parser_parse(parser, 2, args2, false);
    assert(res2 == MISSING_VALUE);

    // 3. This one should work (negative number)
    const char *args3[] = {"--items", "-10"};
    const ParsingResult *res3 = flag_parser_parse(parser, 2, args3, false);
    assert(res3 == SUCCESS);
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
    flag_parser_add_float(parser, "--float", NULL, "", &opts.probability, FLAG_OPTIONAL);

    // Test int
    const char *args_int[] = {"--int", "abc"};
    assert(flag_parser_parse(parser, 2, args_int, false) == WRONG_VALUE_TYPE);

    // Test uint (our parser util blocks negatives)
    const char *args_uint[] = {"--uint", "-5"};
    assert(flag_parser_parse(parser, 2, args_uint, false) == WRONG_VALUE_TYPE);

    // Test float
    const char *args_float[] = {"--float", "1.2.3"};
    assert(flag_parser_parse(parser, 2, args_float, false) == WRONG_VALUE_TYPE);

    // Test overflow
    const char *args_overflow[] = {"--int", "99999999999999999999999999999"};
    assert(flag_parser_parse(parser, 2, args_overflow, false) == WRONG_VALUE_TYPE);

    flag_parser_free(parser);
}

void test_mandatory_flags() {
    printf("Running test_mandatory_flags...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    // 1. Register flags
    flag_parser_add_int(parser, "--count", NULL, "", &opts.required_count, FLAG_MANDATORY);
    flag_parser_add_bool(parser, "--help", "-h", "", &opts.show_help, FLAG_OPTIONAL);

    // 2. Test 1: Missing mandatory flag
    const char *args1[] = {"-h"};
    const ParsingResult *res_parse1 = flag_parser_parse(parser, 1, args1, false);

    // Parse is fine, but validation fails
    assert(res_parse1 == SUCCESS);
    const ParsingResult *res_valid1 = flag_parser_validate(parser);
    assert(res_valid1 == MISSING_MANDATORY_FLAG);

    // 3. Test 2: All mandatory flags present
    reset_options(&opts);
    flag_parser_free(parser); // Need a fresh parser state
    parser = flag_parser_new(&opts);
    flag_parser_add_int(parser, "--count", NULL, "", &opts.required_count, FLAG_MANDATORY);
    flag_parser_add_bool(parser, "--help", "-h", "", &opts.show_help, FLAG_OPTIONAL);

    const char *args2[] = {"--count", "100", "-h"};
    const ParsingResult *res_parse2 = flag_parser_parse(parser, 3, args2, false);
    const ParsingResult *res_valid2 = flag_parser_validate(parser);

    assert(res_parse2 == SUCCESS);
    assert(res_valid2 == SUCCESS); // Validation now passes
    assert(opts.required_count == 100);
    assert(opts.show_help == true);

    flag_parser_free(parser);
}

void test_error_usage() {
    printf("Running test_error_usage...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    // 1. Register a MANDATORY flag
    flag_parser_add_int(parser, "--count", NULL, "", &opts.required_count, FLAG_MANDATORY);

    // 2. Provide it twice
    const char *args[] = {"--count", "100", "--count", "200"};
    const ParsingResult *res = flag_parser_parse(parser, 4, args, false);

    // 3. Assert failure
    assert(res == USAGE_ERROR);

    flag_parser_free(parser);
}

void test_optional_overwrite() {
    printf("Running test_optional_overwrite...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    // 1. Register an OPTIONAL flag
    flag_parser_add_int(parser, "--items", NULL, "", &opts.items, FLAG_OPTIONAL);

    // 2. Provide it twice
    const char *args[] = {"--items", "100", "--items", "200"};
    const ParsingResult *res = flag_parser_parse(parser, 4, args, false);

    // 3. Assert SUCCESS, and last one wins
    assert(res == SUCCESS);
    assert(opts.items == 200);

    flag_parser_free(parser);
}

void test_mark_visited() {
    printf("Running test_mark_visited (INI file emulation)...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    // 1. Register flags
    flag_parser_add_int(parser, "--count", NULL, "", &opts.required_count, FLAG_MANDATORY);
    flag_parser_add_string(parser, "--config", NULL, "", &opts.config_file, FLAG_OPTIONAL);

    // 2. Emulate parsing INI file
    // The INI parser finds "count" and sets it in the struct
    opts.required_count = 500;
    // It *must* also tell the flag parser it was found
    bool marked = flag_parser_mark_visited(parser, "--count");
    assert(marked == true);

    // 3. Parse command line (user only provided --config)
    const char *args[] = {"--config", "my.ini"};
    const ParsingResult *res_parse = flag_parser_parse(parser, 2, args, false);
    assert(res_parse == SUCCESS);

    // 4. Validate
    const ParsingResult *res_valid = flag_parser_validate(parser);

    // 5. Assert validation SUCCEEDS, because we marked --count as visited
    assert(res_valid == SUCCESS);
    assert(opts.required_count == 500); // Value from INI is intact
    assert(strcmp(opts.config_file, "my.ini") == 0); // Value from CLI

    flag_parser_free(parser);
}

void test_logical_grouping() {
    printf("Running test_logical_grouping...\n");
    TestOptions opts;
    reset_options(&opts); // This sets feature_a_param to UNSET_INT
    FlagParser *parser = flag_parser_new(&opts);

    // 1. Register optional flags
    flag_parser_add_bool(parser, "--feature-a", NULL, "", &opts.enable_feature_a, FLAG_OPTIONAL);
    flag_parser_add_int(parser, "--param-a", NULL, "", &opts.feature_a_param, FLAG_OPTIONAL);

    // 2. Test 1: User provides --feature-a and --param-a 0
    const char *args1[] = {"--feature-a", "--param-a", "0"};
    const ParsingResult *res1 = flag_parser_parse(parser, 3, args1, false);

    assert(res1 == SUCCESS);
    assert(opts.enable_feature_a == true);
    assert(opts.feature_a_param == 0); // Correctly parsed 0, not UNSET_INT

    // 3. Test 2: User provides only --feature-a
    reset_options(&opts);
    flag_parser_free(parser);
    parser = flag_parser_new(&opts);
    flag_parser_add_bool(parser, "--feature-a", NULL, "", &opts.enable_feature_a, FLAG_OPTIONAL);
    flag_parser_add_int(parser, "--param-a", NULL, "", &opts.feature_a_param, FLAG_OPTIONAL);

    const char *args2[] = {"--feature-a"};
    const ParsingResult *res2 = flag_parser_parse(parser, 1, args2, false);

    assert(res2 == SUCCESS);
    assert(opts.enable_feature_a == true);
    // This is the key: the library didn't touch 'feature_a_param', so it's still UNSET
    assert(opts.feature_a_param == UNSET_INT);
    // (The application would now see this and report an error)

    flag_parser_free(parser);
}

void test_terminator() {
    printf("Running test_terminator (--)...\n");
    TestOptions opts;
    reset_options(&opts);
    FlagParser *parser = flag_parser_new(&opts);

    // Register a flag and a string
    flag_parser_add_int(parser, "--items", "-i", "", &opts.items, FLAG_OPTIONAL);
    flag_parser_add_string(parser, "--out", "-o", "", &opts.output_file, FLAG_OPTIONAL);

    // Our implementation just breaks the loop at "--".
    const char *args[] = {"-i", "10", "--", "-o", "filename.txt"};
    const ParsingResult *res = flag_parser_parse(parser, 5, args, false);

    assert(res == SUCCESS);
    assert(opts.items == 10);
    assert(opts.output_file == NULL); // "--" stopped it from seeing "-o"

    flag_parser_free(parser);
}

// --- Main Test Runner ---

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