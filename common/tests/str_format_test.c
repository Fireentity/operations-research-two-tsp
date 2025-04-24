#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_util.h"

static void test_string_only(void) {
    char *s = str_format("Hello, %s!", "World");
    assert(s != NULL);
    assert(strcmp(s, "Hello, World!") == 0);
    free(s);
}

static void test_multiple_placeholders(void) {
    char *s = str_format("%s scored %d out of %.1f on %s.",
                        "Alice", 42, 99.5, "her test");
    assert(s != NULL);
    assert(strcmp(s, "Alice scored 42 out of 99.5 on her test.") == 0);
    free(s);
}

static void test_integer_formats(void) {
    // decimal, hex, octal, unsigned
    char *s = str_format("d=%d, x=%x, o=%o, u=%u", -15, 0x1A, 0755, 300u);
    assert(s != NULL);
    assert(strcmp(s, "d=-15, x=1a, o=755, u=300") == 0);
    free(s);
}

static void test_float_precision(void) {
    // default precision is 6 decimals
    char *s1 = str_format("pi approx: %f", 3.141592653589793);
    assert(s1 != NULL);
    assert(strcmp(s1, "pi approx: 3.141593") == 0);
    free(s1);
    // custom precision
    char *s2 = str_format("pi truncated: %.3f", 3.141592653589793);
    assert(s2 != NULL);
    assert(strcmp(s2, "pi truncated: 3.142") == 0);
    free(s2);
}

static void test_no_placeholders(void) {
    char *s = str_format("Just a plain string, no args.");
    assert(s != NULL);
    assert(strcmp(s, "Just a plain string, no args.") == 0);
    free(s);
}

static void test_empty_string(void) {
    char *s = str_format("");
    assert(s != NULL);
    assert(strcmp(s, "") == 0);
    free(s);
}

static void test_percent_escape(void) {
    // literal %%
    char *s = str_format("100%% sure");
    assert(s != NULL);
    assert(strcmp(s, "100% sure") == 0);
    free(s);
}

int main(void) {
    test_string_only();
    test_multiple_placeholders();
    test_integer_formats();
    test_float_precision();
    test_no_placeholders();
    test_empty_string();
    test_percent_escape();

    puts("All tests passed!");
    return 0;
}
