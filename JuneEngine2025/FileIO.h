#ifndef FILEIO
#define FILEIO

#include <stdio.h>
#include <stdlib.h>

static char* FileToString(const char* fileName) {
	FILE* file;
	fopen_s(&file, fileName, "rb");

	if (!file) {
		printf("Unable to open file with name %s\n", fileName);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	rewind(file);

	char* buf = (char*)malloc(length + 1);
	if (!buf) {
		printf("Unable to allocate memory for file %s\n", fileName);
		fclose(file);
		return NULL;
	}

	fread(buf, 1, length, file);
	buf[length] = '\0';
	fclose(file);
	return buf;
}

#endif // !FILEIO
