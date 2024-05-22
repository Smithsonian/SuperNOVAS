
# ===============================================================================
# WARNING! You should leave this Makefile alone probably
#          To configure the build, you can edit config.mk
# ===============================================================================

include config.mk

# ===============================================================================
# Specific build targets and recipes below...
# ===============================================================================


# The targets to build by default if not otherwise specified to 'make'
DEFAULT_TARGETS := static shared cio_ra.bin

# Check if there is a doxygen we can run
ifndef DOXYGEN
  DOXYGEN := $(shell which doxygen)
else
  $(shell test -f $(DOXYGEN))
endif

# If there is doxygen, build the API documentation also by default
ifeq ($(.SHELLSTATUS),0)
  DEFAULT_TARGETS += dox
else
  $(info WARNING! Doxygen is not available. Will skip 'dox' target) 
endif

.PHONY: api
api: $(DEFAULT_TARGETS)

.PHONY: static
static: lib/novas.a

.PHONY: shared
shared: lib/novas.so lib/solsys1.so lib/solsys2.so

.PHONY: solsys
solsys: obj/solsys1.o obj/eph_manager.o obj/solsys2.o obj/jplint.o obj/solsys3.o obj/solsys-ephem.o

.PHONY: test
test:
	make -C test run

.PHONY: coverage
coverage:
	make -C test coverage

.PHONY: all
all: api solsys obj/novascon.o test coverage check

.PHONY: clean
clean:
	rm -f obj VERSION README-headless.md bin/cio_file
	make -C test clean

.PHONY: distclean
distclean: clean
	rm -f lib cio_ra.bin

# Static library: novas.a
lib/novas.a: $(OBJECTS) | lib
	ar -rc $@ $^
	ranlib $@

# Shared library: novas.so -- same as supernovas.so except the builtin SONAME
lib/novas.so: $(SOURCES) | lib VERSION
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC -Wl,-soname,libnovas.so.$(shell VERSION)

# Shared library: supernovas.so -- same as novas.so except the builtin SONAME
lib/supernovas.so: $(SOURCES) | lib VERSION
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC -Wl,-soname,libsupernovas.so.$(shell VERSION)


# Shared library: solsys1.so (standalone solsys1.c functionality)
lib/solsys1.so: BUILTIN_SOLSYS1 := 0
lib/solsys1.so: $(SRC)/solsys1.c $(SRC)/eph_manager.c | lib VERSION
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC -Wl,-soname,libsolsys1.so.$(shell VERSION)

# Shared library: solsys2.so (standalone solsys2.c functionality)
lib/solsys2.so: BUILTIN_SOLSYS2 := 0
lib/solsys2.so: $(SRC)/solsys2.c $(SRC)/jplint.f | lib VERSION
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC -Wl,-soname,libsolsys2.so.$(shell VERSION)

VERSION: bin/version
	$< >> $@

.INTERMEDIATE: bin/version
bin/version: $(SRC)/version.c | bin
	$(CC) -o $@ -I$(INC) $<

# CIO locator data
.PHONY: cio_ra.bin
cio_ra.bin: bin/cio_file lib/novas.a data/CIO_RA.TXT
	bin/cio_file data/CIO_RA.TXT $@
	rm -f bin/cio_file

bin/cio_file: obj/cio_file.o | bin
	$(CC) -o $@ $^ $(LFLAGS)

obj/jplint.o: $(SRC)/jplint.f
	gfortran -c -o $@ $<

README-headless.md: README.md
	LINE=`sed -n '/\# /{=;q;}' $<` && tail -n +$$((LINE+2)) $< > $@

dox: README-headless.md

.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  api           (default) 'static', 'shared', 'cio_ra.bin' targets, and also" 
	@echo "                'dox' if 'doxygen' is available, or was specified via the"
	@echo "                DOXYGEN variable (e.g. in 'config.mk')."
	@echo "  static        Builds the static 'lib/novas.a' library."
	@echo "  shared        Builds the shared 'lib/novas.so' library."
	@echo "  cio_ra.bin    Generates the CIO locator lookup data file 'cio_ra.bin', in the"
	@echo "                destination specified in 'config.mk'."
	@echo "  dox           Compiles HTML API documentation using 'doxygen'."
	@echo "  solsys        Builds only the objects that may provide 'solarsystem()' call"
	@echo "                implemtations (e.g. 'solsys1.o', 'eph_manager.o'...)."
	@echo "  check         Performs static analysis with 'cppcheck'."
	@echo "  test          Runs regression tests."
	@echo "  coverage      Runs 'gcov' to analyze regression test coverage."
	@echo "  all           All of the above."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo

Makefile: config.mk build.mk

vpath %.c $(SRC)

# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include build.mk
