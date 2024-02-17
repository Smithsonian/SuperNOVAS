
# ===============================================================================
# WARNING! You should leave this Makefile alone probably
#          To configure the build, you can edit config.mk
# ===============================================================================

include config.mk


# ===============================================================================
# Specific build targets and recipes below...
# ===============================================================================


.PHONY: api
api: static shared cio_ra.bin dox

.PHONY: static
static: lib/novas.a

.PHONY: shared
shared: lib/novas.so

.PHONY: solsys
solsys: obj/solsys1.o obj/eph_manager.o obj/solsys2.o obj/jplint.o obj/solsys3.o obj/solsys-ephem.o

.PHONY: test
test:
	make -C test run

.PHONY: coverage
coverage:
	make -C test coverage

.PHONY: all
all: api solsys test coverage check

.PHONY: clean
clean:
	rm -f object README-headless.md bin/cio_file
	@make -C test clean

.PHONY: distclean
distclean: clean
	rm -f lib cio_ra.bin
	@make -C test clean


# Static library: novas.a
lib/novas.a: $(OBJECTS) | lib
	ar -rc $@ $^
	ranlib $@

# Shared library: novas.so
lib/novas.so: $(SOURCES) | lib
	$(CC) -o $@ $(CFLAGS) $^ -shared -fPIC

# CIO locator data
.PHONY: cio_ra.bin
cio_ra.bin: bin/cio_file lib/novas.a data/CIO_RA.TXT
	bin/cio_file data/CIO_RA.TXT $@
	rm -f bin/cio_file

bin/cio_file: obj/cio_file.o | bin
	$(CC) -o $@ $^ $(LFLAGS)

obj/jplint.o: $(SRC)/jplint.f
	gfortran -c -o $@ $<

README-headless.md: README.md
	tail -n +`sed -n '/\# /{=;q;}' $<` $< > $@

dox: README-headless.md


vpath %.c $(SRC)

# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include $(PROJECT_ROOT)/build.mk
