#ifndef ERROR_HANDLING_H_GUARD
#define ERROR_HANDLING_H_GUARD

typedef enum log_entry_type_t {
	LET_INFO,
	LET_WARNING,
	LET_ERROR,
	LET_FATAL_ERROR,
	LET_BUG,
	LET_FATAL_BUG
} log_entry_type_t;

void print_log(log_entry_type_t entry_type, const char * const msg);
void print_log_and_dirty(log_entry_type_t entry_type, const char * const msg);

void print_log_fatal_error_and_exit(const char * const msg);
void print_log_fatal_bug_and_exit(const char * const msg);

#endif
