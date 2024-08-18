#ifndef PROGRAM_EXIT_CODE_H_GUARD
#define PROGRAM_EXIT_CODE_H_GUARD

#include <stdlib.h>

static int program_exit_code = EXIT_SUCCESS;

void dirty_program_exit_code();
int get_program_exit_code();

#endif
