
# ===============================================================================
# WARNING! You should leave this Makefile alone probably
#          To configure the build, you can edit config.mk
# ===============================================================================

include config.mk


# ===============================================================================
# Specific build targets and recipes below...
# ===============================================================================


.PHONY: api
api: static shared cio_file dox

.PHONY: static
static: lib/novas.a

.PHONY: shared
shared: lib/novas.so

.PHONY: solsys
solsys: obj/solsys1.o obj/eph_manager.o obj/solsys2.o obj/jplint.o obj/solsys3.o obj/solsys-ephem.o

.PHONY: tools
tools: lib/novas.a
	make -C tools

.PHONY: test
test: tools
	make -C test run

.PHONY: coverage
coverage: test
	make -C test coverage

.PHONY: all
all: api solsys tools test coverage check

.PHONY: clean
clean:
	rm -f README-pruned.md
	@make -C tools clean
	@make -C test clean

.PHONY: distclean
distclean:
	@make -C tools clean
	@make -C test clean


# Static library: novas.a
lib/novas.a: $(OBJECTS) | lib
	ar -rc $@ $^
	ranlib $@

# Shared library: novas.so
lib/novas.so: $(SOURCES) | lib
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC

# CIO locator data
.PHONY: cio_file
cio_file: lib/novas.a
	make -C tools cio_file

obj/jplint.o: $(SRC)/jplint.f
	gfortran -c -o $@ $<

README-pruned.md: README.md
	tail -n +`sed -n '/\# /{=;q;}' $<` $< > $@

dox: README-pruned.md


# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include $(PROJECT_ROOT)/build.mk
