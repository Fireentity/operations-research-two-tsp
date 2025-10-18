#include <c_util.h>
#include <flag.h>
#include <parsing_result.h>

struct FlagState {
    const unsigned int number_of_params;
    const char* label;
    bool mandatory;
    const ParsingResult* (* const parse_function)(CmdOptions* cmd_options, const char** arg);
};

static bool is_mandatory(const Flag* const self) {
    return self->state->mandatory;
}

static unsigned int get_number_of_params(const Flag* const self) {
    return self->state->number_of_params;
}

static const char* get_label(const Flag* const self) {
    return self->state->label;
}

static const ParsingResult* parse(const Flag* flag,
                                  CmdOptions* const cmd_options,
                                  const char** argv,
                                  unsigned int* index) {
    const ParsingResult* result = flag->state->parse_function(cmd_options, argv + *index);
    if (result->state == PARSE_SUCCESS)
        *index += flag->state->number_of_params;
    return result;
}

const Flag* init_flag_with_children(const char* label,
                                    const unsigned int number_of_params,
                                    const ParsingResult* (*const param_function)(
                                        CmdOptions* cmd_option,
                                        const char** arg
                                    ),
                                    const bool mandatory,
                                    struct FlagsArray* children) {
    const FlagState state = {
        .number_of_params = number_of_params,
        .parse_function = param_function,
        .label = memdup(label, strlen(label) + 1),
        .mandatory = mandatory
    };
    const Flag flag = {
        .state = memdup(&state, sizeof(state)),
        .parse = parse,
        .is_mandatory = is_mandatory,
        .get_number_of_params = get_number_of_params,
        .get_label = get_label,
        .children = children
    };
    return memdup(&flag, sizeof(flag));
}

const Flag* init_flag(const char* label,
                      const unsigned int number_of_params,
                      const ParsingResult* (*const param_function)(CmdOptions* cmd_option, const char** arg),
                      const bool mandatory) {
    return init_flag_with_children(label, number_of_params, param_function, mandatory, EMPTY_FLAGS_ARRAY_HEAP);
}

void free_flags_array(struct FlagsArray* self) {
    if (!self) return;
    if (self->flags) {
        for (int i = 0; i < self->number_of_flags; i++) {
            free_flag(self->flags[i]);
        }
        free(self->flags);
    }
    free(self);
}

void free_flag_state(FlagState* self) {
    if (!self) return;
    if (self->label) free((void*)self->label);
    free(self);
}

void free_flag(Flag* self) {
    if (!self) return;
    free_flags_array(self->children);
    free_flag_state((FlagState*)self->state);
    free(self);
}
