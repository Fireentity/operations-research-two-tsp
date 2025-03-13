# Aggiungi Google Test o CUnit
enable_testing()

add_executable(cli_lib_tests
        n_opt_test.c
)

target_link_libraries(cli_lib_tests PRIVATE cli_lib)

add_test(NAME cli_lib_tests COMMAND cli_lib_tests)
