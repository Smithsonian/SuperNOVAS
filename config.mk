# ===========================================================================
# Generic configuration options for building the SuperNOVAS libraries (both 
# static and shared).
#
# You can include this snipplet in your Makefile also.
# ============================================================================

# Folders in which sources and headers are located, respectively
SRC ?= src
INC ?= include

# Folders for compiled objects, libraries, and binaries, respectively 
OBJ ?= obj
LIB ?= lib
BIN ?= bin

# Compiler: use gcc by default
CC ?= gcc

# Add include directory
CPPFLAGS += -I$(INC)

# Base compiler options (if not defined externally...)
# -std=c99 may not be supported by some very old compilers...
CFLAGS ?= -g -Os -Wall

# Compile for specific C standard
ifdef CSTANDARD
  CFLAGS += -std=$(CSTANDARD)
endif

# Extra warnings (not supported on all compilers)
ifeq ($(WEXTRA), 1) 
  CFLAGS += -Wextra
endif

# Specific Doxygen to use if not the default one
#DOXYGEN ?= /opt/bin/doxygen

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
#DEFAULT_READEPH ?= $(SRC)/readeph0.c


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


# Whether or not to build solsys-calceph libraries. You need the calceph 
# development libraries (libcalceph.so and/or libcaclceph.a) installed in
# LD_LIBRARY_PATH, and calceph.h in /usr/include or some other accessible
# location (you may also  set an appropriate -I<path> option to CPPFLAGS 
# prior to calling make).
#CALCEPH_SUPPORT = 1


# Whether or not to build solsys-cspice libraries. You need the CSPICE 
# development libraries (libcspice.so and/or libcspice.a) installed in
# LD_LIBRARY_PATH, and CSPICE header files in /usr/include/cspice or some 
# other accessible location (you may also  set an appropriate -I<path> 
# option to CPPFLAGS prior to calling make).
#CSPICE_SUPPORT = 1


# cppcheck options for 'check' target. You can add additional options by
# setting the CHECKEXTRA variable (e.g. in shell) prior to invoking 'make'.
CHECKOPTS ?= --enable=performance,warning,portability,style --language=c \
            --error-exitcode=1 --std=c99

# Add-on ccpcheck options
CHECKOPTS += --inline-suppr $(CHECKEXTRA)

# Exhaustive checking for newer cppcheck...
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

SOURCES = $(SRC)/novas.c $(SRC)/nutation.c $(SRC)/super.c $(SRC)/timescale.c \
          $(SRC)/place.c $(SRC)/transform.c $(SRC)/spectral.c $(SRC)/frames.c \
          $(SRC)/refract.c $(SRC)/naif.c $(SRC)/parse.c 

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

ifdef DEFAULT_READEPH
  SOURCES += $(DEFAULT_READEPH)
  CPPFLAGS += -DDEFAULT_READEPH=1
endif

# Use ldconfig (if available) to detect CALCEPH / CSPICE shared libs automatically
ifneq ($(shell which ldconfig), )
  # Detect CALCEPH automatically, and enable support if present
  ifndef CALCEPH_SUPPORT 
    ifneq ($(shell ldconfig -p | grep libcalceph), )
      $(info INFO: CALCEPH support is enabled automatically.)
      CALCEPH_SUPPORT = 1
    else
      $(info INFO: optional CALCEPH support is not enabled.)
      CALCEPH_SUPPORT = 0
    endif
  endif

  # Detect CSPICE automatically, and enable support if present
  ifndef CPSICE_SUPPORT
    ifneq ($(shell ldconfig -p | grep libcspice), )
      $(info INFO: CSPICE support is enabled automatically.)
      CSPICE_SUPPORT = 1
    else
      $(info INFO: optional CSPICE support is not enabled.)
      CSPICE_SUPPORT = 0
    endif
  endif
endif

# Generate a list of object (obj/*.o) files from the input sources
OBJECTS := $(subst $(SRC),$(OBJ),$(SOURCES))
OBJECTS := $(subst .c,.o,$(OBJECTS))

# Search for files in the designated locations
vpath %.h $(INCLUDE)
vpath %.c $(SRC)
vpath %.o $(OBJ)
vpath %.d dep

