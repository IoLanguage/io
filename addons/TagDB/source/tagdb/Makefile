CC		:= gcc
RANLIB		:= ranlib
AR		:= ar rcu
SED		:= sed

WARN		:= -Wall -Wstrict-prototypes -Winline
OPTIMIZE	:= -O3
HEADERS		:= -I. -I/opt/local/include
DEFINE		:= 
LFLAGS		:= -L. -L./_libs -L/usr/local/lib -L/usr/lib
#LFLAGS		+= -lMallocDebug
LIBS		:= -lm -ltagdb -lqdbm
DLL_LIBS := -lqdbm
CCOUTFLAG := -o
DLL_LIB_SUFFIX := a
LINK_DLL := $(CC)
AROUTFLAG :=
STATIC_SUFFIX :=

ifeq ($(CC),gcc)
DFLAGS		:= -MM
else
DFLAGS		:= -M
endif

ifeq ($(PLATFORM), Darwin) 
  CFLAGS += -falign-loops=16
	DLL_SUFFIX := dylib
	DLL_COMMAND := -dynamiclib
	FLAT_NAMESPACE := -flat_namespace
	SINGLE_MODULE := -single_module
	READ_ONLY_RELOCS := -read_only_relocs
	LFLAGS := $(LFLAGS) -L/opt/local/lib
	INSTALL_DIR := /usr/local/lib/io/
	INSTALL_NAME := -install_name $(INSTALL_DIR)libtagdb.$(DLL_SUFFIX)
	SUPPRESS := suppress
endif

ifneq (,$(findstring Windows,$(SYS)))
	CC := cl -nologo
	LINKDLL := link
	LINKDLLOUTFLAG :=-out:
	DLL_SUFFIX := dll
	DLL_COMMAND := -dll -debug /INCREMENTAL:NO -subsystem:CONSOLE
	DLL_LIBS := libqdbm.lib
	FLAT_NAMESPACE :=
	SINGLE_MODULE :=
	READ_ONLY_RELOCS :=
	AR := link -lib
	ARFLAGS :=
	AROUTFLAG := -out:
	STATIC_SUFFIX := _static
	DLL_LIB_SUFFIX := lib
	LIBS		:= ./_libs/libtagdb$(STATIC_SUFFIX).lib libqdbm.lib
	LFLAGS :=
	HEADERS		:= -I.
	WARN :=
	OPTIMIZE := -Zi -MD -D_USE_MATH_DEFINES -DWIN32 -DNDEBUG -D_CRT_SECURE_NO_DEPRECATE
	DEFINE := -DLIBTAGDB_EXPORTS
	CCOUTFLAG := -Fo
	INSTALL_NAME :=
	SUPPRESS :=
endif

CPPFLAGS	:= $(HEADERS) $(DEFINE)
CFLAGS		:= $(OPTIMIZE) $(WARN) $(CPPFLAGS)

### PLATFORM #################################################################
#
# Platform specifics compile time options:
#   NEEDS_DL - Needs -ldl for dlopen and friends.
#   SANE_POPEN - popen() can return a bidirectional FILE*
#

NEEDS_DL	:= Linux
SANE_POPEN	:= FreeBSD Darwin
OS		:= $(shell uname -s)

ifneq (,$(findstring $(OS),$(NEEDS_DL)))
LIBS		+= -ldl
endif

ifneq (,$(findstring $(OS),$(SANE_POPEN)))
CFLAGS		+= -DSANE_POPEN
endif

ifeq ($(OS),IRIX)
RANLIB		:= touch
endif 

ifeq ($(OS),Darwin)
#CFLAGS          += -mdynamic-no-pic
endif


### FILES ####################################################################

EXE		:= test
STATIC_LIB		:= _libs/libtagdb$(STATIC_SUFFIX).$(DLL_LIB_SUFFIX)
OBJDIR		:= _objs

sources		:= \
			TagDB.c \
			TagDBItem.c \
			TagIdArray.c \
			KeyIdArray.c \
			Datum.c \
			SymbolDB.c \
			villa_extras.c

objects		:= $(addprefix _objs/,$(sources:.c=.o))
#depends	:= $(objects:.o=.d)


### RULES ####################################################################

all: 
	mkdir _objs || true
	mkdir _libs || true
	mkdir _include || true
	$(MAKE) $(STATIC_LIB) 
	$(MAKE) _tests/correctness
	$(MAKE) _tests/performance
	$(MAKE) _docs/sample
	$(MAKE) dll

dll:
	$(LINKDLL) $(DLL_COMMAND) $(FLAT_NAMESPACE) $(SINGLE_MODULE) $(READ_ONLY_RELOCS) $(SUPPRESS) $(INSTALL_NAME) $(LINKDLLOUTFLAG)_libs/libtagdb.$(DLL_SUFFIX) _objs/*.o $(LFLAGS) $(DLL_LIBS)
	


#-include $(depends)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< $(CCOUTFLAG)$@

$(OBJDIR)/%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) $(DFLAGS) $(CPPFLAGS) $< | \
	 sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' > $@


$(STATIC_LIB): $(objects) 
	$(AR) $(AROUTFLAG)$@ $(objects)
	$(RANLIB) $@
	cp *.h _include

_tests/correctness: _tests/correctness.c $(STATIC_LIB)
	$(CC) $(CFLAGS) $(LFLAGS) $(CCOUTFLAG)_tests/correctness _tests/correctness.c $(LIBS)

_tests/performance: _tests/performance.c $(STATIC_LIB)
	$(CC) $(CFLAGS) $(LFLAGS) $(CCOUTFLAG)_tests/performance _tests/performance.c $(LIBS)

_docs/sample: _docs/sample.c $(STATIC_LIB)
	$(CC) $(CFLAGS) $(LFLAGS) $(CCOUTFLAG)_docs/sample _docs/sample.c $(LIBS)
	
clean:
	-rm -rf _objs
	-rm -rf _include
	-rm -rf _libs
	-rm _tests/correctness
	-rm _tests/performance
	-rm _docs/sample
	-rm -rf build
	-rm testdb*

install:
	mkdir -p /usr/local/include/tagdb
	cp _include/*.h /usr/local/include/tagdb
	cp _libs/libtagdb.* /usr/local/lib
	ranlib /usr/local/lib/libtagdb.a

test:
	./_tests/correctness
	./_docs/sample
	./_tests/performance


