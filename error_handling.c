#include "error_handling.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void print_log(log_entry_type_t entry_type, const char * const msg, int posix_errnum)
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

	// yes, <= 0 is what I meant, apparently errno will never be set to 0 for error,
	// so I assume it's an invalid value for a posix error num.
	if (posix_errnum <= 0) {
		printf("tcp-mirror: %s: %s\n", entry_type_string, msg);
	}
	else {
		printf("tcp-mirror: %s: %s (raw posix err: %s: %s)\n", entry_type_string,
								       msg,
								       strerrorname_np(posix_errnum),
								       strerror(posix_errnum))
	}
}

void print_log_fatal_error_and_exit(const char * const msg, int posix_errnum)
{
	print_log(LET_FATAL_ERROR, msg, posix_errnum);
	print_log(LET_FATAL_ERROR, "exited", -1);
	exit(EXIT_FAILURE);
}

void print_log_fatal_bug_and_exit(const char * const msg, int posix_errnum)
{
	print_log(LET_FATAL_BUG, msg, posix_errnum);
	print_log(LET_FATAL_ERROR, "exited", -1);
	exit(EXIT_FAILURE);
}
