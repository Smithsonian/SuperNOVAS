# ===========================================================================
# Generic configuration options for building the SuperNOVAS libraries (both 
# static and shared).
#
# You can include this snipplet in your Makefile also.
# ============================================================================

# Compiler: use gcc by default
CC ?= gcc

# Root directory in which files are to be installed
DESTDIR ?= /usr

# Base compiler options (if not defined externally...)
CFLAGS ?= -Os -Wall 

# Add include/ directory
CFLAGS += -I$(INC)


# Folders in which sources and headers are located, respectively
SRC ?= src
INC ?= include

# Specific Doxygen to use if not the default one
#DOXYGEN ?= /opt/bin/doxygen


# Extra warnings (not supported on all compilers)
#CFLAGS += -Wextra


# For maximum compatibility with NOVAS C 3.1, uncomment the line below
#COMPAT ?= 1


# To make SuperNOVAS thread-safe, we use thread-local storage modifier
# keywords. These were not standardized prior to C11. So while we automatically
# recognize C11 or GCC >= 3.3 to use the correct thread-local modifier keyword,
# for other compilers (e.g. Intel C, LLVM) you may need to specify it 
# explicitly here by passing the keyword via the THREAD_LOCAL definition
#
# E.g. 
#THREAD_LOCAL ?= __thread
#or
#THREAD_LOCAL ?= __declspec(thread)


# You can set the default CIO locator file to use depending on where you 
# installed it. By default, the library will assume 
# '/usr/share/novas/cio_ra.bin', or else 'cio_ra.bin' if the COMPAT flag is 
# set to a nonzero value (above). Some other good locations for this file may 
# be in '/usr/local/share/novas', or '/opt/share/novas' for system-wide
# availability, or in '$(HOME)/.local/share/novas' for user-specific 
# installation.
#
#CIO_LOCATOR_FILE ?= $(DESTDIR)/share/novas/cio_ra.bin


# Whether to build into the library planet_eph_manager() routines provided in 
# solsys1.c. Because the default readeph implementation (readeph0.c) does not
# provide useful functionality, we do not include solsys1.c in the build
# by default.
#BUILTIN_SOLSYS1 ?= 1


# Compile library with a default readeph() implementation for solsys1.c, which 
# will be used only if the application does not define another implementation 
# via calls to the to set_ephem_reader() function.
DEFAULT_READEPH ?= $(SRC)/readeph0.c


# Whether to build into the library planet_jplint() routines provided in 
# solsys2.c. Note, that if you choose to build in the functionality of 
# solsys2.c you will need to provide a jplint_() implementation and its
# dependencies such as pleph_() as well when linking your application.
# Therefore, we do not include solsys2.c by default...
#BUILTIN_SOLSYS2 ?= 1


# Whether to build into the library earth_sun_calc() routines provided in 
# solsys3.c
BUILTIN_SOLSYS3 ?= 1


# Whether to build into the library planet_ephem_reader() routines provided in 
# solsys3.c
BUILTIN_SOLSYS_EPHEM ?= 1


# Compile library with a default solarsystem() implementation. If you want to
# use your library with your own solarsystem() implementation, you should
# not set this option. In that case you must always provide a solarsystem()
# implementation when linking your application against this library.
DEFAULT_SOLSYS ?= 3


# cppcheck options for 'check' target
CHECKOPTS ?= --enable=performance,warning,portability,style --language=c \
            --error-exitcode=1

# Exhaustive checking for newer cppcheck
#CHECKOPTS += --check-level=exhaustive

# ============================================================================
# END of user config section. 
#
# Below are some generated constants based on the one that were set above
# ============================================================================

# If the COMPAT variable is set to one, then force set compatibility mode
ifeq ($(COMPAT),1)
  CPPFLAGS += -DCOMPAT=1
endif

# If the CIO_LOCATOR_FILE variable is defined, the use its definition
ifdef CIO_LOCATOR_FILE
  CPPFLAGS += -DDEFAULT_CIO_LOCATOR_FILE=\"$(CIO_LOCATOR_FILE)\"
else
  $(info WARNING! No default CIO_LOCATOR_FILE defined.)
  $(info .        Will use local 'cio_ra.bin' if present.)
endif

# If the THREAD_LOCAL variable was defined externally, use that definition to 
# specify the thread local keyword to use. 
ifdef THREAD_LOCAL
  CPPFLAGS += -DTHREAD_LOCAL=\"$(THREAD_LOCAL)"
endif

ifeq ($(DEFAULT_SOLSYS), 1) 
  BUILTIN_SOLSYS1 = 1
  CPPFLAGS += -DDEFAULT_SOLSYS=1
endif

ifeq ($(DEFAULT_SOLSYS), 2)
  BUILTIN_SOLSYS2 = 1
  CPPFLAGS += -DDEFAULT_SOLSYS=2
endif

ifeq ($(DEFAULT_SOLSYS), 3)
  BUILTIN_SOLSYS3 = 1
  CPPFLAGS += -DDEFAULT_SOLSYS=3
endif

SOURCES = $(SRC)/novas.c $(SRC)/nutation.c

ifeq ($(BUILTIN_SOLSYS1), 1) 
  SOURCES += $(SRC)/solsys1.c $(SRC)/eph_manager.c 
  CPPFLAGS += -DBUILTIN_SOLSYS1=1
endif

ifeq ($(BUILTIN_SOLSYS2), 1) 
  SOURCES += $(SRC)/solsys2.c
  CPPFLAGS += -DBUILTIN_SOLSYS2=1
endif

ifeq ($(BUILTIN_SOLSYS3), 1) 
  SOURCES += $(SRC)/solsys3.c
  CPPFLAGS += -DBUILTIN_SOLSYS3=1
endif

ifeq ($(BUILTIN_SOLSYS_EPHEM), 1) 
  SOURCES += $(SRC)/solsys-ephem.c
endif

ifdef (DEFAULT_READEPH) 
  SOURCES += $(DEFAULT_READEPH)
  CPPFLAGS += -DDEFAULT_READEPH=1
endif


# Compiler and linker options etc.
ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
endif

# Generate a list of object (obj/*.o) files from the input sources
OBJECTS := $(subst $(SRC),obj,$(SOURCES))
OBJECTS := $(subst .c,.o,$(OBJECTS))

