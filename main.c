#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "parse_args.h"
#include "generic_data_structures.h"

static int program_exit_code = EXIT_SUCCESS;

enum signal_flags_flag_t {
	SFF_SIGINT = 1,
	SFF_SIGTERM = 2
};
static volatile uint8_t signal_flags = 0;

static void signal_handler(int signal) {
	// TODO: set flags
}


DECLARE_RUNTIME_FIXED_VECTOR_TYPE(pollfd_vector_t, pollfd)
DECLARE_RUNTIME_FIXED_VECTOR_TYPE(size_t_vector_t, size_t)

static size_t_vector_t poll_random_service_queue;
static pollfd_vector_t poll_list;

static void serve_connection(const int conn_socket) {
	char buffer[BUFSIZ];
	const ssize_t bytes_read = read(conn_socket, buffer, sizeof(buffer));
	if (bytes_read < 0) {
		switch (errno) {
		case EAGAIN:
			// TODO: report non-fatal bug and set exit code and exit out of this function.
		case EINTR:
			// TODO: call signal response function.
		// TODO: go through possible errors, because some of them are clients fault probably, and shouldn't take down this server.
		default:
			// TODO: report fatal error with strerr
		}
	}

	const ssize_t bytes_to_write = bytes_read;
	while (bytes_to_write != 0) {
		const ssize_t bytes_written = write(conn_socket, buffer, bytes_to_write);
		if (bytes_written == -1) {
			switch (errno) {
				// TODO: pretty much same as above
			}
		}
		bytes_to_write -= bytes_written;
	}
}

static void attend_to_hot_poll_list()
{
	if (poll_list->length == 0) {
		// TODO: report fatal bug, supposed to be at least 1
	}
	if (poll_list->data[0].revents & POLLIN) {
		if (poll_list->length < poll_list->capacity) {
			const int new_conn_fd = accept4(poll_list->data[0].fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
			if (new_conn_fd < 0) {
				switch (errno) {
				case EAGAIN:
					// TODO: report non-fatal bug and keep going, this case shouldn't happen, set program_exit_code to
					// EXIT_FAILURE
				case EINTR:
					// TODO: what to do here? handle signals just like below right? signal handling put in extra function?
				default:
					// TODO: fatal error with strerr output
				}
			}
			const pollfd new_pollfd = {
				.fd = new_conn_fd,
				.events = POLLIN,
				.revents = 0
			};
			if (pollfd_vector_t__push(poll_list, new_pollfd) == false) {
				// TODO: report fatal bug, this shouldn't happen in this case because it's checked above
			}
		}
	}

	for (size_t i = 1; i < poll_list->length; i++) {
		if (poll_list->data[i].revents & POLLIN == false) { continue; }

		const long queue_index = random() % poll_random_service_queue.length;
		while (poll_random_service_queue.data[queue_index] != -1) {
			queue_index = (queue_index + 1) % poll_random_service_queue.length;
		}
		poll_random_service_queue.data[queue_index] = i;
	}

	for (size_t i = 0; i < poll_random_service_queue.length; i++) {
		serve_connection(poll_random_service_queue.data[i]);
	}
}

int main(const int argc, const char * const * const argv)
{
	const prog_args_t args = parse_args(argc, argv);

	const int listener_socket_fd = socket(args.address->ss_family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (listener_socket_fd == -1) {
		// TODO: report fatal error here, failed to create listener socket, return errno and use strerr syscall to get error string.
	}

	if (bind(listener_socket_fd, (const sockaddr*)args.address, address_size) < 0) {
		// TODO: report fatal error here, failed to bind listener socket, return errno with strerr.
		// TODO: check to make sure you don't need special gai_strerr-like thing to create string from errno for all these syscalls.
	}

	// max_concurrent_clients + 1 because we need extra room for listener socket
	poll_random_service_queue = size_t_vector_t__construct(args.max_concurrent_clients);
	poll_list = pollfd_vector_t__construct(args.max_concurrent_clients + 1);
	pollfd new_poll_entry = {
		.fd = listener_socket_fd,
		.events = POLLIN,
		.revents = 0
	};
	if (pollfd_vector_t__push(poll_list, &new_poll_entry) == false) {
		// TODO: report fatal bug here
	}

	while (true) {
		int poll_result = poll(poll_list.data, poll_list.length, -1);
		if (poll_result < 0) {
			switch (errno) {
			case EINTR:
				if (signal_flags != 0) { goto poll_loop_escape; }
				break;
			case EAGAIN:
				break;
			default:
				// TODO: report fatal poll error with strerr
			}
		}
		else if (poll_result == 0) {
			// TODO: report fatal bug here, this case shouldn't happen because timeout is -1
		}
		else {
			poll_random_service_queue.length = poll_result;
			attend_to_hot_poll_list();
		}
	}
poll_loop_escape:

	if (signal_flags & ~(SFF_SIGINT | SFF_SIGTERM)) {
		// TODO: report non-fatal bug signal flags had unexpected value
		// TODO: Set return value to 1
	}

	if (signal_flags & SFF_SIGINT) {
		// TODO: report sigint and exit
	}
	if (signal_flags & SFF_SIGTERM) {
		// TODO: report sigterm and exit
	}

	return program_exit_code;
}
