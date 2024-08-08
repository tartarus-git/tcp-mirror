#include "parse_args.h"

/*
   ROUGH PLAN:
   <addr> <port> [-c/--clients]
*/

void parse_long_option(program_arguments_t * const prog_parsed_args, const char * const option)
{
	print_log_fatal_error_and_exit("invalid long option (--%s)", option);
	//print_log(LOG_TYPE_FATAL_ERROR, "exit because of fatal error");
}

unsigned long long get_next_arg_as_ull(const char * const arg) {
	strtoull()
}

int parse_flag_options(const int argc, const char * const * const argv)
{
	for (const char *ptr = argv[0] + 1; *ptr != '\0'; ptr++)
	{
		switch (*ptr) {
			case 'c':
				break;
			default:
				print_log_fatal_error_and_exit("invalid flag option (-%c)", *ptr)
				break;
		}
	}
	return next_arg;
}

int parse_argument(const int argc, const char * const * const argv)
{
	// TODO: implement
	return next_arg;
}

program_arguments_t parse_args(const int argc, const char * const * const argv)
{
	program_arguments_t result = { };

	for (size_t i = 1; i < argc; i++)
	{
		const char * const arg = argv[i];
		if (arg[0] == '-')
		{
			if (arg[1] == '-') {
				parse_option(argc - i, argv + i);
				continue;
			}

			i = parse_flags(argc - i, argv + i);
			i--;
			continue;
		}

		i = parse_argument(argc - i, argv + i);
		i--;
	}

	return result;
}
