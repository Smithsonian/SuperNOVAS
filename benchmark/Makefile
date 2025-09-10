# Part of SuperNOVAS
#
# Generates headless README.md and HTML documentation 
#
# Author: Attila Kovacs

# Use the definitions project definitions
include ../config.mk

BENCHMARKS = benchmark-nutation benchmark-place

CPPFLAGS += -I../include
LDFLAGS += -L$(LIB) -lsupernovas

.PHONY: run
run: all
	@for prog in $(BENCHMARKS) ; do ./$${prog} ; done

.PHONY: all
all: $(BENCHMARKS)

.PHONY: clean
clean:

.PHONY: distclean
distclean: clean
	@rm -f $(BENCHMARKS)