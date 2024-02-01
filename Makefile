
# ===============================================================================
# WARNING! You should leave this Makefile alone probably
#          To configure the build, you can edit config.mk
# ===============================================================================

include config.mk




# ===============================================================================
# Specific build targets and recipes below...
# ===============================================================================


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

# CIO locator data
.PHONY: cio_file
cio_file:
	make -C tools cio_file

.PHONY: tools
tools:
	make -C tools

.PHONY: test
test: tools
	make -C test run

.PHONY: coverage
coverage: test
	make -C test coverage

clean:
	@make -C tools clean
	@make -C test clean

distclean:
	@make -C tools clean
	@make -C test clean


# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include $(PROJECT_ROOT)/build.mk
