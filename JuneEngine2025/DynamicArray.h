#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GEN_DYNAMIC_ARRAY_TYPE(name, type) \
typedef struct name { \
	type* data; \
	size_t count; \
	size_t capacity; \
} name; \
\
inline name* name##_Init(size_t init) { \
	if (init <= 0) { printf("Invalid Initialization!\n"); return; }; \
	name* arr = (type*)calloc(1, sizeof(type)); \
	if (arr == NULL) { \
		perror("calloc\n"); \
		return; \
	}\
	arr->data = (type*)calloc(init, sizeof(type)); \
	if (arr->data == NULL) { \
		perror("calloc\n"); \
		return; \
	} \
	arr->count = 0; \
	arr->capacity = init; \
	return arr; \
};\
\
inline size_t name##_Equals(type* a, type* b) { \
	return (memcmp(a, b, sizeof(type)) == 0) ? 1 : 0; \
};\
\
inline void name##_Push(name* arr, type val) { \
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
};\
\
inline void name##_Remove(name* arr, type val) { \
	for (size_t i = 0; i < arr->count; ++i) { \
		if (name##_Equals(val, arr->data[i]) == 1) { \
			for (size_t j = i; j < arr->count - 1; ++j) { \
				arr->data[j] = arr->data[j + 1]; \
			} \
			arr->count--; \
			return; \
		} \
	}\
};\
\
inline void name##_Free(name* arr) { \
	if (arr->data == NULL) return; \
	free(arr->data); \
	arr->data = NULL; \
	arr->count = arr->capacity = 0; \
};\

#endif // !DYNAMIC_ARRAY

#ifdef DA_COMMON_TYPES
	GEN_DYNAMIC_ARRAY_TYPE(DAchar, char)
	GEN_DYNAMIC_ARRAY_TYPE(DAint, int)
	GEN_DYNAMIC_ARRAY_TYPE(DAfloat, float)
	GEN_DYNAMIC_ARRAY_TYPE(DAdouble, double)
#endif
