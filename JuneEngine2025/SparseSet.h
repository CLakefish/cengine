#ifndef SPARSE_SET
#define SPARSE_SET

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 

#define GEN_SPARSE_SET(name, type) \
typedef struct name { \
	size_t** sparse; \
	size_t sparseCount, sparseCapacity; \
	size_t pageSize; \
	type* dense; \
	size_t denseCount, denseCapacity; \
} name; \
\
static name* name##_Init(size_t pageSize, size_t sparseCapacity, size_t denseCapacity) { \
	name* sp = (name*)calloc(1, sizeof(name)); \
	if (!sp) { \
		perror("calloc\n"); \
		return NULL; \
	} \
	size_t pages = (sparseCapacity + pageSize - 1) / pageSize; \
	size_t** sparse = (size_t**)calloc(pages, sizeof(size_t*)); \
	if (!sparse) { \
		perror("calloc\n"); \
		return NULL; \
	}\
	\
	for (size_t i = 0; i < pages; ++i) sparse[i] = NULL; \
	\
	type* dense = (type*)calloc(denseCapacity, sizeof(type)); \
	if (!dense) { \
		perror("calloc\n"); \
		return NULL; \
	} \
	sp->sparse = sparse; \
	sp->dense = dense; \
	\
	sp->denseCapacity = denseCapacity; \
	sp->sparseCapacity = sparseCapacity;\
	sp->pageSize = pageSize; \
	return sp; \
}; \
\
static void name##_Free(name* set) { \
	size_t pageTotal = (set->sparseCapacity + set->pageSize - 1) / set->pageSize; \
	for (size_t i = 0; i < pageTotal; ++i) { \
		free(set->sparse[i]); \
	}; \
	\
	free(set->sparse); \
	free(set->dense); \
	free(set); \
};\
\
static void name##_ExpandSparse(name* set) { \
	if (!set) return; \
	size_t oldPages = (set->sparseCapacity + set->pageSize - 1) / set->pageSize; \
	set->sparseCapacity *= 2; \
	size_t newPages = (set->sparseCapacity + set->pageSize - 1) / set->pageSize; \
	\
	size_t** t = (size_t**)realloc(set->sparse, newPages * sizeof(size_t*)); \
	if (!t) { \
		perror("realloc\n"); \
		return; \
	} \
	\
	set->sparse = t; \
	for (size_t i = oldPages; i < newPages; ++i) {\
		set->sparse[i] = NULL; \
	}\
}; \
\
static void name##_ExpandDense(name* set) { \
	if (!set) return;\
	set->denseCapacity *= 2; \
	type* t = (type*)realloc(set->dense, set->denseCapacity * sizeof(type)); \
	if (!t) { \
		perror("realloc\n"); \
		return; \
	} \
	set->dense = t; \
}; \
\
static int name##_Contains(name* set, size_t ID) { \
	if (!set) return 0; \
	if (ID >= set->sparseCapacity) return 0; \
	\
	size_t page = ID / set->pageSize; \
	size_t ind  = ID % set->pageSize; \
	if (page >= set->sparseCapacity / set->pageSize || set->sparse[page] == NULL) return 0; \
	\
	size_t denseInd = set->sparse[page][ind]; \
	if (denseInd == (size_t)(-1) || denseInd >= set->denseCount) return 0; \
	\
	return (set->dense[denseInd].ID == ID) ? 1 : 0; \
}; \
\
static type* name##_Get(name* set, size_t ID) { \
	if (!set) return 0; \
	if (ID >= set->sparseCapacity) return 0; \
	\
	size_t page = ID / set->pageSize; \
	size_t ind  = ID % set->pageSize; \
	if (page >= set->sparseCapacity || set->sparse[page] == NULL) return 0; \
	\
	size_t denseInd = set->sparse[page][ind]; \
	if (denseInd == (size_t)(-1) || denseInd >= set->denseCount) return 0; \
	\
	return &set->dense[denseInd]; \
}; \
\
static type* name##_Insert(name* set, type item, size_t ID) { \
	if (name##_Contains(set, ID)) { \
		printf("Sparse set already contains ID of %zu\n", ID);\
		return NULL; \
	}\
	while (set->sparseCount >= set->sparseCapacity || ID >= set->sparseCapacity) name##_ExpandSparse(set); \
	\
	size_t page = ID / set->pageSize; \
	size_t ind  = ID % set->pageSize; \
	\
	if (set->sparse[page] == NULL) { \
		size_t* t = (size_t*)calloc(set->pageSize, sizeof(size_t)); \
		if (!t) { \
			perror("calloc\n"); \
			return NULL; \
		}\
		set->sparse[page] = t; \
	}\
	item.ID = ID; \
	\
	if (set->denseCount >= set->denseCapacity) name##_ExpandDense(set); \
    size_t prev = set->denseCount; \
	set->dense[prev] = item; \
	set->sparse[page][ind] = prev; \
	set->denseCount++; \
	set->sparseCount++; \
	return &set->dense[prev]; \
}; \
\
static void name##_Remove(name* set, size_t ID) { \
	if (!set) return; \
	if (ID >= set->sparseCapacity) return; \
	if (!name##_Contains(set, ID)) return; \
	\
	size_t page = ID / set->pageSize; \
	size_t ind  = ID % set->pageSize; \
	size_t denseInd = set->sparse[page][ind]; \
	\
	type last = set->dense[set->denseCount - 1]; \
	set->dense[set->denseCount - 1] = set->dense[denseInd]; \
	set->dense[denseInd]			= last; \
	set->denseCount--; \
	set->sparseCount--; \
	\
	size_t lastPage = last.ID / set->pageSize; \
	size_t lastInd  = last.ID % set->pageSize; \
	\
	set->sparse[page][ind]		   = set->sparse[lastPage][lastInd]; \
	set->sparse[lastPage][lastInd] = denseInd; \
	set->sparseCount++; \
};\

#endif