#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "program_exit_code.h"
#include "parse_args.h"
#include "generic_data_structures.h"
#include "error_handling.h"

/*
enum signal_flags_flag_t {
	SFF_SIGINT = 1,
	SFF_SIGTERM = 2
};
static volatile uint8_t signal_flags = 0;

// TODO: Check again about signature and about "proper" naming convention for the argument and the function name or whatever
static void signal_handler(int signal) {
	switch (signal) {
	case SIGINT: signal_flags |= SFF_SIGINT; break;
	case SIGTERM: signal_flags |= SFF_SIGTERM; break;
	}
}
*/

static void serve_connection(const int conn_socket) {
	char buffer[BUFSIZ];
	const ssize_t bytes_read = read(conn_socket, buffer, sizeof(buffer));
	if (bytes_read < 0) {
		switch (errno) {
		case EAGAIN:
			print_log_and_dirty(LET_BUG, "serve_connection: read() returned EAGAIN even though poll was set, shouldn't happen");
			return;
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

DECLARE_RUNTIME_FIXED_VECTOR_TYPE(pollfd_vector_t, pollfd)
DECLARE_RUNTIME_FIXED_VECTOR_TYPE(size_t_vector_t, size_t)

static size_t_vector_t poll_random_service_queue;
static pollfd_vector_t poll_list;

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

void add_signalfd_to_poll_list(int signal)
{
	sigset_t mask;
	if (sigemptyset(&mask) < 0) {
		log_fatal_bug_and_exit("add_signalfds_to_poll_list: sigemptyset() error (errno: %s: %s)",
				custom_strerror_name(errno),
				custom_strerror_desc(errno));
	}
	if (sigaddset(&mask, signal) < 0) {
		log_fatal_bug_and_exit("add_signalfds_to_poll_list: sigaddset() error (errno: %s: %s)",
				custom_strerror_name(errno),
				custom_strerror_desc(errno));
	}

	const int fd = signalfd(-1, mask, SFD_CLOEXEC);
	if (fd < 0) {
		log_fatal_error_and_exit("add_signalfds_to_poll_list: signalfd() error (errno: %s: %s)",
				custom_strerror_name(errno),
				custom_strerror_desc(errno));
	}

	pollfd new_poll_entry = {
		.fd = fd,
		.events = POLLIN,
		.revents = 0
	}
	if (pollfd_vector_t__push(poll_list &new_poll_entry) == false) {
		log_fatal_bug_and_exit("add_signalfds_to_poll_list: pollfd_vector_t__push() failed, capacity reached, shouldn't happen");
	}
}

int main(const int argc, const char * const * const argv)
{
	const prog_args_t args = parse_args(argc, argv);

	const int listener_socket_fd = socket(args.address->ss_family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (listener_socket_fd == -1) {
		log_fatal_error_and_exit("listener_socket_fd socket() failed (errno: %s: %s)",
				custom_strerror_name(errno),
				custom_strerror_desc(errno));
	}

	if (bind(listener_socket_fd, (const sockaddr*)args.address, address_size) < 0) {
		log_fatal_error_and_exit("bind(listener_socket_fd) failed (errno: %s: %s)",
				custom_strerror_name(errno),
				custom_strerror_desc(errno));
	}

	// max_concurrent_clients + 2 + 1 because we need extra room for signalfds and listener socket
	poll_list = pollfd_vector_t__construct(args.max_concurrent_clients + 2 + 1);
	poll_random_service_queue = size_t_vector_t__construct(args.max_concurrent_clients);

	add_signalfd_to_poll_list(SIGINT);
	add_signalfd_to_poll_list(SIGTERM);

	pollfd new_poll_entry = {
		.fd = listener_socket_fd,
		.events = POLLIN,
		.revents = 0
	};
	if (pollfd_vector_t__push(poll_list, &new_poll_entry) == false) {
		log_fatal_bug_and_exit("pollfd_vector_t__push(listener_socket_fd) failed, capacity reached, shouldn't happen");
	}

	while (true)
	{
		int poll_result = poll(poll_list.data, poll_list.length, -1);
		if (poll_result < 0)
		{
			switch (errno) {

			case EINTR:
			case EAGAIN:
				break;

			default:
				log_fatal_error_and_exit("poll(poll_list) failed (errno: %s: %s)",
						custom_strerror_name(errno),
						custom_strerror_desc(errno));

			}
		}
		else if (poll_result == 0)
		{
			log_fatal_bug_and_exit("poll(poll_list) returned 0 with timeout -1, shouldn't happen");
		}
		else
		{
			poll_random_service_queue.length = poll_result;
			// TODO: Why don't you pass this as a parameter instead of doing this weird setting thing here? You should consider doing it
			// differently.
			attend_to_hot_poll_list();
		}
	}

	return get_program_exit_code();
}
