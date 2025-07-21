#ifndef BENCHMARKER
#define BENCHMARKER

typedef struct Benchmarker {
	double start, end;
} Benchmarker;

void	Benchmark_Start(Benchmarker* marker);
void	Benchmark_End(Benchmarker* marker);
double	Benchmark_Difference(const Benchmarker* marker);
double	Benchmark_DifferenceMicro(const Benchmarker* marker);

/// <summary>
/// Returns the difference of a benchmarker in seconds
/// </summary>
/// <param name="marker"></param>
/// <returns></returns>
double Benchmark_Difference(const Benchmarker* marker) {
	return marker->end - marker->start;
}

double Benchmark_DifferenceMicro(const Benchmarker* marker) {
	return Benchmark_Difference(marker) * 1000000;
}

#ifdef _WIN32

#include <windows.h>

static double b_Freq = 0.0;

void Benchmark_Init(void) {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	b_Freq = (double)freq.QuadPart;
}

void Benchmark_Start(Benchmarker* marker) {
	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);

	marker->start = (double)start.QuadPart / b_Freq;
}

void Benchmark_End(Benchmarker* marker) {
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	marker->end = (double)end.QuadPart / b_Freq;
}

#else
// NOT IMPLEMENTED!
#endif

#endif 