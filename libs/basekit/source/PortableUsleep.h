
#ifdef ON_WINDOWS
	#include <windows.h>
#ifndef WIN32
	int usleep (unsigned int us);
#endif
#elif !defined(__CYGWIN__)
	#include <unistd.h>
#endif

