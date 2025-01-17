
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

ifeq ($(CALCEPH_SUPPORT),1)
  CPPFLAGS += -DUSE_CALCEPH=1
  SOLSYS_TARGETS += $(OBJ)/solsys-calceph.o
  SHARED_TARGETS += $(LIB)/libsolsys-calceph.so
endif

ifeq ($(CSPICE_SUPPORT),1)
  CPPFLAGS += -DUSE_CSPICE=1
  SOLSYS_TARGETS += $(OBJ)/solsys-cspice.o
  SHARED_TARGETS += $(LIB)/libsolsys-cspice.so
endif

# Default target for packaging with Linux distributions
.PHONY: distro
distro: $(SHARED_TARGETS) $(DOC_TARGETS)

# Shared libraries (versioned and unversioned)
.PHONY: shared
shared: check-cio-locator $(SHARED_TARGETS)

# Legacy static libraries (locally built)
.PHONY: static
static: check-cio-locator $(LIB)/libnovas.a solsys

# solarsystem() call handler objects
.PHONY: solsys
solsys: $(SOLSYS_TARGETS)

# All of the above
.PHONY: all
all: distro static test coverage analyze

# Run regression tests
.PHONY: test
test:
	$(MAKE) -C test run

# Perform checks (test + analyze)
.PHONY: check
check: test analyze

# Measure test coverage (on test set of files only)
.PHONY: coverage
coverage:
	$(MAKE) -C test coverage

# Remove intermediates
.PHONY: clean
clean:
	rm -f $(OBJECTS) README-orig.md $(BIN)/cio_file gmon.out
	$(MAKE) -C test clean

# Remove all generated files
.PHONY: distclean
distclean: clean
	rm -f $(LIB)/libsupernovas.so* $(LIB)/libsupernovas.a $(LIB)/libnovas.so* $(LIB)/libnovas.a $(LIB)/libsolsys*.so* cio_ra.bin

.PHONY:
check-cio-locator:
ifndef CIO_LOCATOR_FILE
	  $(info WARNING! No default CIO_LOCATOR_FILE defined. Will use local 'cio_ra.bin' if present.)
endif


# ----------------------------------------------------------------------------
# The nitty-gritty stuff below
# ----------------------------------------------------------------------------

$(LIB)/libsupernovas.so: $(LIB)/libsupernovas.so.$(SO_VERSION)

$(LIB)/libsolsys1.so: $(LIB)/libsolsys1.so.$(SO_VERSION)

$(LIB)/libsolsys2.so: $(LIB)/libsolsys2.so.$(SO_VERSION)

$(LIB)/libsolsys3.so: $(LIB)/libsolsys2.so.$(SO_VERSION)

$(LIB)/libsolsys-ephem.so: $(LIB)/libsolsys-ephem.so.$(SO_VERSION)

$(LIB)/libsolsys-calceph.so: $(LIB)/libsolsys-calceph.so.$(SO_VERSION)

$(LIB)/libsolsys-cspice.so: $(LIB)/libsolsys-cspice.so.$(SO_VERSION)

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

# Shared library: libsolsys-cspice.so.1 (standalone solsys2.c functionality)
$(LIB)/libsolsys-cspice.so.$(SO_VERSION): LDFLAGS += -lcspice
$(LIB)/libsolsys-cspice.so.$(SO_VERSION): $(SRC)/solsys-cspice.c | $(LIB)/libsupernovas.so

# Static library: libsupernovas.a
$(LIB)/libsupernovas.a: $(OBJECTS) | $(LIB) Makefile

$(LIB)/libnovas.a: $(LIB)/libsupernovas.a
	@rm -f $@
	( cd $(LIB); ln -s libsupernovas.a libnovas.a )

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

# Default values for install locations
# See https://www.gnu.org/prep/standards/html_node/Directory-Variables.html 
prefix ?= /usr
exec_prefix ?= $(prefix)
libdir ?= $(exec_prefix)/lib
includedir ?= $(prefix)/include
datarootdir ?= $(prefix)/share
datadir ?= $(datarootdir)
mydatadir ?= $(datadir)/supernovas
docdir ?= $(datarootdir)/doc/supernovas
htmldir ?= $(docdir)/html

# Standard install commands
INSTALL_PROGRAM ?= install
INSTALL_DATA ?= install -m 644

.PHONY: install
install: install-libs install-cio-data install-headers install-html install-examples

.PHONY: install-libs
install-libs:
ifneq ($(wildcard $(LIB)/*),)
	@echo "installing libraries to $(DESTDIR)$(libdir)"
	install -d $(DESTDIR)$(libdir)
	cp $(LIB)/lib* $(DESTDIR)$(libdir)/
else
	@echo "WARNING! Skipping libs install: needs 'shared' and/or 'static'"
endif

.PHONY: install-cio-data
install-cio-data:
	@echo "installing CIO locator data to $(DESTDIR)$(mydatadir)"
	install -d $(DESTDIR)$(mydatadir)
	$(INSTALL_DATA) data/CIO_RA.TXT $(DESTDIR)$(mydatadir)/CIO_RA.TXT

.PHONY: install-headers
install-headers:
	@echo "installing headers to $(DESTDIR)$(includedir)"
	install -d $(DESTDIR)$(includedir)
	$(INSTALL_DATA) -D include/* $(DESTDIR)$(includedir)/

.PHONY: install-html
install-html:
ifneq ($(wildcard apidoc/html/search/*),)
	@echo "installing API documentation to $(DESTDIR)$(htmldir)"
	install -d $(DESTDIR)$(htmldir)/search
	$(INSTALL_DATA) -D apidoc/html/search/* $(DESTDIR)$(htmldir)/search/
	$(INSTALL_DATA) -D apidoc/html/*.* $(DESTDIR)$(htmldir)/
	@echo "installing images to $(DESTDIR)$(htmldir)/resources"
	install -d $(DESTDIR)$(htmldir)/resources
	$(INSTALL_DATA) -D resources/SuperNOVAS-systems.png $(DESTDIR)$(htmldir)/resources/
	@echo "installing Doxygen tag file to $(DESTDIR)$(docdir)"
	install -d $(DESTDIR)$(docdir)
	$(INSTALL_DATA) apidoc/supernovas.tag $(DESTDIR)$(docdir)/supernovas.tag
else
	@echo "WARNING! Skipping apidoc install: needs doxygen and 'local-dox'"
endif

.PHONY: install-examples
install-examples:
	@echo "installing examples to $(DESTDIR)$(docdir)"
	install -d $(DESTDIR)$(docdir)
	$(INSTALL_DATA) -D examples/* $(DESTDIR)$(docdir)/


# Some standard GNU targets, that should always exist...
.PHONY: html
html: local-dox

.PHONY: dvi
dvi:

.PHONY: ps
ps:

.PHONY: pdf
pdf:


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
	@echo "  test          Runs regression tests."
	@echo "  analyze       Performs static code analysis with 'cppcheck'."
	@echo "  check         Same as 'test' and then 'analyze'."
	@echo "  coverage      Runs 'gcov' to analyze regression test coverage."
	@echo "  all           All of the above."
	@echo "  install       Install components (e.g. 'make prefix=<path> install')"
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo

# This Makefile depends on the config and build snipplets.
Makefile: config.mk build.mk

# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include build.mk
