
#ifdef ON_WINDOWS
#include <windows.h>
#if !defined(WIN32) && !defined(__MINGW32__)
int usleep(unsigned int us);
#endif
#elif !defined(__CYGWIN__)
#include <unistd.h>
#endif
