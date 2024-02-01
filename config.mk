# ===============================================================================
# Generic configuration options for 
# You can include this in your Makefile
# ===============================================================================

PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Folders in which sources and headers are located, respectively
SRC = src
INC = $(PROJECT_ROOT)/include

# Compiler options
CFLAGS = -Os -Wall -Wextra -I$(INC)

# For maximum compatibility with NOVAS C 3.1, uncomment the line below
#CFLAGS += -DCOMPAT=1

# You can set the default CIO locator file to use depending on where you installed it.
# By default, the library will assume '/usr/share/novas/cio_ra.bin', or simply 'cio_ra.bin'
# if the COMPAT flag is set to a nonzero value (above). Some other good locations
# for this file may be in '/usr/local/share/novas', or '/opt/share/novas' for system-wide
# availability, or in '$(HOME)/.local/share/novas' for user-specific installation.
#
#CFLAGS += -DDEFAULT_CIO_LOCATOR_FILE="/user/share/novas/cio_ra.bin"

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
endif

# cppcheck options
CHECKOPTS = --enable=performance,warning,portability,style --language=c --std=c90 --error-exitcode=1

# ===============================================================================
# END of user config section. 
#
# Below are some generated constants based on the one that were set above
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


