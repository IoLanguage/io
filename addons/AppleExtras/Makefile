#include ../../Makefile.preamble

#CC := g++

RANLIB ?= ranlib
AR     ?= ar rcu

# may need to remove --param max-inline-insns-single=500 for older versions of gccs
WARN :=-Wall -Wstrict-prototypes -Winline
#--param max-inline-insns-single=500

#-ansi -pedantic
OPTIMIZE :=-Os -fPIC

dependson := $(shell cat depends) || ""

HEADERS  :=-I. 
others := $(dependson)
others := $(addprefix -I../,$(others))
others := $(addsuffix /_headers,$(others))
HEADERS += $(others)

DLL_L := $(dependson)
DLL_L := $(addprefix -L../,$(DLL_L))
DLL_L := $(addsuffix /_dll,$(DLL_L))

DLL_l := $(dependson)
DLL_l := $(addprefix -l,$(DLL_l))

CFLAGS := $(OPTIMIZE) $(WARN) $(HEADERS) #--param max-inline-insns-single=500

# Uncommment for Coros to register their stack with Valgrind
#CFLAGS += -DUSE_VALGRIND

### PLATFORM #####################################################

SYS ?= $(shell uname -s)

ifeq ($(SYS),Darwin)
	CFLAGS += -falign-loops=16
endif

ifeq ($(SYS),IRIX)
	RANLIB ?= touch
endif 

ifeq ($(SYS),Darwin)
DLL_SUFFIX := dylib
DLL_COMMAND := -dynamiclib
FLAT_NAMESPACE := -flat_namespace
else
DLL_SUFFIX := so
DLL_COMMAND := -shared
FLAT_NAMESPACE :=
endif 

### FILES #########################################################

NAME := $(notdir $(shell pwd))
LIBR    := _lib/lib$(NAME).a
DLL     := _dll/lib$(NAME).$(DLL_SUFFIX)
infiles := $(wildcard source/*.c) $(wildcard *.S)
objects := $(notdir $(infiles))
objects := $(basename $(objects))
objects := $(addsuffix .o,$(objects))
objects := $(addprefix _objs/,$(objects))

### RULES ###########################################################

all: 
	mkdir -p _objs
	mkdir -p _headers
	mkdir -p _lib
	mkdir -p _dll
	$(MAKE) $(LIBR)
	$(MAKE) $(DLL) 
	cp source/*.h _headers

_objs/%.o: source/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBR): $(objects)
	$(AR) $@ $(objects)
	$(RANLIB) $@

$(DLL): $(objects)
	$(CC) $(DLL_COMMAND) $(FLAT_NAMESPACE) $(DLL_L) $(DLL_l) $(objects) -o $(DLL)

clean:
	rm -rf _headers
	rm -rf _objs
	rm -rf _lib
