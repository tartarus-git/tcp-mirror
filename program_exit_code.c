#include "program_exit_code.h"

#include <stdlib.h>

void dirty_program_exit_code()
{
	program_exit_code = EXIT_FAILURE;
}

int get_program_exit_code()
{
	return program_exit_code;
}
