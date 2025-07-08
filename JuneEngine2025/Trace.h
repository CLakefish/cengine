#ifndef TRACE
#define TRACE

#define TRACE_LOG "trace.txt"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

typedef enum TraceType {
	TR_LOG = 0,
	TR_SUCCESS = 1,
	TR_WARNING = 2,
	TR_ERROR = 3,
} TraceType;

typedef enum TraceInit {
	TR_INIT_NONE = 0,
	TR_INIT_PRINT = 1,
} TraceInit;

struct {
	FILE* file;
	int print;
} Trace;

static void Trace_Init(TraceInit print) {
	fopen_s(&Trace.file, "trace.txt", "w");

	if (Trace.file == NULL) {
		perror("Could not open file for writing!");
		return;
	}

	Trace.print = print;
}

static inline void Trace_Shutdown(void) {
	fclose(Trace.file);
}

static inline void Trace_Clear(void) {
	freopen_s(&Trace.file, TRACE_LOG, "w", Trace.file);
}

static void Trace_Log(TraceType type, const char* format, ...) {
	const char* prefix = "\0";

	switch (type) {
		default: 
			break;
		case TR_WARNING:
			prefix = "[WARNING] \0";
			break;
		case TR_SUCCESS:
			prefix = "[SUCCESS] \0";
			break;
		case TR_ERROR:
			prefix = "[ERROR] \0";
			break;
	}

	fprintf(Trace.file, prefix);
	if (Trace.print) printf("%s", prefix);

	va_list args;
	va_start(args, format);
		if (Trace.print) vprintf(format, args);
		vfprintf(Trace.file, format, args);
	va_end(args);
}
#endif