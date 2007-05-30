INSTALL_PREFIX ?= /usr/local
INSTALL_LIBDIR ?= /usr/local/lib

SYS ?= $(shell uname -s)

NEEDS_DL := Darwin Linux SunOS syllable
ifneq (,$(findstring $(SYS),$(NEEDS_DL)))
LFLAGS +=-ldl
endif

NEEDS_M := FreeBSD Linux NetBSD
ifneq (,$(findstring $(SYS),$(NEEDS_M)))
LFLAGS +=-lm
endif

#NEEDS_PTHREAD := FreeBSD Linux NetBSD
#ifneq (,$(findstring $(SYS),$(NEEDS_PTHREAD)))
#LFLAGS +=-lpthread
#endif

DLL_SUFFIX := so
DLL_COMMAND := -shared -Wl,-soname="libiovmall.so"
FLAT_NAMESPACE :=

LINKDLL := $(CC)
LINKDLLOUTFLAG := -o 
LINKDIRFLAG := -L
LINKLIBFLAG := -l
DLL_LIB_SUFFIX := 
CCOUTFLAG := -o 
AROUTFLAG := 
RANLIB ?= ranlib
AR     ?= ar
ARFLAGS := rcu 

ifeq ($(SYS),Darwin)
DLL_SUFFIX := dylib
DLL_COMMAND := -dynamiclib
FLAT_NAMESPACE := -flat_namespace
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
LINKDLL := link
LINKDLLOUTFLAG :=-out:
DLL_SUFFIX := dll
DLL_COMMAND := -dll -debug /INCREMENTAL:NO -subsystem:CONSOLE 
DLL_EXTRA_LIBS := ws2_32.lib
FLAT_NAMESPACE :=
AR := link -lib
ARFLAGS :=
AROUTFLAG := -out:
VMALL := vmall_
BINARY_SUFFIX := .exe
endif

###########################

date := $(shell date +'%Y-%m-%d')

#libs := $(shell ls libs)
libs := basekit coroutine garbagecollector iovm

all: addons

testaddon:
	./_build/binaries/io_static$(BINARY_SUFFIX) addons/$(addon)/tests/run.io

vm:
	for dir in $(libs); do INSTALL_PREFIX=$(INSTALL_PREFIX) $(MAKE) -C libs/$$dir; done
	$(MAKE) vmlib
	cd tools; $(MAKE)
	mkdir -p _build/binaries || true
	cp tools/_build/binaries/* _build/binaries

addons: vm
	./_build/binaries/io_static$(BINARY_SUFFIX) build.io
	@if [ -f errors ]; then cat errors; rm errors; fi

vmlib:
	mkdir -p _build || true
	mkdir -p _build/dll || true
	$(LINKDLL) $(DLL_COMMAND) $(LINKDLLOUTFLAG)_build/dll/libiovmall.$(DLL_SUFFIX) libs/*/_build/$(VMALL)objs/*.o $(LFLAGS) $(DLL_EXTRA_LIBS)
	mkdir -p _build/lib || true
	$(AR) $(ARFLAGS) $(AROUTFLAG)_build/lib/libiovmall.a\
        libs/*/_build/$(VMALL)objs/*.o
	mkdir -p _build/headers || true
	cp libs/*/_build/headers/* _build/headers

# Unlink the io binary before copying so running Io processes will keep running
install:
	umask 022
	mkdir -p $(INSTALL_PREFIX)/bin || true
	mkdir -p $(INSTALL_LIBDIR) || true
	rm -f $(INSTALL_PREFIX)/bin/io$(BINARY_SUFFIX)
	cp _build/binaries/io$(BINARY_SUFFIX) $(INSTALL_PREFIX)/bin || true
	chmod ugo+rx $(INSTALL_PREFIX)/bin/io$(BINARY_SUFFIX)
	cp _build/binaries/io_static$(BINARY_SUFFIX) $(INSTALL_PREFIX)/bin
	chmod ugo+rx $(INSTALL_PREFIX)/bin/io_static$(BINARY_SUFFIX)  || true
	cp _build/dll/* $(INSTALL_LIBDIR)  || true
	cp _build/lib/* $(INSTALL_LIBDIR)  || true
	rm -rf $(INSTALL_LIBDIR)/io || true
	mkdir -p $(INSTALL_LIBDIR)/io || true
	cp -fR addons $(INSTALL_LIBDIR)/io
	chmod -R ugo+rX $(INSTALL_LIBDIR)/io

linkInstall:
	mkdir -p $(INSTALL_PREFIX)/bin || true
	ln -sf `pwd`/_build/binaries/io$(BINARY_SUFFIX) $(INSTALL_PREFIX)/bin
	chmod ugo+rx $(INSTALL_PREFIX)/bin/io
	ln -sf `pwd`/_build/binaries/io_static$(BINARY_SUFFIX) $(INSTALL_PREFIX)/bin
	chmod ugo+rx $(INSTALL_PREFIX)/bin/io_static$(BINARY_SUFFIX)
	ln -sf `pwd`/_build/dll/* $(INSTALL_LIBDIR)
	ln -sf `pwd`/_build/lib/* $(INSTALL_LIBDIR)
	rm -rf $(INSTALL_LIBDIR)/io || true
	mkdir -p $(INSTALL_LIBDIR)/io || true
	ln -s `pwd`/addons $(INSTALL_LIBDIR)/io/addons
	chmod -R ugo+rX $(INSTALL_LIBDIR)/io

uninstall:
	rm -rf $(INSTALL_LIBDIR)/io
	rm $(INSTALL_PREFIX)/bin/io
	rm $(INSTALL_PREFIX)/bin/io_static$(BINARY_SUFFIX)
	rm $(INSTALL_LIBDIR)/libiovmall.*

doc:
	./_build/binaries/io_static$(BINARY_SUFFIX) build.io docs

cleanDocs:
	./_build/binaries/io_static$(BINARY_SUFFIX) build.io cleanDocs

clean:
	for dir in $(libs); do \
		$(MAKE) -C libs/$$dir clean; \
	done
	
	cd tools; $(MAKE) cleanDocs; cd ..
	./_build/binaries/io_static$(BINARY_SUFFIX) build.io clean || true
	-rm -f IoBindingsInit.*
	-rm -rf _build
	-rm -rf projects/osx/build
	-rm -rf projects/osxvm/build
	cd tools; $(MAKE) clean

testvm:
	cd tools; make test

testaddons:
	_build/binaries/io$(BINARY_SUFFIX) build.io runUnitTests

test:
	$(MAKE) testvm
	$(MAKE) testaddons

dist_tar:
	-rm -f Io-*.tar.gz
	echo "#define IO_VERSION_NUMBER "$(shell date +'%Y%m%d') > libs/iovm/source/IoVersion.h
	git add libs/iovm/source/IoVersion.h 
	git commit -q --no-verify -m "setting version string for release"
	git archive --format=tar --prefix=Io-$(date)/ HEAD | gzip > Io-$(date).tar.gz
	ls -al Io-$(date).tar.gz

dist_zip:
	-rm -f Io-*.zip
	echo "#define IO_VERSION_NUMBER "$(shell date +'%Y%m%d') > libs/iovm/source/IoVersion.h
	git add libs/iovm/source/IoVersion.h 
	git commit -q --no-verify -m "setting version string for release"
	git archive --format=zip --prefix=Io-$(date)/ HEAD > Io-$(date).zip
	ls -al Io-$(date).zip
	
dist:
	$(MAKE) dist_zip

metrics:
	ls -1 libs/iovm/source/*.c | io -e 'File standardInput readLines map(asFile contents occurancesOfSeq(";")) sum .. " iovm"'
	ls -1 libs/basekit/source/*.c | io -e 'File standardInput readLines map(asFile contents occurancesOfSeq(";")) sum .. " basekit"'
	ls -1 libs/coroutine/source/*.c | io -e 'File standardInput readLines map(asFile contents occurancesOfSeq(";")) sum .. " libcoroutine"'
	ls -1 libs/*/source/*.c | io -e 'File standardInput readLines map(asFile contents occurancesOfSeq(";")) sum .. " total in core"'

aptget:
	_build/binaries/io$(BINARY_SUFFIX) build.io aptget

emerge:
	_build/binaries/io$(BINARY_SUFFIX) build.io emerge

port:
	_build/binaries/io$(BINARY_SUFFIX) build.io port

urpmi:
	_build/binaries/io$(BINARY_SUFFIX) build.io urpmi

.DEFAULT:
	./_build/binaries/io_static$(BINARY_SUFFIX) build.io -a $@
