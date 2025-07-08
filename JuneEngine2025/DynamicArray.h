#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

#include "stdinc.h"
#include <string.h>

#define GEN_DYNAMIC_ARRAY_TYPE(type) \
typedef struct DA##type { \
	type* data; \
	int count; \
	int capacity; \
} DA##type; \
\
inline void DA##type##_Init(DA##type* arr, int init) { \
	if (init <= 0) { printf("Invalid Initialization!\n"); return; }; \
	if (arr->data != NULL || arr->count > 0 || arr->capacity > 0) { printf("Array is already initialized!\n"); return; };\
	arr->data = (type*)calloc(init, sizeof(type)); \
	if (arr->data == NULL) { \
		printf("Unable to allocate memory!\n"); \
		return; \
	} \
	arr->count = 0; \
	arr->capacity = init; \
}\
\
inline int DA##type##_Equals(type a, type b) { \
	return memcmp(&a, &b, sizeof(type)) == 0; \
} \
\
inline void DA##type##_Push(DA##type* arr, type val) { \
	if (arr->count >= arr->capacity) { \
		arr->capacity *= 2; \
		type* t = (type*)realloc(arr->data, arr->capacity * sizeof(type)); \
		if (t == NULL) { \
			printf("Unable to reallocate memory!\n"); \
			return; \
		} \
		arr->data = t; \
	} \
	arr->data[arr->count++] = val; \
}\
\
inline void DA##type##_Remove(DA##type* arr, type val) { \
	for (int i = 0; i < arr->count; ++i) { \
		if (DA##type##_Equals(val, arr->data[i]) == 1) { \
			for (int j = i; j < arr->count - 1; ++j) { \
				arr->data[j] = arr->data[j + 1]; \
			} \
			arr->count--; \
			return; \
		} \
	}\
}\
\
inline void DA##type##_Free(DA##type* arr) { \
	if (arr->data == NULL) return; \
	free(arr->data); \
	arr->data = NULL; \
	arr->count = arr->capacity = 0; \
}\
\

#endif // !DYNAMIC_ARRAY

#ifdef DA_COMMON_TYPES
	GEN_DYNAMIC_ARRAY_TYPE(char)
	GEN_DYNAMIC_ARRAY_TYPE(int)
	GEN_DYNAMIC_ARRAY_TYPE(float)
	GEN_DYNAMIC_ARRAY_TYPE(double)
#endif
