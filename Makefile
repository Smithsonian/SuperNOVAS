
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

SOLSYS_TARGETS :=
SHARED_TARGETS := lib/novas.so

ifneq ($(BUILTIN_SOLSYS1),1)
  SOLSYS_TARGETS += obj/solsys1.o obj/eph_manager.o
  SHARED_TARGETS += lib/solsys1.so
endif

ifneq ($(BUILTIN_SOLSYS2),1)
  SOLSYS_TARGETS += obj/solsys2.o obj/jplint.o
  SHARED_TARGETS += lib/solsys2.so
endif

ifneq ($(BUILTIN_SOLSYS3),1)
  SOLSYS_TARGETS += obj/solsys3.o
  SHARED_TARGETS += lib/solsys3.so
endif

ifneq ($(BUILTIN_SOLSYS_EPHEM),1)
  SOLSYS_TARGETS += obj/solsys-ephem.o
  SHARED_TARGETS += lib/solsys-ephem.so
endif



.PHONY: api
api: $(DEFAULT_TARGETS)

.PHONY: static
static: lib/novas.a solsys

.PHONY: shared
shared: $(SHARED_TARGETS)

.PHONY: solsys
solsys: $(SOLSYS_TARGETS)

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
lib/novas.so: LIBNAME := novas
lib/novas.so: $(SOURCES)

# Shared library: supernovas.so -- same as novas.so except the builtin SONAME
lib/supernovas.so: LIBNAME := supernovas
lib/supernovas.so: $(SOURCES)

# Shared library: solsys1.so (standalone solsys1.c functionality)
lib/solsys1.so: BUILTIN_SOLSYS1 := 0
lib/solsys1.so: LIBNAME := solsys1
lib/solsys1.so: $(SRC)/solsys1.c $(SRC)/eph_manager.c

# Shared library: solsys2.so (standalone solsys2.c functionality)
lib/solsys2.so: BUILTIN_SOLSYS2 := 0
lib/solsys2.so: LIBNAME := solsys2
lib/solsys2.so: $(SRC)/solsys2.c $(SRC)/jplint.f

# Shared library: solsys1.so (standalone solsys1.c functionality)
lib/solsys3.so: BUILTIN_SOLSYS3 := 0
lib/solsys3.so: LIBNAME := solsys3
lib/solsys3.so: $(SRC)/solsys3.c

# Shared library: solsys2.so (standalone solsys2.c functionality)
lib/solsys-ephem.so: BUILTIN_SOLSYS_EPHEM := 0
lib/solsys-ephem.so: LIBNAME := solsys-ephem
lib/solsys-ephem.so: $(SRC)/solsys-ephem.c

lib/%.so: | lib bin/version
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC -Wl,-soname,lib$(LIBNAME).so.$(shell bin/version major) $(LDFLAGS)

.INTERMEDIATE: bin/version
bin/version: $(SRC)/version.c | bin
	$(CC) -o $@ -I$(INC) $<

# CIO locator data
.PHONY: cio_ra.bin
cio_ra.bin: bin/cio_file lib/novas.a data/CIO_RA.TXT
	bin/cio_file data/CIO_RA.TXT $@

.INTERMEDIATE: bin/cio_file
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
