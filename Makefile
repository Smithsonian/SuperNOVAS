PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

SRC = $(PROJECT_ROOT)src
INC = $(PROJECT_ROOT)include

# Compiler options
CFLAGS = -I$(INC) -Os -Wall -Wextra

# For maximum compatibility with NOVAS C 3.1, uncomment the line below
#CFLAGS += -DCOMPAT=1

# Whether to build into the library specific versions of solarsystem()
#BUILTIN_SOLSYS1 = 1
#BUILTIN_SOLSYS2 = 1
BUILTIN_SOLSYS3 = 1

# Compile library with a default solarsystem() implementation, which will be used
# only if the application does not define another implementation via calls to the
# to set_solarsystem() type functions.
DEFAULT_SOLSYS = 3

# Compile library with a default readeph() implementation, which will be used
# only if the application does not define another implementation via calls to the
# to set_ephem_reader() function.
DEFAULT_READEPH = readeph0

# Compiler and linker options etc.
ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
else ifeq ($(BUILD_MODE),test)
	CFLAGS += -O0 -g -pg -fprofile-arcs -ftest-coverage
	LDFLAGS += -pg -fprofile-arcs -ftest-coverage
endif

# cppcheck options
CHECKOPTS = --enable=performance,warning,portability,style --language=c --std=c90 --error-exitcode=1


# ===============================================================================
# END of user config section, below is some logic and recipes only.
# ===============================================================================


ifeq ($(DEFAULT_SOLSYS), 1) 
  BUILTIN_SOLSYS1 = 1
  CFLAGS += -DDEFAULT_SOLSYS=1
endif

ifeq ($(DEFAULT_SOLSYS), 2)
  BUILTIN_SOLSYS2 = 1
  CFLAGS += -DDEFAULT_SOLSYS=2
endif

ifeq ($(DEFAULT_SOLSYS), 3)
  BUILTIN_SOLSYS3 = 1
  CFLAGS += -DDEFAULT_SOLSYS=3
endif

SOURCES = $(SRC)/novas.c $(SRC)/nutation.c

ifeq ($(BUILTIN_SOLSYS1), 1) 
  SOURCES += $(SRC)/solsys1.c $(SRC)/eph_manager.c 
  CFLAGS += -DBUILTIN_SOLSYS1=1
endif

ifeq ($(BUILTIN_SOLSYS2), 1) 
  SOURCES += $(SRC)/solsys2.c
  CFLAGS += -DBUILTIN_SOLSYS2=1
endif

ifeq ($(BUILTIN_SOLSYS3), 1) 
  SOURCES += $(SRC)/solsys3.c
  CFLAGS += -DBUILTIN_SOLSYS3=1
endif

ifdef (DEFAULT_READEPH) 
  SOURCES += $(SRC)/$(DEFAULT_READEPH).c
  CFLAGS += -DDEFAULT_READEPH=1
endif

OBJECTS := $(subst $(SRC),obj,$(SOURCES))
OBJECTS := $(subst .c,.o,$(OBJECTS))


# ===============================================================================
# Targets ans recipes below...
# ===============================================================================



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
lib/novas.a: $(OBJECTS) | lib
	ar -rc $@ $^
	ranlib $@

# Shared library: novas.so
lib/novas.so: $(SOURCES) | lib
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC

# Regular object files
obj/%.o: $(SRC)/%.c dep/%.d obj Makefile
	$(CC) -o $@ -c $(CFLAGS) $<

# Create sub-directories for build targets
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

