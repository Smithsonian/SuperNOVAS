
# ===============================================================================
# WARNING! You should leave this Makefile alone probably
#          To configure the build, you can edit config.mk, or else you export the 
#          equivalent shell variables prior to invoking 'make' to adjust the
#          build configuration. 
# ===============================================================================

include config.mk

# ===============================================================================
# Specific build targets and recipes below...
# ===============================================================================

# The version of the shared .so libraries
SO_VERSION := 1

# The documentation components to build
DOC_TARGETS := README-orig.md

# Check if there is a doxygen we can run
ifndef DOXYGEN
  DOXYGEN := $(shell which doxygen)
else
  $(shell test -f $(DOXYGEN))
endif

# If there is doxygen, build the API documentation also by default
ifeq ($(.SHELLSTATUS),0)
  DOC_TARGETS += dox
else
  $(info WARNING! Doxygen is not available. Will skip 'dox' target) 
endif

SOLSYS_TARGETS :=
SHARED_TARGETS := lib/libsupernovas.so lib/libnovas.so

ifneq ($(BUILTIN_SOLSYS1),1)
  SOLSYS_TARGETS += obj/solsys1.o obj/eph_manager.o
  SHARED_TARGETS += lib/libsolsys1.so
endif

ifneq ($(BUILTIN_SOLSYS2),1)
  SOLSYS_TARGETS += obj/solsys2.o obj/jplint.o
  SHARED_TARGETS += lib/libsolsys2.so
endif

ifneq ($(BUILTIN_SOLSYS3),1)
  SOLSYS_TARGETS += obj/solsys3.o
  SHARED_TARGETS += lib/libsolsys3.so
endif

ifneq ($(BUILTIN_SOLSYS_EPHEM),1)
  SOLSYS_TARGETS += obj/solsys-ephem.o
  SHARED_TARGETS += lib/libsolsys-ephem.so
endif

# Default target for packaging with Linux distributions
.PHONY: distro
distro: $(SHARED_TARGETS) cio_ra.bin $(DOC_TARGETS)

# Shared libraries (versioned and unversioned)
.PHONY: shared
shared: $(SHARED_TARGETS)

# Legacy static libraries (locally built)
.PHONY: static
static: lib/libnovas.a solsys

# solarsystem() call handler objects
.PHONY: solsys
solsys: $(SOLSYS_TARGETS)

# All of the above
.PHONY: all
all: distro static test coverage check

# Run regression tests
.PHONY: test
test:
	make -C test run

# Measure test coverage (on test set of files only)
.PHONY: coverage
coverage:
	make -C test coverage

# Remove intermediates
.PHONY: clean
clean:
	rm -f obj README-orig.md bin/cio_file
	make -C test clean

# Remove all generated files
.PHONY: distclean
distclean: clean
	rm -f lib cio_ra.bin


# ----------------------------------------------------------------------------
# The nitty-gritty stuff below
# ----------------------------------------------------------------------------

# Unversioned shared libs (for linking against)
lib/lib%.so:
	ln -sr $< $@

lib/libsupernovas.so: lib/libsupernovas.so.$(SO_VERSION)

lib/libsolsys1.so: lib/libsolsys1.so.$(SO_VERSION)

lib/libsolsys2.so: lib/libsolsys2.so.$(SO_VERSION)

lib/libnovas.so: lib/libsupernovas.so

# Share librarry recipe
lib/%.so.$(SO_VERSION) : | lib
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC -Wl,-soname,lib$(LIBNAME).so.$(SO_VERSION)

# Shared library: supernovas.so -- same as novas.so except the builtin SONAME
lib/libsupernovas.so.$(SO_VERSION): LIBNAME := supernovas
lib/libsupernovas.so.$(SO_VERSION): $(SOURCES)

# Shared library: solsys1.so (standalone solsys1.c functionality)
lib/libsolsys1.so.$(SO_VERSION): BUILTIN_SOLSYS1 := 0
lib/libsolsys1.so.$(SO_VERSION): LIBNAME := solsys1
lib/libsolsys1.so.$(SO_VERSION): $(SRC)/solsys1.c $(SRC)/eph_manager.c

# Shared library: solsys2.so (standalone solsys2.c functionality)
lib/libsolsys2.so.$(SO_VERSION): BUILTIN_SOLSYS2 := 0
lib/libsolsys2.so.$(SO_VERSION): LIBNAME := solsys2
lib/libsolsys2.so.$(SO_VERSION): $(SRC)/solsys2.c $(SRC)/jplint.f

# Shared library: solsys1.so (standalone solsys1.c functionality)
lib/libsolsys3.so.$(SO_VERSION): BUILTIN_SOLSYS3 := 0
lib/libsolsys3.so.$(SO_VERSION): LIBNAME := solsys3
lib/libsolsys3.so.$(SO_VERSION): $(SRC)/solsys3.c

# Shared library: solsys2.so (standalone solsys2.c functionality)
lib/libsolsys-ephem.so.$(SO_VERSION): BUILTIN_SOLSYS_EPHEM := 0
lib/libsolsys-ephem.so.$(SO_VERSION): LIBNAME := solsys-ephem
lib/libsolsys-ephem.so.$(SO_VERSION): $(SRC)/solsys-ephem.c


# Static library: novas.a
lib/libnovas.a: $(OBJECTS) | lib
	ar -rc $@ $^
	ranlib $@

# CIO locator data
.PHONY: cio_ra.bin
cio_ra.bin: bin/cio_file lib/libnovas.a data/CIO_RA.TXT
	bin/cio_file data/CIO_RA.TXT $@

.INTERMEDIATE: bin/cio_file
bin/cio_file: obj/cio_file.o | bin
	$(CC) -o $@ $^ $(LFLAGS)

obj/jplint.o: $(SRC)/jplint.f
	gfortran -c -o $@ $<

README-orig.md: README.md
	LINE=`sed -n '/\# /{=;q;}' $<` && tail -n +$$((LINE+2)) $< > $@

dox: README-orig.md

.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  distro        (default) 'shared', 'cio_ra.bin' targets, and also 'dox'" 
	@echo "                if 'doxygen' is available, or was specified via the DOXYGEN"
	@echo "                variable (e.g. in 'config.mk')."
	@echo "  static        Builds the static 'lib/novas.a' library."
	@echo "  shared        Builds the shared 'novas.so', 'solsys1.so', and 'solsys2.so'."
	@echo "  cio_ra.bin    Generates the CIO locator lookup data file 'cio_ra.bin', in the"
	@echo "                destination specified in 'config.mk'."
	@echo "  dox           Compiles HTML API documentation using 'doxygen'."
	@echo "  solsys        Builds only the objects that may provide external 'solarsystem()'"
	@echo "                call implentations (e.g. 'solsys1.o', 'eph_manager.o'...)."
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
