#CC ?= g++

WARN :=-Wall -Wstrict-prototypes
OPTIMIZE :=-Os -g
HEADERS :=-I.
LINK := $(CC)
CCOUTFLAG := -o 
LINKOUTFLAG := -o 

sublibs := basekit coroutine garbagecollector iovm
sublibs_headers := $(addprefix -I../libs/, $(sublibs))
sublibs_headers := $(addsuffix /_build/headers, $(sublibs_headers))

HEADERS += $(sublibs_headers)

#sublibs_L := $(addprefix -L../libs/, $(sublibs))
#sublibs_L := $(addsuffix /_build/lib, $(sublibs_L))
sublibs_L := $(LINKDIRFLAG)../_build/lib

#sublibs_l := $(addprefix -l, $(sublibs))
sublibs_l := $(LINKLIBFLAG)iovmall$(DLL_LIB_SUFFIX)

#sublibs_DL := $(addprefix -L../libs/, $(sublibs))
#sublibs_DL := $(addsuffix _build/dll, $(sublibs_DL))
sublibs_DL := $(LINKDIRFLAG)../_build/dll

CFLAGS = $(OPTIMIZE) $(WARN)
LFLAGS :=
#LFLAGS +=-lMallocDebug
LIBS := $(sublibs_l)

LINKDIRFLAG := -L
LINKLIBFLAG := -l
DLL_LIB_SUFFIX := 
DLL_LIB_PREFIX := lib

SYS ?= $(shell uname -s)

# NEEDS_DL - Needs -ldl for dlopen and friends.

NEEDS_DL := Darwin Linux SunOS syllable
ifneq (,$(findstring $(SYS),$(NEEDS_DL)))
LFLAGS +=-ldl -g
endif

NEEDS_M := FreeBSD Linux NetBSD DragonFly
ifneq (,$(findstring $(SYS),$(NEEDS_M)))
LFLAGS +=-lm
endif

#NEEDS_PTHREAD := FreeBSD Linux NetBSD
#ifneq (,$(findstring $(SYS),$(NEEDS_PTHREAD)))
#LFLAGS +=-lpthread
#endif

# SANE_POPEN - popen() can return a bidirectional FILE*

SANE_POPEN := Darwin FreeBSD NetBSD
ifneq (,$(findstring $(SYS),$(SANE_POPEN)))
CFLAGS +=-DSANE_POPEN
endif

DLL_SUFFIX := so
DLL_COMMAND := -shared
FLAT_NAMESPACE :=
FIXDLL :=

ifeq ($(SYS),Darwin)
DLL_SUFFIX := dylib
DLL_COMMAND := -dynamiclib
FLAT_NAMESPACE := -flat_namespace
#FIXDLL := install_name_tool _build/binaries/io$(BINARY_SUFFIX) -change _build/dll/$(DLL_LIB_PREFIX)iovmall.dylib @executable_path/$(DLL_LIB_PREFIX)iovmall.dylib
FIXDLL := install_name_tool _build/binaries/io$(BINARY_SUFFIX) -change _build/dll/$(DLL_LIB_PREFIX)iovmall.dylib @executable_path/../lib/$(DLL_LIB_PREFIX)iovmall.dylib
endif

ifeq ($(SYS),IRIX)
RANLIB ?= touch
endif


ifneq (,$(findstring CYGW,$(SYS)))
DLL_SUFFIX := dll
BINARY_SUFFIX := .exe
endif

ifneq (,$(findstring MINGW,$(SYS)))
DLL_SUFFIX := dll
BINARY_SUFFIX := .exe
endif

ifneq (,$(findstring Windows,$(SYS)))
CC := cl -nologo
CCOUTFLAG := -Fo
LINK := link -link -debug
LINKOUTFLAG := -out:
DLL_SUFFIX := dll
DLL_COMMAND := -dll
LFLAGS += ws2_32.lib kernel32.lib shell32.lib
#OPTIMIZE := -Zi -MDd -DWIN32 -D_DEBUG -DBUILDING_IOVMALL_DLL -D_CRT_SECURE_NO_DEPRECATE
OPTIMIZE := -Zi -MD -DWIN32 -DNDEBUG -DBUILDING_IOVMALL_DLL -D_CRT_SECURE_NO_DEPRECATE
WARN :=
LINKDIRFLAG := -libpath:
LINKLIBFLAG := lib
DLL_LIB_SUFFIX := .lib
DLL_LIB_PREFIX := lib
BINARY_SUFFIX := .exe
endif

### FILES ####################################################################

sources := $(wildcard *.c)
objects := $(addprefix _build/objs/,$(sources:.c=.o))

### RULES ####################################################################

all:
	mkdir -p _build/binaries
	mkdir -p _build/objs
	#$(MAKE) _build/binaries/io_static$(BINARY_SUFFIX)
	#$(MAKE) _build/binaries/io$(BINARY_SUFFIX)

#_build/objs/main.o:
	$(CC) $(CFLAGS) -c $(CCOUTFLAG)_build/objs/main.o source/main.c $(HEADERS)

#_build/binaries/io_static$(BINARY_SUFFIX): _build/objs/main.o
	$(LINK) $(LINKOUTFLAG)_build/binaries/io_static$(BINARY_SUFFIX) _build/objs/main.o ../_build/lib/$(DLL_LIB_PREFIX)iovmall.a $(LFLAGS)

#_build/binaries/io: _build/objs/main.o
	#$(CC) $(CFLAGS) $(FLAT_NAMESPACE) $(LFLAGS) -o _build/binaries/io$(BINARY_SUFFIX) _build/objs/main.o ../_build/dll/$(DLL_LIB_PREFIX)iovmall.$(DLL_SUFFIX)
	$(LINK) $(FLAT_NAMESPACE) $(LFLAGS) $(LINKOUTFLAG)_build/binaries/io$(BINARY_SUFFIX) _build/objs/main.o $(LINKDIRFLAG)../_build/dll $(LINKLIBFLAG)iovmall$(DLL_LIB_SUFFIX)

	$(FIXDLL)

clean:
	-rm -rf _build

test:
	./_build/binaries/io_static$(BINARY_SUFFIX) ../libs/iovm/tests/correctness/run.io || true

docs:
	io_static$(BINARY_SUFFIX) _docgen/DocsExtractor.io .

cleanDocs:
	-rm -f _ioCode/zzz_docs.io

speedtest:
	./io_static$(BINARY_SUFFIX) _tests/_speed/speed.io   >  _tests/_speed/SpeedResults
	python _tests/_speed/speed.py >> _tests/_speed/SpeedResults
	ruby _tests/_speed/speed.ruby >> _tests/_speed/SpeedResults
	#lua _tests/_speed/speed.lua  >> _tests/_speed/SpeedResults
	./io_static$(BINARY_SUFFIX) _tests/_speed/speedResults.io > _tests/_speed/SpeedResults.html
