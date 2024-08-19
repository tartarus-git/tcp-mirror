#include "error_handling.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "program_exit_code.h"
#include "arena.h"

void print_log(log_entry_type_t entry_type, const char * const msg, ...)
{
	va_list arg_iterator;
	va_start(arg_iterator, msg);

	print_log__internal(entry_type, msg, arg_iterator);

	va_end(arg_iterator);
}

void print_log__internal(log_entry_type_t entry_type, const char * const msg, va_list arg_iterator)
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

	if (vprintf(arena_printf(&print_log_arena, "tcp-mirror: %s: %s\n", entry_type, msg), arg_iterator) < 0) {
		// TODO: raw IO last ditch error output?
	}
}

void print_log_and_dirty(log_entry_type_t entry_type, const char * const msg, ...)
{
	// we want to try to get to this no matter what, so having it here at the top in front of everything else is good
	dirty_program_exit_code();

	va_list arg_iterator;
	va_start(arg_iterator, msg);

	print_log__internal(entry_type, msg, arg_iterator);

	va_end(arg_iterator);
}

void print_log_fatal_error_and_exit(const char * const msg, ...)
{
	va_list arg_iterator;
	va_start(arg_iterator, msg);

	print_log__internal(LET_FATAL_ERROR, msg, arg_iterator);
	print_log(LET_FATAL_ERROR, "exited");

	va_end(arg_iterator);

	exit(EXIT_FAILURE);
}

void print_log_fatal_bug_and_exit(const char * const msg, ...)
{
	va_list arg_iterator;
	va_start(arg_iterator, msg);

	print_log(LET_FATAL_BUG, msg, addon, arg_iterator);
	print_log(LET_FATAL_ERROR, "exited");

	va_end(arg_iterator);

	exit(EXIT_FAILURE);
}

const char* custom_strerror_name(int posix_error)
{
	const char * const result = strerrorname_np(posix_error);
	return result != NULL ? result : "UNKNOWN";
}

const char* custom_strerror_desc(int posix_error)
{
	const char * const result = strerrordesc_np(posix_error);
	if (result == NULL) {
		return arena_printf(&strerror_desc_arena, "Unknown error %i", posix_error);
	}
	return result;
}
