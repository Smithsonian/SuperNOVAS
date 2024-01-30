PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

SRC = $(PROJECT_ROOT)src
INC = $(PROJECT_ROOT)include

# Compiler options
CFLAGS = -I$(INC) -Os -Wall -Wextra

# For maximum compatibility with NOVAS C 3.1, uncomment the line below
#CFLAGS += -DCOMPAT=1

# Compile library with a default solarsystem() implementation, which will be used
# only if the application does not define another implementation via calls to the
# to set_solarsystem() type functions.
DEFAULT_SOLSYS = solsys3

# Compile library with a default readeph() implementation, which will be used
# only if the application does not define another implementation via calls to the
# to set_ephem_reader() function.
DEFAULT_READEPH = readeph0

# cppcheck options
CHECKOPTS = --enable=performance,warning,portability,style --language=c --std=c90 --error-exitcode=1


ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
else ifeq ($(BUILD_MODE),linuxtools)
	CFLAGS += -g -pg -fprofile-arcs -ftest-coverage
	LDFLAGS += -pg -fprofile-arcs -ftest-coverage
	EXTRA_CLEAN += novas.gcda novas.gcno $(PROJECT_ROOT)gmon.out
	EXTRA_CMDS = rm -rf novas.gcda
endif


# You can set the default CIO locator file to use depending on where you installed it.
# By default, the library will assume '/usr/share/novas/cio_ra.bin', or simply 'cio_ra.bin'
# if the COMPAT flag is set to a nonzero value (above). Some other good locations
# for this file may be in '/usr/local/share/novas', or '/opt/share/novas' for system-wide
# availability, or in '$(HOME)/.local/share/novas' for user-specific installation.
#
#CFLAGS += -DDEFAULT_CIO_LOCATOR_FILE="/user/share/novas/cio_ra.bin"

.PHONY: all
all: static shared dox

.PHONY: static
static: lib/novas.a

.PHONY: shared
shared: lib/novas.so

# Static library: novas.a
lib/novas.a: obj/novas.o obj/nutation.o obj/eph_manager.o obj/$(DEFAULT_SOLSYS).o obj/$(DEFAULT_READEPH).o | lib
	ar -rc $@ $^
	ranlib $@

# Shared library: novas.so
lib/novas.so: $(SRC)/novas.c $(SRC)/nutation.c $(SRC)/eph_manager.c $(SRC)/$(DEFAULT_SOLSYS).c $(SRC)/$(DEFAULT_READEPH).c | lib
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC

# Regular object files
obj/%.o: $(SRC)/%.c dep/%.d obj
	$(CC) -o $@ -c $(CFLAGS) $<

# Sub-directories for build targets
dep obj lib apidoc:
	mkdir $@

# Remove intermediate files
.PHONY: clean
clean:
	rm -rf obj dep

# Remove all locally built files, effectively restoring the repo to its pristine state
.PHONY: distclean
distclean: clean
	rm -rf lib apidoc

# Static code analysis using 'cppcheck'
.PHONY: check
check:
	@echo "   [check]"
	@cppcheck -Iinclude $(CHECKOPTS) src

# Doxygen documentation (HTML and man pages) under apidocs/
.PHONY: dox
dox: $(SRC) $(INC) README.md | apidoc
	@echo "   [doxygen]"
	@doxygen

# Automatic dependence on included header files.
.PRECIOUS: dep/%.d
dep/%.d: $(SRC)/%.c dep
	@echo " > $@" \
	&& $(CC) -I$(INC) -MM -MG $< > $@.$$$$ \
	&& sed 's|\w*\.o[ :]*| obj/&|g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

