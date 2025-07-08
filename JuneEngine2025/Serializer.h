#ifndef SERIALIZER
#define SERIALIZER

#include <stdio.h>
#include <stdlib.h>

typedef void (*sfunc)(void*);

typedef struct stype_t {
	const char* tag;
	sfunc serialize, deserialize;
} stype_t;

typedef struct Serializer {
	FILE* file;
	const char* path;

	void* ctx;

	stype_t* types;
	int count, capacity;
} Serializer;

Serializer* Serializer_Init	(const char*, void* ctx);
void Serializer_Destroy		(Serializer*);
void Serializer_Register	(Serializer* , const char*, sfunc, sfunc);
void Serializer_Load		(Serializer*);
void Serializer_Write		(Serializer*);

#endif