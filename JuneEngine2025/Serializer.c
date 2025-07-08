#pragma once
#include <string.h>

#include "Serializer.h"
#include "Trace.h"

static void Serializer_Expand(Serializer* serializer) 
{
	serializer->capacity *= 2;

	stype_t* t = (stype_t*)realloc(serializer->types, sizeof(stype_t) * serializer->capacity);

	if (!t) 
	{
		perror("Unable to reallocate for expansion!");
		return;
	}

	serializer->types = t;
}

Serializer* Serializer_Init(const char* filePath, void* ctx) 
{
	Serializer* s = (Serializer*)malloc(sizeof(Serializer));

	if (!s) 
	{
		perror("Could not initialize serializer!");
		return NULL;
	}

	s->count	= 0;
	s->capacity = 1;
	s->path		= filePath;
	s->types	= (stype_t*)calloc(1, sizeof(stype_t));
	s->ctx		= ctx;

	if (!s->types) 
	{
		perror("Unable to initialize internal memory for serializer!");
		return NULL;
	}

	return s;
}

void Serializer_Destroy(Serializer* serializer) 
{
	free(serializer->types);
	free(serializer);
	serializer = NULL;
}

void Serializer_Register(Serializer* serializer, const char* type, sfunc sFunc, sfunc dsFunc) 
{
	if (serializer == NULL || serializer->types == NULL) 
	{
		Trace_Log(TR_ERROR, "Serializer uninitialized! Unable to register type of %s in serializer.\n", type);
		return;
	}

	stype_t t = (stype_t){ type, sFunc, dsFunc };
	if (serializer->count >= serializer->capacity) Serializer_Expand(serializer);
	serializer->types[serializer->count++] = t;
}

void Serializer_Load(Serializer* serializer) 
{
	if (serializer->types == NULL) 
	{
		Trace_Log(TR_ERROR, "Serializer uninitialized!\n");
		return;
	}

	fopen_s(&serializer->file, serializer->path, "rb");

	if (serializer->file == NULL) {
		Trace_Log(TR_ERROR, "Unable to open filepath!");
		return;
	}

	while (1) {
		char   buf[128] = { 0 };
		int    pos = 0;
		char   curr;
		size_t bytesRead;

		while ((bytesRead = fread(&curr, 1, 1, serializer->file)) > 0 && curr != ':') {
			if (curr == '\n' || curr == '\r' || curr == '\0') continue;

			buf[pos] = curr;
			pos++;
		}

		if (feof(serializer->file)) {
			break;
		}

		buf[pos] = '\0';

		int found = 0;
		for (int i = 0; i < serializer->count; ++i) {
			const char* tag = serializer->types[i].tag;
			if (strcmp(buf, tag) == 0) {
				found = 1;
				//printf("Found! Type is %s\n", serializer->types[i].tag);

				if (serializer->types[i].deserialize == NULL) {
					Trace_Log(TR_WARNING, "Type of %s does not have deserialize function definition.\n", serializer->types[i].tag);
					//printf("Type of %s does not have deserialize function definition.\n", serializer->types[i].tag);
					break;
				}

				serializer->types[i].deserialize(serializer->ctx);
				break;
			}
		}

		if (!found) {
			Trace_Log(TR_ERROR, "Unable to find type with tag '%s'\n", buf);
		}
	}

	fclose(serializer->file);
	serializer->file = NULL;
}

void Serializer_Write(Serializer* serializer) 
{
	if (serializer->types == NULL)
	{
		Trace_Log(TR_ERROR, "Serializer uninitialized!\n");
		return;
	}

	fopen_s(&serializer->file, serializer->path, "wb");

	if (serializer->file == NULL) {
		Trace_Log(TR_ERROR, "Unable to open filepath!");
		return;
	}

	char endl = '\n';
	for (int i = 0; i < serializer->count; ++i) {
		if (serializer->types[i].serialize == NULL) {
			Trace_Log(TR_WARNING, "Type of %s does not have serialize function definition.\n", serializer->types[i].tag);
			continue;
		}

		char buf[129] = { 0 };
		strcpy_s(buf, 128, serializer->types[i].tag);
		size_t len = strlen(buf);
		buf[len] = ':';
		buf[len + 1] = '\0';

		fwrite(&buf, sizeof(char), len + 1, serializer->file);

		serializer->types[i].serialize(serializer->ctx);
		fwrite(&endl, sizeof(char), 1, serializer->file);
	}

	fclose(serializer->file);
	serializer->file = NULL;
}