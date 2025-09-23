# Part of SuperNOVAS
#
# Generates legacy CIO vs GCRS locator table, and conversion toolkit.
#
# Author: Attila Kovacs

# Use the definitions project definitions
include ../config.mk

CPPFLAGS += -I../include
LDFLAGS += -lm

.PHONY: legacy
legacy: cio_file CIO_RA.TXT clean

.PHONY: all
all: CIO_RA.TXT cio_ra.bin clean

cio_filegen: LDFLAGS += -L../$(LIB) -lsupernovas
cio_file%: cio_file%.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

CIO_RA.TXT: cio_filegen
	$(LIB_PATH_VAR)=../$(LIB) ./cio_filegen $@

cio_ra.bin: CIO_RA.TXT cio_file
	./cio_file $< $@

.PHONY: clean
clean:
	@rm -f cio_filegen

.PHONY: distclean
distclean: clean
	@rm -f cio_file CIO_RA.TXT cio_ra.bin

# Built-in help screen for `make help`
.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  legacy        (default) 'CIO_RA.TXT' and 'cio_file' conversion tool."
	@echo "  all           'legacy' and also the converted 'cio_ra.bin'."
	@echo "  CIO_RA.TXT    Generates the legacy CIO vs GCRS location lookup table."
	@echo "  cio_file      Toolkit to convert ASCII CIO locator table to binary."
	@echo "  cio_ra.bin    Binary CIO locator table."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo