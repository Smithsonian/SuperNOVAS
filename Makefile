
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
  DOC_TARGETS += local-dox
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
  SOLSYS_TARGETS += obj/solsys2.o
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
distro: $(SHARED_TARGETS) $(DOC_TARGETS)

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
	rm -f obj README-orig.md bin/cio_file gmon.out
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

SO_LINK := $(LDFLAGS) -lm

# Share librarry recipe
lib/%.so.$(SO_VERSION) : | lib
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^ -shared -fPIC -Wl,-soname,$(subst lib/,,$@) $(SO_LINK)

lib/libsolsys%.so.$(SO_VERSION): SO_LINK += -Llib -lsupernovas

# Shared library: supernovas.so -- same as novas.so except the builtin SONAME
lib/libsupernovas.so.$(SO_VERSION): $(SOURCES)

# Shared library: solsys1.so (standalone solsys1.c functionality)
lib/libsolsys1.so.$(SO_VERSION): BUILTIN_SOLSYS1 := 0
lib/libsolsys1.so.$(SO_VERSION): $(SRC)/solsys1.c $(SRC)/eph_manager.c | lib/libsupernovas.so

# Shared library: solsys2.so (standalone solsys2.c functionality)
lib/libsolsys2.so.$(SO_VERSION): BUILTIN_SOLSYS2 := 0
lib/libsolsys2.so.$(SO_VERSION): $(SRC)/solsys2.c | lib/libsupernovas.so

# Shared library: solsys1.so (standalone solsys1.c functionality)
lib/libsolsys3.so.$(SO_VERSION): BUILTIN_SOLSYS3 := 0
lib/libsolsys3.so.$(SO_VERSION): $(SRC)/solsys3.c | lib/libsupernovas.so

# Shared library: solsys2.so (standalone solsys2.c functionality)
lib/libsolsys-ephem.so.$(SO_VERSION): BUILTIN_SOLSYS_EPHEM := 0
lib/libsolsys-ephem.so.$(SO_VERSION): $(SRC)/solsys-ephem.c | lib/libsupernovas.so


# Static library: novas.a
lib/libnovas.a: $(OBJECTS) | lib
	ar -rc $@ $^
	ranlib $@

# CIO locator data
cio_ra.bin: data/CIO_RA.TXT bin/cio_file lib/libnovas.a
	bin/cio_file $< $@

.INTERMEDIATE: bin/cio_file
bin/cio_file: obj/cio_file.o | bin
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^ $(LDFLAGS)

README-orig.md: README.md
	LINE=`sed -n '/\# /{=;q;}' $<` && tail -n +$$((LINE+2)) $< > $@

dox: README-orig.md

.INTERMEDIATE: Doxyfile.local
Doxyfile.local:
	sed "s:resources/header.html::g" Doxyfile > $@

# Local documentation without specialized headers. The resulting HTML documents do not have
# Google Search or Analytics tracking info.
local-dox: README-orig.md Doxyfile.local
	doxygen Doxyfile.local


.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  distro        (default) 'shared' targets and also 'local-dox' provided 'doxygen'" 
	@echo "                is available, or was specified via the DOXYGEN variable (e.g. in"
	@echo "                'config.mk')."
	@echo "  static        Builds the static 'lib/libsupernovas.a' library."
	@echo "  shared        Builds the shared 'libsupernovas.so', 'libsolsys1.so', and" 
	@echo "                'libsolsys2.so' libraries (linked to versioned ones)."
	@echo "  local-dox     Compiles local HTML API documentation using 'doxygen'."
	@echo "  solsys        Builds only the objects that may provide external 'solarsystem()'"
	@echo "                call implentations (e.g. 'solsys1.o', 'eph_manager.o'...)."
	@echo "  cio_ra.bin    Generates a platform-specific binary CIO locator lookup data file"
	@echo "                'cio_ra.bin' from the ASCII 'data/CIO_RA.TXT'."
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
