#ifndef GENERIC_DATA_STRUCTURES_H_GUARD
#define GENERIC_DATA_STRUCTURES_H_GUARD

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define DECLARE_FIXED_VECTOR_TYPE(type_name, element_t, capacity) \
	typedef struct type_name { \
		element_t *data; \
		size_t length; \
	} type_name; \
	\
	type_name type_name##__construct() { \
		const type_name result = { \
			.data = malloc(sizeof(element_t) * capacity), \
			.length = 0 \
		}; \
		if (result.data == nullptr) { /* no action is needed because object state is already good */ } \
		return result; \
	} \
	\
	bool type_name##__push(type_name * const this, const element_t * const new_element) { \
		if (this->length >= capacity) { return false; } \
		this->data[this->length++] = new_element; \
		return true; \
	} \
	\
	bool type_name##__pop(type_name * const this, element_t * const element_storage) { \
		if (this->length == 0) { return false; } \
		*element_storage = this->data[--this->length]; \
		return true; \
	} \
	\
	bool type_name##__insert(type_name * const this, const element_t * const new_element, const size_t index) { \
		if (index > this->length || this->length >= capacity) { return false; } \
		memmove(this->data + index + 1, this->data + index, this->length - index) \
		return true; \
	} \
	\
	bool type_name##__remove(type_name * const this, size_t index) { \
		if (index >= this->length) { return false; } \
		memmove(this->data + index, this->data + index + 1, this->length - (index + 1)); \
		return true; \
	} \
	\
	void type_name##__release(type_name * const this) { \
		free(this->data); \
	}

#define DECLARE_RUNTIME_FIXED_VECTOR_TYPE(type_name, element_t) \
	typedef struct type_name { \
		element_t *data; \
		size_t length; \
		size_t capacity; \
	} type_name; \
	\
	type_name type_name##__construct(size_t capacity) { \
		const type_name result = { \
			.data = malloc(sizeof(element_t) * capacity), \
			.length = 0, \
			.capacity = capacity \
		}; \
		if (result.data == nullptr) { /* no action is needed because object state is already good */ } \
		return result; \
	} \
	\
	bool type_name##__push(type_name * const this, const element_t * const new_element) { \
		if (this->length >= this->capacity) { return false; } \
		this->data[this->length++] = new_element; \
		return true; \
	} \
	\
	bool type_name##__pop(type_name * const this, element_t * const element_storage) { \
		if (this->length == 0) { return false; } \
		*element_storage = this->data[--this->length]; \
		return true; \
	} \
	\
	bool type_name##__insert(type_name * const this, const element_t * const new_element, const size_t index) { \
		if (index > this->length || this->length >= this->capacity) { return false; } \
		memmove(this->data + index + 1, this->data + index, this->length - index) \
		return true; \
	} \
	\
	bool type_name##__remove(type_name * const this, size_t index) { \
		if (index >= this->length) { return false; } \
		memmove(this->data + index, this->data + index + 1, this->length - (index + 1)); \
		return true; \
	} \
	\
	void type_name##__release(type_name * const this) { \
		free(this->data); \
	}

#endif
