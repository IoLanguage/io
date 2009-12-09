#include "IoState.h"

void IoAddonsInit(IoObject *context);

//#define IOBINDINGS

#ifdef IO_CHECK_ALLOC
#define IO_SHOW_STATS 1
#endif

#ifdef IO_SHOW_STATS
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

double System_UserTime(void)
{
	struct rusage u;
	int r = getrusage(0, &u);
	return r == -1 ? -1 : u.ru_utime.tv_sec + (((double)u.ru_utime.tv_usec)/1000000);
}

#endif

int main(int argc, const char *argv[])
{
	int exitResult;
	IoState *self;
#ifdef IO_SHOW_STATS
	size_t t1 = clock();
	size_t maxAllocatedBytes;
	double collectorTime;
	size_t sweepCount;
#endif


	self = IoState_new();
#ifdef IOBINDINGS
	IoState_setBindingsInitCallback(self, (IoStateBindingsInitCallback *)IoAddonsInit);
#endif
	IoState_init(self);
	IoState_argc_argv_(self, argc, argv);
	//IoState_doCString_(self, "some test code...");
	IoState_runCLI(self);
	exitResult = IoState_exitResult(self);

#ifdef IO_SHOW_STATS
	maxAllocatedBytes = io_maxAllocatedBytes();
	collectorTime = Collector_timeUsed(self->collector);
	sweepCount = self->collector->sweepCount;
#endif

	IoState_free(self);

#ifdef IO_SHOW_STATS
	{
		float totalTime = (clock()-t1)/(float)CLOCKS_PER_SEC;
		printf("[ %.3fs user  %.3fs total  %.1f%% gc  %i sweeps  %i frees  %.3fmb max ]\n",
			   System_UserTime(),
			   totalTime,
			   100.0*collectorTime/totalTime,
			   (int)sweepCount,
			   (int)io_frees(),
			   maxAllocatedBytes/1048576.0);

		if(io_allocatedBytes() != 0)
		{
			printf("warning: %i bytes in %i blocks not freed:\n\n",
				   (int)io_allocatedBytes(), (int)(io_allocs() - io_frees()));
			io_showUnfreed();
		}
		else
		{
			printf("[ no memory leaks found ]\n");
		}
	}
#endif

	//printf("exitResult = %i\n", exitResult);
	return exitResult;
}
