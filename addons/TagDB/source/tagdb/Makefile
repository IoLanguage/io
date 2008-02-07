CC		:= gcc
RANLIB		:= ranlib
AR		:= ar rcu
SED		:= sed

WARN		:= -Wall -Wstrict-prototypes -Winline
OPTIMIZE	:= -O3
INCLUDE		:= -I. -I/opt/local/include
DEFINE		:= 

ifeq ($(CC),gcc)
DFLAGS		:= -MM
else
DFLAGS		:= -M
endif

CPPFLAGS	:= $(INCLUDE) $(DEFINE)
CFLAGS		:= $(OPTIMIZE) $(WARN) $(CPPFLAGS)
LFLAGS		:= -L. -L./_libs -L/opt/local/lib
#LFLAGS		+= -lMallocDebug
LIBS		:= -lm -ltagdb -lqdbm

ifeq ($(PLATFORM), Darwin) 
  CFLAGS += -falign-loops=16
	DLL_SUFFIX := dylib
	DLL_COMMAND := -dynamiclib
	FLAT_NAMESPACE := -flat_namespace
	SINGLE_MODULE := -single_module
	READ_ONLY_RELOCS := -read_only_relocs
endif

ifneq (,$(findstring Windows,$(SYS)))
CC := cl -nologo
LINKDLL := link
LINKDLLOUTFLAG :=-out:
DLL_SUFFIX := dll
DLL_COMMAND := -dll -debug /INCREMENTAL:NO -subsystem:CONSOLE
FLAT_NAMESPACE :=
SINGLE_MODULE :=
READ_ONLY_RELOCS :=
AR := link -lib
ARFLAGS :=
AROUTFLAG := -out:
VMALL := vmall_
BINARY_SUFFIX := .exe
endif

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
LIB		:= _libs/libtagdb.a
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
	$(MAKE) $(LIB) 
	$(MAKE) _tests/correctness
	$(MAKE) _tests/performance
	$(MAKE) _docs/sample
	$(MAKE) dll

dll:
	$(CC) $(DLL_COMMAND) $(FLAT_NAMESPACE) $(SINGLE_MODULE) $(READ_ONLY_RELOCS) suppress -install_name /usr/local/lib/io/libtagdb.dylib -o _libs/libtagdb.dylib _objs/*.o -L/usr/local/lib -L/usr/lib -L/opt/local/lib -lqdbm
	


#-include $(depends)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) $(DFLAGS) $(CPPFLAGS) $< | \
	 sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' > $@


$(LIB): $(objects) 
	$(AR) $@ $(objects)
	$(RANLIB) $@
	cp *.h _include

_tests/correctness: _tests/correctness.c $(LIB)
	$(CC) $(CFLAGS) $(LFLAGS) -o _tests/correctness _tests/correctness.c $(LIBS)

_tests/performance: _tests/performance.c $(LIB)
	$(CC) $(CFLAGS) $(LFLAGS) -o _tests/performance _tests/performance.c $(LIBS)

_docs/sample: _docs/sample.c $(LIB)
	$(CC) $(CFLAGS) $(LFLAGS) -o _docs/sample _docs/sample.c $(LIBS)
	
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


