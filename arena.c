#include "arena.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

const char* arena_printf(const char * restrict * restrict const arena_ptr, const char * restrict const format, ...)
{
	va_list arg_iterator;
	va_start(arg_iterator, format);

	const int bytes_required = vsnprintf(NULL, 0, format, arg_iterator);
	if (bytes_required < 0) {
		// TODO: consider adding emergency raw IO (write()) error notification
		exit(EXIT_FAILURE);
	}

	{
		const char * restrict const new_arena_ptr = realloc(*arena_ptr, bytes_required);
		if (new_arena_ptr == NULL) {
			print_log_fatal_error_and_exit("arena_printf: realloc() failed, out of memory");
		}
		*arena_ptr = new_arena_ptr;
	}

	const int bytes_unwritten = vsnprintf(*arena_ptr, bytes_required, format, arg_iterator);
	if (bytes_unwritten < 0) {
		// TODO: same TODO as above
		exit(EXIT_FAILURE);
	}
	if (bytes_unwritten > 0) {
		print_log_fatal_bug_and_exit("arena_printf: vsnprintf() returned unwritten bytes, shouldn't happen");
	}

	va_end(arg_iterator);

	return *arena_ptr;
}

void arena_free(void * const arena_ptr)
{
	free(arena_ptr);
}
