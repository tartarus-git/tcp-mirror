#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>

#include "error_handling.h"
#include "parse_args.h"
#include "networking.h"

static int program_exit_code = EXIT_SUCCESS;

void dirty_program_exit_code()
{
	program_exit_code = EXIT_FAILURE;
}

void get_program_exit_code()
{
	return program_exit_code;
}

enum signal_flags_flag_t {
	SFF_SIGINT = 1,
	SFF_SIGTERM = 2
};
static volatile uint8_t signal_flags = 0;

uint8_t get_signal_flags()
{
	if (signal_flags & ~(SFF_SIGINT | SFF_SIGTERM))
	{
		print_log(LET_BUG, "get_signal_flags: signal_flags contains unforseen signals, this shouldn't happen, cleaning signal flags and continuing...", -1);
		dirty_program_exit_code();

		signal_flags &= SFF_SIGINT | SFF_SIGTERM;
	}

	return signal_flags;
}

static void signal_handler(int signal) {
	switch (signal) {
	case SIGINT: signal_flags |= SFF_SIGINT; break;
	case SIGTERM: signal_flags |= SFF_SIGTERM; break;
	}
}

static void init_signal_handler() {
	sigaction signal_action = {
		.sa_handler = signal_handler,
		// .sa_sigaction <-- NEVER assign both sa_handler and sa_sigaction!!
		.sa_mask = 0,
		.sa_flags = 0,
		.sa_restorer = NULL
	};
	if (sigaction(SIGINT, &signal_action, NULL) < 0) {
		switch (errno) {
		case EINVAL:
			print_log_fatal_bug_and_exit("init_signal_handler: sigaction(SIGINT) returned EINVAL, shouldn't be possible", EINVAL);
		case EFAULT:
			print_log_fatal_bug_and_exit("init_signal_handler: sigaction(SIGINT) returned EFAULT, shouldn't be possible", EFAULT);
		default:
			print_log_fatal_bug_and_exit("init_signal_handler: sigaction(SIGINT) returned unknown, undocumented (at this time) error", errno);
		}
	}
	if (sigaction(SIGTERM, &signal_action, NULL) < 0) {
		switch (errno) {
		case EINVAL:
			print_log_fatal_bug_and_exit("init_signal_handler: sigaction(SIGTERM) returned EINVAL, shouldn't be possible", EINVAL);
		case EFAULT:
			print_log_fatal_bug_and_exit("init_signal_handler: sigaction(SIGTERM) returned EFAULT, shouldn't be possible", EFAULT);
		default:
			print_log_fatal_bug_and_exit("init_signal_handler: sigaction(SIGTERM) returned unknown, undocumented (at this time) error", errno);
		}
	}
}

int main(const int argc, const char * const * const argv)
{
	init_signal_handler();

	const prog_args_t args = parse_args(argc, argv);

	do_networking();

	return get_program_exit_code();
}
