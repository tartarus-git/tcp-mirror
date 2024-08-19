#include "parse_args.h"

#include <poll.h>
#include <limits.h>

#include "error_handling.h"

/*
   ROUGH PLAN:
   <addr> <port> [-c/--clients]
*/

static void parse_long_option(program_arguments_t * const prog_parsed_args, const char * const option)
{

	log_fatal_error_and_exit("invalid long option (--%s)", option);
}

// TODO: arg preprocessing. Strip leading and trailing whitespace, figure out what to do with
// contained whitespace (I can imagine a scenario where it would be nice to handle, but definitely see what other
// linux commands do first). Strip out empty arguments.

static unsigned long parse_ulong(const char * const input)
{
	// TODO: consider replacing with bug detector, at this point, empty strings should
	// already be filtered out
	if (*input == '\0') { return ULONG_MAX; }

	char *first_invalid_char = NULL;
	unsigned long result = strtoul(input, &first_invalid_char, 10);

	if (first_invalid_char == NULL) {
		// TODO: consider doing bug log here. Follow-up question: fatal or non-fatal.
		// Follow-follow-up question: if non-fatal, whats the best way to deal with the situation.
	}

	if (result == ULONG_MAX || *first_invalid_char != '\0')
	{
		return ULONG_MAX;
	}

	return result;
}

static int parse_short_options(prog_args_t * const parsed_args, const int argc, const char * const * const argv)
{
	for (const char *ptr = argv[0] + 1; *ptr != '\0'; ptr++)
	{
		switch (*ptr) {

			case 'c':
				if (*(ptr + 1) != '\0') {
					log_fatal_error_and_exit("invalid short option (-%c), requires 1 %u-bit unsigned integer argument (MUST ALSO BE LESS THAN 2^%u - 1)",
							(int)*ptr, // IMPORTANT: int cast must be there! OR ELSE UB!! minimum width in printf args is int for some reason.
							sizeof(nfds_t) * CHAR_BIT,
							sizeof(long) * CHAR_BIT);
				}
				// TODO: test if next arg exists, test if it's an option of some sort,
				// if both of those aren't true, parse it with a parse function.
				// TODO: encapsulate this general functionality into a function, otherwise
				// it's going to get way too messy really quick if we add more short options,
				// which we very probably will. Even if we weren't going to though,
				// function is still much better.
				break;

			default:
				log_fatal_error_and_exit("invalid short option (-%c)", *ptr);

		}
	}
	return next_arg;
}

static int parse_argument(prog_args_t * const parsed_args, const int argc, const char * const * const argv)
{
	// TODO: implement
	return next_arg;
}

prog_args_t parse_args(const int argc, const char * const * const argv)
{
	program_arguments_t result = { };

	for (size_t i = 1; i < argc; i++)
	{
		const char * const arg = argv[i];
		if (arg[0] == '-')
		{
			if (arg[1] == '-') {
				i = parse_long_option(&result, argc - i, argv + i);
				i--;
				continue;
			}

			i = parse_short_options(&result, argc - i, argv + i);
			i--;
			continue;
		}

		i = parse_argument(&result, argc - i, argv + i);
		i--;
	}

	return result;
}
