#ifndef ARENA_H_GUARD
#define ARENA_H_GUARD

const char* arena_printf(const char * * arena_ptr, const char * format, ...);

void arena_free(void * arena_ptr);

#endif
