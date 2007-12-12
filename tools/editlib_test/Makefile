LDFLAGS=-ledit
editlib_test: editlib_test.c

clean:
	$(RM) editlib_test

hasEditLib := $(wildcard editlib_test)
ifeq ($(strip $(hasEditLib)),)
warn:
	@echo
	@echo "Compiling libedit test                [FAILED]"
	@echo "  - Command line history disabled"
	@echo
else
warn:
endif

.PHONY: clean warn
