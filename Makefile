
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

# We'll need math functions to link
LDFLAGS += -lm

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
SHARED_TARGETS := $(LIB)/libsupernovas.so $(LIB)/libnovas.so

ifneq ($(BUILTIN_SOLSYS1),1)
  SOLSYS_TARGETS += $(OBJ)/solsys1.o $(OBJ)/eph_manager.o
  SHARED_TARGETS += $(LIB)/libsolsys1.so
endif

ifneq ($(BUILTIN_SOLSYS2),1)
  SOLSYS_TARGETS += $(OBJ)/solsys2.o
  SHARED_TARGETS += $(LIB)/libsolsys2.so
endif

ifneq ($(BUILTIN_SOLSYS3),1)
  SOLSYS_TARGETS += $(OBJ)/solsys3.o
  SHARED_TARGETS += $(LIB)/libsolsys3.so
endif

ifneq ($(BUILTIN_SOLSYS_EPHEM),1)
  SOLSYS_TARGETS += $(OBJ)/solsys-ephem.o
  SHARED_TARGETS += $(LIB)/libsolsys-ephem.so
endif

ifeq ($(CALCEPH_INTEGRATION),1)
  CPPFLAGS += -DUSE_CALCEPH=1
  SOLSYS_TARGETS += $(OBJ)/solsys-calceph.o
  SHARED_TARGETS += $(LIB)/libsolsys-calceph.so
endif

# Default target for packaging with Linux distributions
.PHONY: distro
distro: $(SHARED_TARGETS) $(DOC_TARGETS)

# Shared libraries (versioned and unversioned)
.PHONY: shared
shared: $(SHARED_TARGETS)

# Legacy static libraries (locally built)
.PHONY: static
static: $(LIB)/libnovas.a solsys

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
	rm -f $(OBJECTS) README-orig.md $(BIN)/cio_file gmon.out
	make -C test clean

# Remove all generated files
.PHONY: distclean
distclean: clean
	rm -f $(LIB)/libsupernovas.so* $(LIB)/libnovas.so* $(LIB)/libnovas.a $(LIB)/libsolsys*.so* cio_ra.bin

# ----------------------------------------------------------------------------
# The nitty-gritty stuff below
# ----------------------------------------------------------------------------

$(LIB)/libsupernovas.so: $(LIB)/libsupernovas.so.$(SO_VERSION)

$(LIB)/libsolsys1.so: $(LIB)/libsolsys1.so.$(SO_VERSION)

$(LIB)/libsolsys2.so: $(LIB)/libsolsys2.so.$(SO_VERSION)

$(LIB)/libsolsys3.so: $(LIB)/libsolsys2.so.$(SO_VERSION)

$(LIB)/libsolsys-ephem.so: $(LIB)/libsolsys-ephem.so.$(SO_VERSION)

$(LIB)/libsolsys-calceph.so: $(LIB)/libsolsys-calceph.so.$(SO_VERSION)

$(LIB)/libnovas.so: $(LIB)/libsupernovas.so

$(LIB)/libsolsys%.so.$(SO_VERSION): LDFLAGS += -L$(LIB) -lsupernovas

# Shared library: libsupernovas.so.1 -- same as novas.so except the builtin SONAME
$(LIB)/libsupernovas.so.$(SO_VERSION): $(SOURCES)

# Shared library: libsolsys1.so.1 (standalone solsys1.c functionality)
$(LIB)/libsolsys1.so.$(SO_VERSION): BUILTIN_SOLSYS1 := 0
$(LIB)/libsolsys1.so.$(SO_VERSION): $(SRC)/solsys1.c $(SRC)/eph_manager.c | $(LIB)/libsupernovas.so

# Shared library: libsolsys2.so.1 (standalone solsys2.c functionality)
$(LIB)/libsolsys2.so.$(SO_VERSION): BUILTIN_SOLSYS2 := 0
$(LIB)/libsolsys2.so.$(SO_VERSION): $(SRC)/solsys2.c | $(LIB)/libsupernovas.so

# Shared library: libsolsys3.so.1 (standalone solsys1.c functionality)
$(LIB)/libsolsys3.so.$(SO_VERSION): BUILTIN_SOLSYS3 := 0
$(LIB)/libsolsys3.so.$(SO_VERSION): $(SRC)/solsys3.c | $(LIB)/libsupernovas.so

# Shared library: libsolsys-ephem.so.1 (standalone solsys2.c functionality)
$(LIB)/libsolsys-ephem.so.$(SO_VERSION): BUILTIN_SOLSYS_EPHEM := 0
$(LIB)/libsolsys-ephem.so.$(SO_VERSION): $(SRC)/solsys-ephem.c | $(LIB)/libsupernovas.so

# Shared library: libsolsys-calceph.so.1 (standalone solsys2.c functionality)
$(LIB)/libsolsys-calceph.so.$(SO_VERSION): LDFLAGS += -lcalceph
$(LIB)/libsolsys-calceph.so.$(SO_VERSION): $(SRC)/solsys-calceph.c | $(LIB)/libsupernovas.so


# Static library: libnovas.a
$(LIB)/libnovas.a: $(OBJECTS) | $(LIB) Makefile

# CIO locator data
cio_ra.bin: data/CIO_RA.TXT $(BIN)/cio_file
	$(BIN)/cio_file $< $@

.INTERMEDIATE: $(BIN)/cio_file
bin/cio_file: $(OBJ)/cio_file.o | $(BIN)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^ $(LDFLAGS)

README-orig.md: README.md
	LINE=`sed -n '/\# /{=;q;}' $<` && tail -n +$$((LINE+2)) $< > $@

dox: README-orig.md

.INTERMEDIATE: Doxyfile.local
Doxyfile.local: Doxyfile Makefile
	sed "s:resources/header.html::g" Doxyfile > $@

# Local documentation without specialized headers. The resulting HTML documents do not have
# Google Search or Analytics tracking info.
.PHONY: local-dox
local-dox: README-orig.md Doxyfile.local
	doxygen Doxyfile.local

# Built-in help screen for `make help`
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

# This Makefile depends on the config and build snipplets.
Makefile: config.mk build.mk

# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include build.mk
