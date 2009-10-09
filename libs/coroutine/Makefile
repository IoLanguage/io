include ./Makefile.lib
CFLAGS += -DBUILDING_CORO_DLL $(IOVMALLFLAGS)

# Manually control which coro implementation to use
#CFLAGS += -DUSE_UCONTEXT   	# preferred on OSX, Linux and friends
#CFLAGS += -DUSE_FIBERS		# preferred on Windows
#CFLAGS += -DUSE_SETJMP		# method of last resort
