# Part of SuperNOVAS
#
# Generates headless README.md and HTML documentation 
#
# Author: Attila Kovacs

# Use the definitions project definitions
include ../config.mk

BENCHMARKS = benchmark-nutation benchmark-place

CPPFLAGS += -I../include
LDFLAGS += -L../$(LIB) -lsupernovas

.PHONY: all
all: build run

.PHONY: build
build: $(BENCHMARKS)

.PHONY: run
run: build
	@for prog in $(BENCHMARKS) ; do $(LIB_PATH_VAR)=../$(LIB) ./$${prog} ; done

.PHONY: clean
clean:

.PHONY: distclean
distclean: clean
	@rm -f $(BENCHMARKS)

benchmark-%: benchmark-%.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

# Built-in help screen for `make help`
.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  all           (default) builds and runs benchmarks."
	@echo "  build         Builds benchmarks."
	@echo "  run           Runs benchmarks."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo