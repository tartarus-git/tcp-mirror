#ifndef PARSE_ARGS_H_GUARD
#define PARSE_ARGS_H_GUARD

typedef struct prog_args_t {
	sockaddr_storage listen_socket_address;
	size_t concurrent_clients;
} prog_args_t;

prog_args_t parse_args(const int argc, const char * const * const argv);

#endif
