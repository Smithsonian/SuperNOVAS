# Adjusted configuration for test builds
BUILTIN_SOLSYS3 := 0 
BUILTIN_DEFAULT_SOLSYS := 0 
CIO_LOCATOR_FILE := cio_ra.bin

# Use above config in sub-makes also
export BUILTIN_SOLSYS3 BUILTIN_DEFAULT_SOLSYS CIO_LOCATOR_FILE

CFLAGS += -I../include -pg -Wall -fprofile-arcs -ftest-coverage
LDFLAGS += -fprofile-arcs -ftest-coverage -lm

include ../config.mk

#OBJECTS := $(subst obj/,,$(OBJECTS))
OBJECTS := novas.o nutation.o solsys3.o super.o frames.o timescale.o refract.o naif.o
TESTS := test-compat test-cio_file test-super test-errors
COVFILES := novas.c.gcov nutation.c.gcov solsys3.c.gcov super.c.gcov timescale.c.gcov refract.c.gcov frames.c.gcov naif.c.gcov

ifeq ($(CALCEPH_SUPPORT), 1)
  LDFLAGS += -lcalceph
  OBJECTS += solsys-calceph.o
  TESTS += test-calceph
  COVFILES += solsys-calceph.c.gcov
endif

ifeq ($(CSPICE_SUPPORT), 1)
  LDFLAGS += -lcspice
  OBJECTS += solsys-cspice.o
  TESTS += test-cspice
  COVFILES += solsys-cspice.c.gcov
endif

.PHONY: run
run: clean-cov clean-data $(TESTS) data ../cio_ra.bin bad-data
	./test-compat
	ln -s ../cio_ra.bin
	./test-cio_file
	sed -i "s:-nan: nan:g" data/*
	diff data reference
	./test-super
	rm -f cio_ra.bin
	./test-errors
ifeq ($(CALCEPH_SUPPORT), 1)
	./test-calceph ephem
endif
ifeq ($(CSPICE_SUPPORT), 1)
	./test-cspice ephem
endif

cov: cov.info
	genhtml $< -o $@

cov.info:
	geninfo . -b . -o $@

.PHONY: coverage

coverage: $(COVFILES)
	$(MAKE) clean-test
	$(MAKE) cov

data:
	mkdir data

.PHONY: bad-data
bad-data: ../cio_ra.bin bad-cio-data
	@touch bad-cio-data/empty
	@dd status=none if=../cio_ra.bin of=bad-cio-data/bad-1.bin bs=27 count=1
	@dd status=none if=../cio_ra.bin of=bad-cio-data/bad-2.bin bs=63 count=1
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-1.txt bs=14 count=1
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-2.txt bs=79 count=1
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-3.txt bs=21 count=1
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-4.txt bs=21 count=1
	@echo "blah" >> bad-cio-data/bad-4.txt
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-5.txt bs=79 count=1
	@echo "blah blah blah blah blah blah" >> bad-cio-data/bad-5.txt

bad-cio-data:
	mkdir $@

.PRECIOUS: %.c.gcov
%.c.gcov: %.gcno %.gcda
	gcov -b $*

.PRECIOUS: %.gcno %.gcda
%.gcno %.gcda: ../src/%.c
	$(MAKE) clean
	$(MAKE) run

../cio_ra.bin:
	$(MAKE) -C .. cio_ra.bin

test-%: test-%.o $(OBJECTS) | Makefile data
	$(CC) -o $@ $^ $(LDFLAGS)

test-%.o: src/test-%.c | Makefile
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

%.o: ../src/%.c | Makefile
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<


test-calceph: LDFLAGS += -lcalceph

test-cspice: LDFLAGS += -lcspice

.PHONY: clean-test
clean-test:
	rm -f test-*

.PHONY: clean-cov
clean-cov:
	rm -rf gmon.out *.gcov *.gcda *.gcno cov cov.info

.PHONY: clean-data
clean-data:
	rm -rf data cio_ra.bin bad-cio-data

.PHONY: clean
clean: clean-test clean-cov clean-data
	rm -f *.o

.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  run           (default) Compiles and runs regression tests."
	@echo "  coverage      Extracts test coverage data."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo


vpath %.c ../src
vpath %.c src

