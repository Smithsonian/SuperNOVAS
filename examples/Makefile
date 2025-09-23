# Part of SuperNOVAS
#
# Generates headless README.md and HTML documentation 
#
# Author: Attila Kovacs

# Use the definitions project definitions
include ../config.mk

EXAMPLES = example-star example-high-z example-orbital example-time example-rise-set

CPPFLAGS += -I../include
LDFLAGS += -L../$(LIB) -lsupernovas

ifeq ($(CALCEPH_SUPPORT),1)
  EXAMPLES += example-calceph
  LDFLAGS += -lsolsys-calceph -lcalceph
endif

ifeq ($(CSPICE_SUPPORT),1)
  EXAMPLES += example-cspice
  LDFLAGS += -lsolsys-cspice -lcspice
endif

.PHONY: all
all: $(EXAMPLES)

.PHONY: clean
clean:

.PHONY: distclean
distclean:
	rm -f $(EXAMPLES)

example-%: example-%.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)