#include "EngineManager.h"

#include <stdio.h>

#ifdef _WIN32

//#define DA_COMMON_TYPES
//#include "DynamicArray.h"
//
//DAint i;
//DAint_Init(&i, 10);
//
//for (int j = 0; j < 10; ++j) {
//	DAint_Push(&i, j);
//}
//
//for (int j = 0; j < 10; ++j) {
//	printf("%i\n", i.data[j]);
//}
//
//DAint_Free(&i);

	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>

	int main(void) {
		// Doesnt work on Windows system or w/C
		// signal(SIGTERM, Engine_Shutdown);

		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		Engine_Init();

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
		_CrtDumpMemoryLeaks();
		return 0;
	}

#else

	int main(void) {
		Engine_Init();
		return 0;
	}

#endif