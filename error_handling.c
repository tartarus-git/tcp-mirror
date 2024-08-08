#include "error_handling.h"

#include <stdlib.h>
#include <stdio.h>

void print_log(log_entry_type_t entry_type, const char * const msg)
{
	const char *entry_type_string;
	switch (entry_type) {
	case LET_INFO: entry_type_string = "INFO"; break;
	case LET_WARNING: entry_type_string = "WARNING"; break;
	case LET_ERROR: entry_type_string = "ERROR"; break;
	case LET_FATAL_ERROR: entry_type_string = "FATAL ERROR"; break;
	case LET_BUG: entry_type_string = "BUG"; break;
	case LET_FATAL_BUG: entry_type_string = "FATAL BUG"; break;
	}

	printf("tcp-mirror: %s: %s\n", entry_type_string, msg);
}

void print_log_fatal_error_and_exit(const char * const msg)
{
	print_log(LET_FATAL_ERROR, msg);
	print_log(LET_FATAL_ERROR, "exited");
	exit(EXIT_FAILURE);
}

void print_log_fatal_bug_and_exit(const char * const msg)
{
	print_log(LET_FATAL_BUG, msg);
	print_log(LET_FATAL_ERROR, "exited");
	exit(EXIT_FAILURE);
}
